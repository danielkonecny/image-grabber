/**
 * Image Grabber
 * Application for grabbing images from Basler cameras using Pylon API.
 * @file            ImageGrabber.h
 * @author          Daniel Konecny (xkonec75)
 * @organisation    Brno University of Technology - Faculty of Information Technologies
 * @date            30. 04. 2021
 */

#include <iostream>
#include <pylon/PylonIncludes.h>
#include <pylon/BaslerUniversalInstantCamera.h>
#include <pylon/BaslerUniversalInstantCameraArray.h>

#include "ImageEventHandler.h"

using namespace std;
using namespace Pylon;
using namespace GenApi;

class ImageGrabber {
private:
    size_t cameraCount;
    vector<ImageEventHandler *> imageHandlers;

public:
    CBaslerUniversalInstantCameraArray cameras;

    explicit ImageGrabber(const ArgumentsParser &parser);

    ~ImageGrabber();

    void ConfigureCamera(CBaslerUniversalInstantCamera &camera, const ArgumentsParser &parser, size_t index);

    void ResetCamera(CBaslerUniversalInstantCamera &camera, size_t index);

    static void SetImageChunks(CBaslerUniversalInstantCamera &camera);

    void Grab(const ArgumentsParser &parser);
};

