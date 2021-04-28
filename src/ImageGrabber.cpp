/**
 * Image Grabber
 * Application for grabbing images from Basler cameras using Pylon API.
 * @file            ImageGrabber.cpp
 * @author          Daniel Konecny (xkonec75)
 * @organisation    Brno University of Technology - Faculty of Information Technologies
 * @date            28. 04. 2021
 */

#include <iostream>
#include <chrono>
#include <thread>
#include <stdexcept>
#include <pylon/PylonIncludes.h>
#include <pylon/BaslerUniversalInstantCamera.h>

#include "ImageGrabber.h"
#include "ConfigurationEventPrinter.h"

using namespace std;
using namespace Pylon;
using namespace GenApi;


ImageGrabber::ImageGrabber(const ArgumentsParser &parser) {
    CTlFactory &tlFactory = CTlFactory::GetInstance();

    // Get all attached devices and exit application if no device is found.
    DeviceInfoList_t devices;
    if (tlFactory.EnumerateDevices(devices) == 0) {
        throw RUNTIME_EXCEPTION("No camera present."); // NOLINT(hicpp-exception-baseclass)
    }

    // Create an array of instant cameras for the found devices and avoid exceeding a maximum number of devices.
    const size_t maxCamerasToUse = 10;
    cameras.Initialize(min(devices.size(), maxCamerasToUse));
    cameraCount = cameras.GetSize();

    // Create and attach all Pylon Devices.
    for (size_t cameraIndex = 0; cameraIndex < cameraCount; cameraIndex++) {
        cameras[cameraIndex].Attach(tlFactory.CreateDevice(devices[cameraIndex]));

        ConfigureCamera(cameras[cameraIndex], parser, cameraIndex);
    }
}


void ImageGrabber::ConfigureCamera(CBaslerUniversalInstantCamera &camera, const ArgumentsParser &parser, size_t index) {
    // Register an image event handler that accesses the chunk data.
    camera.RegisterConfiguration(
            new CSoftwareTriggerConfiguration, RegistrationMode_ReplaceAll, Cleanup_Delete);

    imageHandlers.push_back(new ImageEventHandler());
    camera.RegisterImageEventHandler(imageHandlers[index], RegistrationMode_Append,
                                     Cleanup_Delete);

    if (parser.IsVerbose()) {
        cout << "Using device " << camera.GetDeviceInfo().GetModelName() << endl;

        camera.RegisterConfiguration(
                new ConfigurationEventPrinter, RegistrationMode_Append, Cleanup_Delete);
    }

    camera.Open();

    imageHandlers[index]->SetProgramParams(parser);
    imageHandlers[index]->SetCameraParams(camera, parser);
    imageHandlers[index]->SetTimeOffset(camera);
    imageHandlers[index]->OpenLogFile();

    if (!parser.IsImage()) {
        imageHandlers[index]->OpenVidOutput(parser.GetFrameRate());
    }

    if (parser.IsVerbose()) {
        imageHandlers[index]->PrintCameraState();
    }

    // Enable chunks in general.
    if (!camera.ChunkModeActive.TrySetValue(true))
        throw RUNTIME_EXCEPTION("The camera doesn't support chunk features"); // NOLINT(hicpp-exception-baseclass)

    // Enable time stamp chunks.
    camera.ChunkSelector.SetValue(Basler_UniversalCameraParams::ChunkSelector_Timestamp);
    camera.ChunkEnable.SetValue(true);
}

void ImageGrabber::ResetCamera(CBaslerUniversalInstantCamera &camera, const ArgumentsParser &parser, size_t index) {
    // Create a device info object for remembering the camera properties.
    CDeviceInfo info;

    // Remember the camera properties that allow detecting the same camera again.
    info.SetDeviceClass(camera.GetDeviceInfo().GetDeviceClass());
    info.SetSerialNumber(camera.GetDeviceInfo().GetSerialNumber());

    // Destroy the Pylon Device representing the detached camera device. It can't be used anymore.
    camera.DestroyDevice();

    // Create a filter containing the CDeviceInfo object info which describes the properties of the device we are looking for.
    DeviceInfoList_t filter;
    filter.push_back(info);

    CTlFactory &tlFactory = CTlFactory::GetInstance();
    while (true) {
        // Try to find the camera we are looking for.
        DeviceInfoList_t devices;

        // The camera has been found. Create and attach it to the Instant Camera object.
        if (tlFactory.EnumerateDevices(devices, filter) > 0) {
            camera.Attach(tlFactory.CreateDevice(devices[0]));
            break;
        }

        WaitObject::Sleep(250);
    }

    // Register an image event handler that accesses the chunk data.
    camera.RegisterConfiguration(
            new CSoftwareTriggerConfiguration, RegistrationMode_ReplaceAll, Cleanup_Delete);

    if (parser.IsVerbose()) {
        cout << "Using device " << camera.GetDeviceInfo().GetModelName() << endl;

        camera.RegisterConfiguration(
                new ConfigurationEventPrinter, RegistrationMode_Append, Cleanup_Delete);
    }

    camera.Open();

    imageHandlers[index]->SetTimeOffset(camera);

    // Enable chunks in general.
    if (!camera.ChunkModeActive.TrySetValue(true))
        throw RUNTIME_EXCEPTION( // NOLINT(hicpp-exception-baseclass)
                "The camera doesn't support chunk features");

    // Enable time stamp chunks.
    camera.ChunkSelector.SetValue(Basler_UniversalCameraParams::ChunkSelector_Timestamp);
    camera.ChunkEnable.SetValue(true);
}

void ImageGrabber::Grab(const ArgumentsParser &parser) {
    unsigned long long int waitTime = 1000000 / parser.GetFrameRate();

    while (true) {
        try {
            cameras.StartGrabbing(GrabStrategy_OneByOne, GrabLoop_ProvidedByInstantCamera);

            while (cameras.IsGrabbing()) {
                for (size_t cameraIndex = 0; cameraIndex < cameraCount; cameraIndex++) {
                    cameras[cameraIndex].WaitForFrameTriggerReady(5000, TimeoutHandling_ThrowException);
                    cameras[cameraIndex].ExecuteSoftwareTrigger();
                }
                this_thread::sleep_for(chrono::microseconds(waitTime));
            }
        }
        catch (const GenericException &e) {
            cameras.StopGrabbing();

            // Known issue: Wait until the system safely detects a possible removal.
            WaitObject::Sleep(1000);

            // An exception occurred. Is it because the camera device has been physically removed?
            if (cameras.IsCameraDeviceRemoved()) {
                for (size_t cameraIndex = 0; cameraIndex < cameraCount; cameraIndex++) {
                    ResetCamera(cameras[cameraIndex], parser, cameraIndex);
                }
            } else {
                throw;
            }
        }
    }
}

