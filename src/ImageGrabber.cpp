/**
 * Image Grabber
 * Application for grabbing images from Basler cameras using Pylon API.
 * @file            ImageGrabber.cpp
 * @author          Daniel Konecny (xkonec75)
 * @organisation    Brno University of Technology - Faculty of Information Technologies
 * @date            30. 04. 2021
 */

#include <iostream>
#include <chrono>
#include <ctime>
#include <thread>
#include <stdexcept>
#include <pylon/PylonIncludes.h>
#include <pylon/BaslerUniversalInstantCamera.h>

#include "ImageGrabber.h"
#include "ConfigurationEventPrinter.h"

using namespace std;
using namespace Pylon;
using namespace GenApi;
using namespace Basler_UniversalCameraParams;

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

    cout << GetDateTime() << "Number of cameras found: " << cameraCount << "." << endl;

    // Create and attach all Pylon Devices.
    for (size_t cameraIndex = 0; cameraIndex < cameraCount; cameraIndex++) {
        cameras[cameraIndex].Attach(tlFactory.CreateDevice(devices[cameraIndex]));

        ConfigureCamera(cameras[cameraIndex], parser, cameraIndex);
    }
}

/**
 * Reset parameters of cameras. Set auto settings to off for later use.
 */
ImageGrabber::~ImageGrabber() {
    for (size_t cameraIndex = 0; cameraIndex < cameraCount; cameraIndex++) {
        cameras[cameraIndex].ExposureAuto.SetValue(ExposureAuto_Off);
        cameras[cameraIndex].GainAuto.SetValue(GainAuto_Off);
        cameras[cameraIndex].ExposureAuto.SetValue(ExposureAuto_Off);
        cameras[cameraIndex].BalanceRatioSelector.SetValue(BalanceRatioSelector_Red);
        cameras[cameraIndex].BalanceWhiteAuto.SetValue(BalanceWhiteAuto_Off);
        cameras[cameraIndex].BalanceRatioSelector.SetValue(BalanceRatioSelector_Green);
        cameras[cameraIndex].BalanceWhiteAuto.SetValue(BalanceWhiteAuto_Off);
        cameras[cameraIndex].BalanceRatioSelector.SetValue(BalanceRatioSelector_Blue);
        cameras[cameraIndex].BalanceWhiteAuto.SetValue(BalanceWhiteAuto_Off);
    }
}

void ImageGrabber::ConfigureCamera(CBaslerUniversalInstantCamera &camera, const ArgumentsParser &parser, size_t index) {
    camera.RegisterConfiguration(new CSoftwareTriggerConfiguration, RegistrationMode_ReplaceAll, Cleanup_Delete);
    camera.RegisterConfiguration(new ConfigurationEventPrinter, RegistrationMode_Append, Cleanup_Delete);

    imageHandlers.push_back(new ImageEventHandler());
    camera.RegisterImageEventHandler(imageHandlers[index], RegistrationMode_Append, Cleanup_Delete);

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

    SetImageChunks(camera);
}

void ImageGrabber::ResetCamera(CBaslerUniversalInstantCamera &camera, size_t index) {
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

    cout << GetDateTime() << "Waiting for camera " << info.GetSerialNumber() << " to be reconnected..." << endl;

    CTlFactory &tlFactory = CTlFactory::GetInstance();
    while (true) {
        // Try to find the camera we are looking for.
        DeviceInfoList_t devices;

        // The camera has been found. Create and attach it to the Instant Camera object.
        if (tlFactory.EnumerateDevices(devices, filter) > 0) {
            camera.Attach(tlFactory.CreateDevice(devices[index]));
            break;
        }

        this_thread::sleep_for(chrono::milliseconds(250));
    }

    // Register an image event handler that accesses the chunk data.
    camera.RegisterConfiguration(
            new CSoftwareTriggerConfiguration, RegistrationMode_ReplaceAll, Cleanup_Delete);

    camera.RegisterConfiguration(new ConfigurationEventPrinter, RegistrationMode_Append, Cleanup_Delete);

    camera.Open();

    imageHandlers[index]->SetTimeOffset(camera);

    SetImageChunks(camera);
}

/**
 * Register an image event handler that accesses the chunk data.
 * @param camera
 */
void ImageGrabber::SetImageChunks(CBaslerUniversalInstantCamera &camera) {
    // Enable chunks in general.
    if (!camera.ChunkModeActive.TrySetValue(true))
        throw RUNTIME_EXCEPTION("The camera doesn't support chunk features"); // NOLINT(hicpp-exception-baseclass)

    // Enable time stamp chunks.
    camera.ChunkSelector.SetValue(Basler_UniversalCameraParams::ChunkSelector_Timestamp);
    camera.ChunkEnable.SetValue(true);

    camera.ChunkSelector.SetValue(Basler_UniversalCameraParams::ChunkSelector_ExposureTime);
    camera.ChunkEnable.SetValue(true);

    camera.ChunkSelector.SetValue(Basler_UniversalCameraParams::ChunkSelector_Gain);
    camera.ChunkEnable.SetValue(true);
}

void ImageGrabber::Grab(const ArgumentsParser &parser) {
    const chrono::system_clock::duration waitInterval = chrono::microseconds((int) (1000000.0 / parser.GetFrameRate()));

    while (true) {
        try {
            cameras.StartGrabbing(GrabStrategy_OneByOne, GrabLoop_ProvidedByInstantCamera);

            chrono::system_clock::time_point waitTime = chrono::system_clock::now() + waitInterval;

            while (cameras.IsGrabbing()) {
                for (size_t cameraIndex = 0; cameraIndex < cameraCount; cameraIndex++) {
                    cameras[cameraIndex].WaitForFrameTriggerReady(5000, TimeoutHandling_ThrowException);
                }
                for (size_t cameraIndex = 0; cameraIndex < cameraCount; cameraIndex++) {
                    cameras[cameraIndex].ExecuteSoftwareTrigger();
                }
                this_thread::sleep_until(waitTime);
                waitTime += waitInterval;
            }
        }
        catch (const GenericException &e) {
            cameras.StopGrabbing();

            // Known issue: Wait until the system safely detects a possible removal.
            this_thread::sleep_for(chrono::milliseconds(1000));

            // An exception occurred. Is it because the camera device has been physically removed?
            if (cameras.IsCameraDeviceRemoved()) {
                for (size_t cameraIndex = 0; cameraIndex < cameraCount; cameraIndex++) {
                    ResetCamera(cameras[cameraIndex], cameraIndex);
                }
            } else {
                throw;
            }
        }
    }
}
