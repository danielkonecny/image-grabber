/**
 * Image Grabber
 * Application for grabbing images from Basler cameras using Pylon API.
 * @file            ImageGrabber.h
 * @version         1.0
 * @author          Daniel Konecny (xkonec75)
 * @organisation    Brno University of Technology - Faculty of Information Technologies
 * @date            02. 04. 2021
 */ 

#include <iostream>
#include <pylon/PylonIncludes.h>
#include <pylon/BaslerUniversalInstantCamera.h>
#include <pylon/BaslerUniversalInstantCameraArray.h>

using namespace std;
using namespace Pylon;
using namespace GenApi;

class ImageGrabber {
private:
	size_t cameraCount;

public:
	CBaslerUniversalInstantCameraArray cameras;
	CBaslerUniversalGrabResultPtr ptrGrabResult;
	
    ImageGrabber ();
    
    void Grab ();
};

