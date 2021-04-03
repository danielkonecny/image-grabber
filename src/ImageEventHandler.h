/**
 * Image Grabber
 * Application for grabbing images from Basler cameras using Pylon API.
 * @file            ImageEventHandler.h
 * @version         1.0
 * @author          Daniel Konecny (xkonec75)
 * @organisation    Brno University of Technology - Faculty of Information Technologies
 * @date            02. 04. 2021
 */ 

#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <pylon/BaslerUniversalInstantCamera.h>
#include <pylon/BaslerUniversalInstantCameraArray.h>

using namespace std;
using namespace Pylon;

class ImageEventHandler : public CBaslerUniversalImageEventHandler {
private:
    ofstream timestampFile;
    string dateString;
    unsigned long long int timeOffset;
    CPylonImage imgPylon;
    CImageFormatConverter formatConverter;
    bool verbose;

public:
    ImageEventHandler (bool inputVerbose);

    ~ImageEventHandler ();

    string NanosecondsToDatetime (unsigned long long int originalTime);

    virtual void OnImageGrabbed (CBaslerUniversalInstantCamera& camera,
                                const CBaslerUniversalGrabResultPtr& ptrGrabResult);
};
