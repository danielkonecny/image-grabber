/**
 * Image Grabber
 * Application for grabbing images from Basler cameras using Pylon API.
 * @file            ImageGrabber.cpp
 * @version         2.0
 * @author          Daniel Konecny (xkonec75)
 * @organisation    Brno University of Technology - Faculty of Information Technologies
 * @date            03. 04. 2021
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
        throw runtime_error("No camera present.");
    }

    // Create an array of instant cameras for the found devices and avoid exceeding a maximum number of devices.
    const size_t maxCamerasToUse = 2;
    cameras.Initialize(min(devices.size(), maxCamerasToUse));
    cameraCount = cameras.GetSize();

    // Create and attach all Pylon Devices.
    for (size_t cameraIndex = 0; cameraIndex < cameraCount; cameraIndex++) {
        cameras[cameraIndex].Attach(tlFactory.CreateDevice(devices[cameraIndex]));

        // Register an image event handler that accesses the chunk data.
        cameras[cameraIndex].RegisterConfiguration(
                new CSoftwareTriggerConfiguration, RegistrationMode_ReplaceAll, Cleanup_Delete);

        imageHandlers.push_back(new ImageEventHandler());
        cameras[cameraIndex].RegisterImageEventHandler(imageHandlers[cameraIndex], RegistrationMode_Append,
                                                       Cleanup_Delete);

        if (parser.IsVerbose()) {
            cout << "Using device " << cameras[cameraIndex].GetDeviceInfo().GetModelName() << endl;

            cameras[cameraIndex].RegisterConfiguration(
                    new ConfigurationEventPrinter, RegistrationMode_Append, Cleanup_Delete);
        }

        // Open the camera.
        cameras[cameraIndex].Open();

        imageHandlers[cameraIndex]->Configure(cameras[cameraIndex], parser);

        // Enable chunks in general.
        if (!cameras[cameraIndex].ChunkModeActive.TrySetValue(true))
            throw runtime_error("The camera doesn't support chunk features");

        // Enable time stamp chunks.
        cameras[cameraIndex].ChunkSelector.SetValue(Basler_UniversalCameraParams::ChunkSelector_Timestamp);
        cameras[cameraIndex].ChunkEnable.SetValue(true);
    }
}

void ImageGrabber::Grab(const ArgumentsParser &parser) {
    unsigned long long waitTime = parser.GetWaitTime();

    cameras.StartGrabbing(GrabStrategy_OneByOne, GrabLoop_ProvidedByInstantCamera);

    while (cameras.IsGrabbing()) {
        for (size_t cameraIndex = 0; cameraIndex < cameraCount; cameraIndex++) {
            cameras[cameraIndex].WaitForFrameTriggerReady(100, TimeoutHandling_ThrowException);
            cameras[cameraIndex].ExecuteSoftwareTrigger();
        }
        this_thread::sleep_for(chrono::milliseconds(waitTime));
    }
}

