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
#include "ConfigurationEventPrinter.h"

using namespace std;
using namespace Pylon;
using namespace GenApi;
using namespace Basler_UniversalCameraParams;
using namespace cv;

ImageEventHandler::~ImageEventHandler() {
    logFile.close();
    if (verbose) {
        cout << GetDateTime() << "Log file closed." << endl;
    }

    if (!image) {
        vidOutput.release();
        if (verbose) {
            cout << GetDateTime() << "Video " << fileNameString << " released." << endl;
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

    exposureTime = parser.GetExposureTime();
    gain = parser.GetGain();
    balanceWhiteRed = parser.GetBalanceWhiteRed();
    balanceWhiteGreen = parser.GetBalanceWhiteGreen();
    balanceWhiteBlue = parser.GetBalanceWhiteBlue();

    if (exposureTime == -1) {
        /*
        // Set the Exposure Auto auto function to its minimum lower limit and its maximum upper limit
        auto minLowerLimit = camera.AutoExposureTimeLowerLimit.GetMin();
        auto maxUpperLimit = camera.AutoExposureTimeUpperLimit.GetMax();
        camera.AutoExposureTimeLowerLimit.SetValue(minLowerLimit);
        camera.AutoExposureTimeUpperLimit.SetValue(maxUpperLimit);
        // Enable Exposure Auto by setting the operating mode to Continuous
        camera.ExposureAuto.SetValue(ExposureAuto_Continuous);
         */
        exposureTime = camera.ExposureTime.GetValue();
    } else {
        camera.ExposureMode.SetValue(ExposureMode_Timed);
        camera.ExposureAuto.SetValue(ExposureAuto_Off);
        camera.ExposureTime.SetValue(exposureTime);
    }

    if (gain == -1) {
        /*
        // Set the the Gain Auto auto function to its minimum lower limit and its maximum upper limit
        auto minLowerLimit = camera.AutoGainLowerLimit.GetMin();
        auto maxUpperLimit = camera.AutoGainUpperLimit.GetMax();
        camera.AutoGainLowerLimit.SetValue(minLowerLimit);
        camera.AutoGainUpperLimit.SetValue(maxUpperLimit);
        // Enable Gain Auto by setting the operating mode to Continuous
        camera.GainAuto.SetValue(GainAuto_Continuous);
         */
        gain = camera.Gain.GetValue();
    } else {
        camera.GainAuto.SetValue(GainAuto_Off);
        camera.Gain.SetValue(gain);
    }

    /*
    // Enable Balance White Auto by setting the operating mode to Continuous.
    camera.BalanceWhiteAuto.SetValue(BalanceWhiteAuto_Continuous);
     */

    camera.BalanceRatioSelector.SetValue(BalanceRatioSelector_Red);
    if (balanceWhiteRed == -1) {
        balanceWhiteRed = camera.BalanceRatio.GetValue();
    } else {
        camera.BalanceRatioAbs.SetValue(balanceWhiteRed);
    }

    camera.BalanceRatioSelector.SetValue(BalanceRatioSelector_Green);
    if (balanceWhiteGreen == -1) {
        balanceWhiteGreen = camera.BalanceRatio.GetValue();
    } else {
        camera.BalanceRatioAbs.SetValue(balanceWhiteGreen);
    }

    camera.BalanceRatioSelector.SetValue(BalanceRatioSelector_Blue);
    if(balanceWhiteBlue == -1) {
        balanceWhiteBlue = camera.BalanceRatio.GetValue();
    } else {
        camera.BalanceRatioAbs.SetValue(balanceWhiteBlue);
    }

    CIntegerParameter camWidth(nodeMap, "Width");
    CIntegerParameter camHeight(nodeMap, "Height");

    width = (int) camWidth.GetMax();
    height = (int) camHeight.GetMax();

    cameraSerialNum = static_cast<const char *>(camera.GetDeviceInfo().GetSerialNumber());

    cout << GetDateTime() << "Camera " << camera.GetDeviceInfo().GetSerialNumber() << " details." << endl;
    cout << GetDateTime() << "- Offset X: " << camera.OffsetX.GetValue() << endl;
    cout << GetDateTime() << "- Offset Y: " << camera.OffsetY.GetValue() << endl;
    cout << GetDateTime() << "- Width: " << camera.Width.GetValue() << endl;
    cout << GetDateTime() << "- Height: " << camera.Height.GetValue() << endl;
    cout << GetDateTime() << "- ROI Offset X: " << camera.AutoFunctionROIOffsetX.GetValue() << endl;
    cout << GetDateTime() << "- ROI Offset Y: " << camera.AutoFunctionROIOffsetY.GetValue() << endl;
    cout << GetDateTime() << "- ROI Width: " << camera.AutoFunctionROIWidth.GetValue() << endl;
    cout << GetDateTime() << "- ROI Height: " << camera.AutoFunctionROIHeight.GetValue() << endl;
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
    cout << GetDateTime() << "Camera " << cameraSerialNum << " settings:" << endl;
    if (image) {
        cout << GetDateTime() << "- Grabbing images in IMAGE MODE." << endl;
    } else {
        cout << GetDateTime() << "- Grabbing images in VIDEO MODE." << endl;
    }
    cout << GetDateTime() << "- Resolution is " << width << "x" << height << "." << endl;
    cout << GetDateTime() << "- Offset set to " << timeOffset << " ns." << endl;
    cout << GetDateTime() << "- Gain is " << gain << "." << endl;
    cout << GetDateTime() << "- Exposure time is " << exposureTime << "." << endl;
    cout << GetDateTime() << "- White balance (red) is " << balanceWhiteRed << "." << endl;
    cout << GetDateTime() << "- White balance (green) is " << balanceWhiteGreen << "." << endl;
    cout << GetDateTime() << "- White balance (blue) is " << balanceWhiteBlue << "." << endl;
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
                << balanceWhiteRed << "," << balanceWhiteGreen << "," << balanceWhiteBlue << endl;

        imageIndex++;

        if (verbose) {
            cout << GetDateTime() << "Camera " << cameraSerialNum << " grabbed image at " <<
                 datetimeString << " (" << to_string(timestamp) << " ms)." << endl;
        }
    } else {
        cerr << GetDateTime() << "Error: " << hex << ptrGrabResult->GetErrorCode()
             << dec << " " << ptrGrabResult->GetErrorDescription() << endl;
    }
}
