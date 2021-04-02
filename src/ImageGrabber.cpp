/**
 * Image Grabber
 * Application for grabbing images from Basler cameras using Pylon API.
 * @file            ImageGrabber.cpp
 * @version         1.0
 * @author          Daniel Konecny (xkonec75)
 * @organisation    Brno University of Technology - Faculty of Information Technologies
 * @date            02. 04. 2021
 */

#include <iostream>
#include <pylon/PylonIncludes.h>
#include <pylon/BaslerUniversalInstantCamera.h>
#include <pylon/BaslerUniversalInstantCameraArray.h>

#include "ImageGrabber.h"
#include "ImageEventHandler.h"
#include "ConfigurationEventPrinter.h"

using namespace std;
using namespace Pylon;
using namespace GenApi;


ImageGrabber::ImageGrabber () {
	CTlFactory& tlFactory = CTlFactory::GetInstance();

	// Get all attached devices and exit application if no device is found.
	DeviceInfoList_t devices;
	if (tlFactory.EnumerateDevices(devices) == 0) {
	    throw RUNTIME_EXCEPTION("No camera present.");
	}

	// Create an array of instant cameras for the found devices and avoid exceeding a maximum number of devices.
	const size_t maxCamerasToUse = 2;
	cameras.Initialize(min(devices.size(), maxCamerasToUse));
	cameraCount = cameras.GetSize();

	// Create and attach all Pylon Devices.
	for (size_t cameraIndex = 0; cameraIndex < cameraCount; cameraIndex++) {
	    cameras[cameraIndex].Attach(tlFactory.CreateDevice(devices[cameraIndex]));

	    cout << "Using device " << cameras[cameraIndex].GetDeviceInfo().GetModelName() << endl;

	    // Register an image event handler that accesses the chunk data.
	    cameras[cameraIndex].RegisterConfiguration(
	        new CSoftwareTriggerConfiguration, RegistrationMode_ReplaceAll, Cleanup_Delete);

        cameras[cameraIndex].RegisterConfiguration(
            new ConfigurationEventPrinter, RegistrationMode_Append, Cleanup_Delete);

	    cameras[cameraIndex].RegisterImageEventHandler(
	        new ImageEventHandler, RegistrationMode_Append, Cleanup_Delete);

	    // Open the camera.
	    cameras[cameraIndex].Open();

	    // Enable chunks in general.
	    if (!cameras[cameraIndex].ChunkModeActive.TrySetValue(true))
	        throw RUNTIME_EXCEPTION("The camera doesn't support chunk features");

	    // Enable time stamp chunks.
	    cameras[cameraIndex].ChunkSelector.SetValue(Basler_UniversalCameraParams::ChunkSelector_Timestamp);
	    cameras[cameraIndex].ChunkEnable.SetValue(true);
	}
}

void ImageGrabber::Grab () {
    size_t maxImagesToTake = 50;

    cameras.StartGrabbing(GrabStrategy_OneByOne, GrabLoop_ProvidedByInstantCamera);

    for (size_t imageIndex = 0; imageIndex < maxImagesToTake && cameras.IsGrabbing(); imageIndex++) {
        for (size_t cameraIndex = 0; cameraIndex < cameraCount; cameraIndex++) {
            if (cameras[cameraIndex].WaitForFrameTriggerReady(1000, TimeoutHandling_ThrowException)) {
                cameras[cameraIndex].ExecuteSoftwareTrigger();
            }
        }
        WaitObject::Sleep(10);

        /*
    	// One possible sequential implementation of capturing images from two cameras.
        if (cameraCount == 2) {
            if (cameras[0].WaitForFrameTriggerReady(1000, TimeoutHandling_ThrowException)) {
                cameras[0].ExecuteSoftwareTrigger();
            }
            if (cameras[1].WaitForFrameTriggerReady(1000, TimeoutHandling_ThrowException)) {
                cameras[1].ExecuteSoftwareTrigger();
            }
        }
        else if (cameraCount == 1) {
            if (cameras[0].WaitForFrameTriggerReady(1000, TimeoutHandling_ThrowException)) {
                cameras[0].ExecuteSoftwareTrigger();
            }
        }
        else {
            cerr << "Camera count " << cameraCount << " is not implemented." << endl;
        }
        */
    }
}

