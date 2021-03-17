#include <iostream>
#include <pylon/PylonIncludes.h>
#include <pylon/BaslerUniversalInstantCamera.h>
#include <pylon/BaslerUniversalInstantCameraArray.h>

#include "ImageGrabber.h"
#include "CSampleImageEventHandler.h"

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
	    cameras[cameraIndex].RegisterImageEventHandler(
	        new CSampleImageEventHandler, RegistrationMode_Append, Cleanup_Delete);
	    cameras[cameraIndex].RegisterConfiguration(
	        new CSoftwareTriggerConfiguration, RegistrationMode_ReplaceAll, Cleanup_Delete);

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

    cameras.StartGrabbing();

    for (size_t imageIndex = 0; imageIndex < maxImagesToTake && cameras.IsGrabbing(); imageIndex++) {
    	// One possible sequential implementation of capturing images from two cameras.
        if (cameraCount == 2) {
            cameras[0].WaitForFrameTriggerReady(1000, TimeoutHandling_ThrowException);
            cameras[1].WaitForFrameTriggerReady(1000, TimeoutHandling_ThrowException);
            cameras[0].ExecuteSoftwareTrigger();
            cameras[1].ExecuteSoftwareTrigger();
            cameras.RetrieveResult(5000, ptrGrabResult);
            cameras.RetrieveResult(5000, ptrGrabResult);
        }
        else if (cameraCount == 1) {
            cameras[0].WaitForFrameTriggerReady(1000, TimeoutHandling_ThrowException);
            cameras[0].ExecuteSoftwareTrigger();
            cameras.RetrieveResult(5000, ptrGrabResult);
        }
        else {
            cerr << "Camera count " << cameraCount << " is not implemented." << endl;
        }

        /*
        // Implementation of the same process just for any number of cameras, might be slower.
        for (size_t cameraIndex = 0; cameraIndex < cameraCount; cameraIndex++)
            cameras[cameraIndex].WaitForFrameTriggerReady(1000, TimeoutHandling_ThrowException);

        for (size_t cameraIndex = 0; cameraIndex < cameraCount; cameraIndex++)
            cameras[cameraIndex].ExecuteSoftwareTrigger();

        for (size_t cameraIndex = 0; cameraIndex < cameraCount; cameraIndex++)
            cameras.RetrieveResult(5000, ptrGrabResult);
        */
    }
}

