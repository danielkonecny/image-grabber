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
#include <opencv2/opencv.hpp>

#include "ArgumentsParser.h"

using namespace std;
using namespace Pylon;
using namespace cv;

class ImageEventHandler : public CBaslerUniversalImageEventHandler {
private:
    ofstream timestampFile;
    string dateString;
    unsigned long long int timeOffset;
    unsigned long long int timeGrabbingStarts;
    string cameraSerialNum;

    CPylonImage imgPylon;
    CImageFormatConverter formatConverter;

    VideoWriter vidOutput;

    bool verbose;
    bool image;
    string outDir;
    int imgQuality;

public:
    ~ImageEventHandler() override;

    void Configure(CBaslerUniversalInstantCamera &camera, ArgumentsParser parser);

    string NanosecondsToDatetime(unsigned long long int originalTime);

    void OnImageGrabbed(CBaslerUniversalInstantCamera &camera,
                        const CBaslerUniversalGrabResultPtr &ptrGrabResult) override;
};
