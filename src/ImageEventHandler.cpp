/**
 * Image Grabber
 * Application for grabbing images from Basler cameras using Pylon API.
 * @file            ImageEventHandler.cpp
 * @author          Daniel Konecny (xkonec75)
 * @organisation    Brno University of Technology - Faculty of Information Technologies
 * @date            28. 04. 2021
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
using namespace Basler_UniversalCameraParams;
using namespace cv;

ImageEventHandler::~ImageEventHandler() {
    logFile.close();
    if (verbose) {
        cout << "Log file closed." << endl;
    }

    if (!image) {
        vidOutput.release();
        if (verbose) {
            cout << "Video " << fileNameString << " released." << endl;
        }
    }
}

void ImageEventHandler::SetProgramParams(ArgumentsParser parser) {
    verbose = parser.IsVerbose();
    image = parser.IsImage();
    outDir = parser.GetOutDir();
    imgQuality = parser.GetImgQuality();

    // Get string representing current date.
    time_t date = chrono::system_clock::to_time_t(chrono::system_clock::now());

    ostringstream dateStream;
    dateStream << put_time(localtime(&date), "%FT");
    dateString = dateStream.str();

    ostringstream fileDateStream;
    fileDateStream << put_time(localtime(&date), "%G") << "_" << put_time(localtime(&date), "%m") << "_"
                   << put_time(localtime(&date), "%d");
    fileDateString = fileDateStream.str();

    formatConverter.OutputPixelFormat = PixelType_BGR8packed;
}

void ImageEventHandler::SetCameraParams(CBaslerUniversalInstantCamera &camera, const ArgumentsParser &parser) {
    INodeMap &nodeMap = camera.GetNodeMap();

    exposureTime = parser.getExposureTime();
    gain = parser.getGain();

    if (exposureTime != -1) {
        camera.ExposureMode.SetValue(ExposureMode_Timed);
        camera.ExposureAuto.SetValue(ExposureAuto_Off);
        camera.ExposureTime.SetValue(exposureTime);
    } else {
        exposureTime = camera.ExposureTime.GetValue();
    }

    if (gain != -1) {
        camera.GainAuto.SetValue(GainAuto_Off);
        camera.Gain.SetValue(gain);
    } else {
        gain = camera.Gain.GetValue();
    }

    camera.BalanceRatioSelector.SetValue(BalanceRatioSelector_Red);
    whiteBalanceR = camera.BalanceRatio.GetValue();
    camera.BalanceRatioSelector.SetValue(BalanceRatioSelector_Green);
    whiteBalanceG = camera.BalanceRatio.GetValue();
    camera.BalanceRatioSelector.SetValue(BalanceRatioSelector_Blue);
    whiteBalanceB = camera.BalanceRatio.GetValue();

    CIntegerParameter camWidth(nodeMap, "Width");
    CIntegerParameter camHeight(nodeMap, "Height");

    width = (int) camWidth.GetMax();
    height = (int) camHeight.GetMax();

    cameraSerialNum = static_cast<const char *>(camera.GetDeviceInfo().GetSerialNumber());
}

/**
 * Set time offset between camera and system time.
 * @param camera
 */
void ImageEventHandler::SetTimeOffset(CBaslerUniversalInstantCamera &camera) {
    INodeMap &nodeMap = camera.GetNodeMap();
    unsigned long long int nowTime = chrono::duration_cast<chrono::nanoseconds>
            (chrono::system_clock::now().time_since_epoch()).count();
    CCommandParameter(nodeMap, "TimestampLatch").Execute();
    int64_t timestamp = CIntegerParameter(nodeMap, "TimestampLatchValue").GetValue();
    timeOffset = nowTime - timestamp;
    if (timeGrabbingStarts == 0) {
        timeGrabbingStarts = nowTime / 1000000;
    }
}

/**
 * Open file for logging grabbed images.
 */
void ImageEventHandler::OpenLogFile() {
    string logNameString = outDir + "/log/cam" + cameraSerialNum + "log" + to_string(timeGrabbingStarts) + ".csv";

    logFile.open(logNameString, fstream::out);

    logFile << R"("index","mode","camera","file_path","timestamp_in_ms","iso_datetime",)"
            << R"("exposure_time","gain","white_balance_r","white_balance_g","white_balance_b")" << endl;
}

void ImageEventHandler::OpenVidOutput(unsigned int frameRate) {
    mode = "vid";
    string fileDatetimeString = NanosecondsToFileDatetime(timeGrabbingStarts);
    fileNameString = outDir + "/" + mode + "/cam" + cameraSerialNum + mode + fileDatetimeString + ".avi";
    vidOutput.open(fileNameString, VideoWriter::fourcc('M', 'J', 'P', 'G'), frameRate, Size(width, height));
}

void ImageEventHandler::PrintCameraState() {
    cout << endl;
    cout << "Camera " << cameraSerialNum << " settings:" << endl;
    if (image) {
        cout << "- Grabbing images in IMAGE MODE." << endl;
    } else {
        cout << "- Grabbing images in VIDEO MODE." << endl;
    }
    cout << "- Resolution is " << width << "x" << height << "." << endl;
    cout << "- Offset set to " << timeOffset << " ns." << endl;
    cout << "- Gain is " << gain << "." << endl;
    cout << "- Exposure time is " << exposureTime << "." << endl;
    cout << "- White balance (red) is " << whiteBalanceR << "." << endl;
    cout << "- White balance (green) is " << whiteBalanceG << "." << endl;
    cout << "- White balance (blue) is " << whiteBalanceB << "." << endl;
    cout << endl;
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
    datetimeStream << dateString << setfill('0') << setw(2) << hours << ":" << setw(2) << minutes << ":" << setw(2)
                   << seconds << "." << setw(3) << milliseconds;
    return datetimeStream.str();
}

string ImageEventHandler::NanosecondsToFileDatetime(unsigned long long int originalTime) {
    unsigned long long int milliseconds = originalTime % 1000;
    unsigned long long int rest = originalTime / 1000;
    unsigned long long int seconds = rest % 60;
    rest /= 60;
    unsigned long long int minutes = rest % 60;
    rest /= 60;
    unsigned long long int hours = rest % 24;

    ostringstream fileDatetimeStream;
    fileDatetimeStream << fileDateString << "_" << setfill('0') << setw(2) << hours << "_" << setw(2) << minutes
                   << "_" << setw(2) << seconds << "_" << setw(3) << milliseconds;
    return fileDatetimeStream.str();
}

void ImageEventHandler::OnImageGrabbed(CBaslerUniversalInstantCamera &camera,
                                       const CBaslerUniversalGrabResultPtr &ptrGrabResult) {
    if (ptrGrabResult->GrabSucceeded()) {
        if (!(ptrGrabResult->ChunkTimestamp.IsReadable()))
            return;

        // Convert and save image.
        formatConverter.Convert(imgPylon, ptrGrabResult);
        Mat imgMat = Mat(height, width, CV_8UC3, (uint8_t *) imgPylon.GetBuffer());

        unsigned long long int chunkTimestamp = ptrGrabResult->ChunkTimestamp.GetValue();
        double chunkExposureTime = ptrGrabResult->ChunkExposureTime.GetValue();
        double chunkGain = ptrGrabResult->ChunkGain.GetValue();

        unsigned long long int timestamp = (chunkTimestamp + timeOffset) / 1000000;
        string datetimeString = NanosecondsToDatetime(timestamp);

        if (image) {
            string fileDatetimeString = NanosecondsToFileDatetime(timestamp);
            fileNameString = outDir + "/" + mode + "/cam" + cameraSerialNum + mode + fileDatetimeString + ".jpg";
            imwrite(fileNameString, imgMat, vector<int>({IMWRITE_JPEG_QUALITY, imgQuality}));
        } else {
            vidOutput.write(imgMat);
        }

        logFile << imageIndex << ",\"" << mode << "\"," << cameraSerialNum << ",\"" << fileNameString << "\","
                << timestamp << ",\"" << datetimeString << "\"," << chunkExposureTime << "," << chunkGain << ","
                << whiteBalanceR << "," << whiteBalanceG << "," << whiteBalanceB << endl;

        imageIndex++;

        if (verbose) {
            cout << "Camera " << cameraSerialNum << " grabbed image at " <<
                 datetimeString << " (" << to_string(timestamp) << " ms)." << endl;
        }
    } else {
        cerr << "Error: " << hex << ptrGrabResult->GetErrorCode()
             << dec << " " << ptrGrabResult->GetErrorDescription() << endl;
    }
}
