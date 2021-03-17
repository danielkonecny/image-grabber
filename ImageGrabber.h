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

