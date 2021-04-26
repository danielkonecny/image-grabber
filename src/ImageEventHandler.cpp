/**
 * Image Grabber
 * Application for grabbing images from Basler cameras using Pylon API.
 * @file            ImageEventHandler.cpp
 * @version         1.0
 * @author          Daniel Konecny (xkonec75)
 * @organisation    Brno University of Technology - Faculty of Information Technologies
 * @date            02. 04. 2021
 */

#include <iostream>
#include <chrono>
#include <ctime>
#include <pylon/PylonIncludes.h>
#include <pylon/BaslerUniversalInstantCamera.h>
#include <opencv2/opencv.hpp>

#include "ImageEventHandler.h"

using namespace std;
using namespace Pylon;
using namespace GenApi;
using namespace cv;

ImageEventHandler::~ImageEventHandler() {
    timestampFile.close();
    if (!image) {
        vidOutput.release();
    }
}

void ImageEventHandler::Configure(CBaslerUniversalInstantCamera &camera, ArgumentsParser parser) {
    verbose = parser.IsVerbose();
    image = parser.IsImage();

    // Open file for logging grabbed images.
    timestampFile.open("out/timestamps.csv");
    timestampFile << "\"mode\",\"camera\",\"file_path\",\"timestamp_in_ms\",\"iso_datetime\"\n";

    // Get string representing current date.
    time_t date = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    ostringstream dateStream;
    dateStream << put_time(std::localtime(&date), "%FT");
    dateString = dateStream.str();

    formatConverter.OutputPixelFormat = PixelType_BGR8packed;

    INodeMap &nodeMap = camera.GetNodeMap();

    // Set time offset between camera and system time.
    unsigned long long int nowTime = std::chrono::duration_cast<std::chrono::nanoseconds>
            (std::chrono::system_clock::now().time_since_epoch()).count();
    CCommandParameter(nodeMap, "TimestampLatch").Execute();
    int64_t timestamp = CIntegerParameter(nodeMap, "TimestampLatchValue").GetValue();
    timeOffset = nowTime - timestamp;
    timeVidStart = nowTime / 1000000;

    if (verbose) {
        cout << "Offset set to " << timeOffset << " ns." << endl;
        if (image) {
            cout << "Grabbing images in IMAGE MODE." << endl;
        } else {
            cout << "Grabbing images in VIDEO MODE" << endl;
        }
    }

    // Open video output.
    if (!image) {
        ostringstream vidNameStream;
        vidNameStream << "out/vid/cam" << camera.GetDeviceInfo().GetSerialNumber()
                      << "vid" << std::to_string(timeVidStart) << ".avi";
        string vidNameString = vidNameStream.str();
        CIntegerParameter width(nodeMap, "Width");
        CIntegerParameter height(nodeMap, "Height");
        vidOutput.open(vidNameString, VideoWriter::fourcc('M', 'J', 'P', 'G'), 1,
                       Size((int) width.GetMax(), (int) height.GetMax()));

        if (verbose) {
            cout << "Video Resolution set to " << width.GetMax() << "x" << height.GetMax() << "." << endl;
        }
    }
}

string ImageEventHandler::NanosecondsToDatetime(unsigned long long int originalTime) {
    unsigned long long int milliseconds = originalTime % 1000;
    unsigned long long int rest = originalTime / 1000;
    unsigned long long int seconds = rest % 60;
    rest /= 60;
    unsigned long long int minutes = rest % 60;
    rest /= 60;
    unsigned long long int hours = rest % 24;

    ostringstream datetimeStream;
    datetimeStream << dateString << std::setfill('0') << std::setw(2) << hours
                   << ":" << std::setw(2) << minutes << ":" << std::setw(2) << seconds
                   << "." << std::setw(3) << milliseconds;
    return datetimeStream.str();
}

void ImageEventHandler::OnImageGrabbed(
        CBaslerUniversalInstantCamera &camera,
        const CBaslerUniversalGrabResultPtr &ptrGrabResult) {

    if (ptrGrabResult->GrabSucceeded()) {
        if (!(ptrGrabResult->ChunkTimestamp.IsReadable()))
            return;

        // Convert and save image.
        formatConverter.Convert(imgPylon, ptrGrabResult);
        Mat imgMat = Mat((int) ptrGrabResult->GetHeight(), (int) ptrGrabResult->GetWidth(),
                         CV_8UC3, (uint8_t *) imgPylon.GetBuffer());

        // Timestamp computation.
        unsigned long long int timestamp = ptrGrabResult->ChunkTimestamp.GetValue();
        timestamp = (timestamp + timeOffset) / 1000000;
        string datetimeString = NanosecondsToDatetime(timestamp);

        if (image) {
            ostringstream imgNameStream;
            imgNameStream << "out/img/cam" << camera.GetDeviceInfo().GetSerialNumber()
                          << "img" << std::to_string(timestamp) << ".jpg";
            string imgNameString = imgNameStream.str();

            imwrite(imgNameString, imgMat);

            timestampFile << "img," << camera.GetDeviceInfo().GetSerialNumber() << ",\"" << imgNameString << "\","
                          << timestamp << ",\"" << datetimeString << "\"\n";
        } else {
            vidOutput.write(imgMat);

            ostringstream vidNameStream;
            vidNameStream << "out/vid/cam" << camera.GetDeviceInfo().GetSerialNumber()
                          << "vid" << std::to_string(timeVidStart) << ".avi";
            string vidNameString = vidNameStream.str();

            timestampFile << "vid," << camera.GetDeviceInfo().GetSerialNumber() << ",\"" << vidNameString << "\","
                          << timestamp << ",\"" << datetimeString << "\"\n";
        }
        timestampFile.flush();

        if (verbose) {
            cout << "Camera " << camera.GetDeviceInfo().GetSerialNumber() << " grabbed image at " <<
                 datetimeString << " (" << std::to_string(timestamp) << " ms)." << endl;
        }
    } else {
        cerr << "Error: " << std::hex << ptrGrabResult->GetErrorCode()
             << std::dec << " " << ptrGrabResult->GetErrorDescription() << endl;
    }
}
