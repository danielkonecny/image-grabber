/**
 * Image Grabber
 * Application for grabbing images from Basler cameras using Pylon API.
 * @file            ImageEventHandler.h
 * @version         2.0
 * @author          Daniel Konecny (xkonec75)
 * @organisation    Brno University of Technology - Faculty of Information Technologies
 * @date            29. 04. 2021
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
    ofstream logFile;
    unsigned long long int timeOffset;
    unsigned long long int timeGrabbingStarts = 0;
    string dateString;
    string fileDateString;
    string mode = "img";
    string fileNameString;
    unsigned long long int imageIndex = 0;

    CPylonImage imgPylon;
    CImageFormatConverter formatConverter;
    VideoWriter vidOutput;

    bool verbose;

    bool image;
    string outDir;
    int imgQuality;

    string cameraSerialNum;
    int width;
    int height;
    double gain;
    double exposureTime;
    double balanceWhiteRed;
    double balanceWhiteGreen;
    double balanceWhiteBlue;

    void CreateOutDir();

public:
    ~ImageEventHandler() override;

    void SetProgramParams(ArgumentsParser parser);

    void SetCameraParams(CBaslerUniversalInstantCamera &camera, const ArgumentsParser &parser);

    void SetTimeOffset(CBaslerUniversalInstantCamera &camera);

    void OpenLogFile();

    void OpenVidOutput(double frameRate);

    void PrintCameraState();

    string NanosecondsToDatetime(unsigned long long int originalTime);

    string NanosecondsToFileDatetime(unsigned long long int originalTime);

    void OnImageGrabbed(CBaslerUniversalInstantCamera &camera,
                        const CBaslerUniversalGrabResultPtr &ptrGrabResult) override;
};
