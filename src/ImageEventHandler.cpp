/**
 * Image Grabber
 * Application for grabbing images from Basler cameras using Pylon API.
 * @file            ImageEventHandler.cpp
 * @author          Daniel Konecny (xkonec75)
 * @organisation    Brno University of Technology - Faculty of Information Technologies
 * @date            30. 04. 2021
 */

#include <iostream>
#include <chrono>
#include <ctime>
#include <filesystem>
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

void ImageEventHandler::CreateOutDir() {
    filesystem::create_directory(outDir);
    filesystem::create_directory(outDir + "/log");
    filesystem::create_directory(outDir + "/img");
    filesystem::create_directory(outDir + "/vid");
}

void ImageEventHandler::SetProgramParams(ArgumentsParser parser) {
    verbose = parser.IsVerbose();
    image = parser.IsImage();
    imgQuality = parser.GetImgQuality();

    // Check if output directory exists and create if not.
    outDir = parser.GetOutDir();
    CreateOutDir();

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
    exposureTime = parser.GetExposureTime();
    gain = parser.GetGain();
    balanceWhiteRed = parser.GetBalanceWhiteRed();
    balanceWhiteGreen = parser.GetBalanceWhiteGreen();
    balanceWhiteBlue = parser.GetBalanceWhiteBlue();

    camera.OffsetX.TrySetToMinimum();
    camera.OffsetY.TrySetToMinimum();
    camera.Width.SetToMaximum();
    camera.Height.SetToMaximum();

    camera.AutoFunctionROISelector.SetValue(AutoFunctionROISelector_ROI1);
    camera.AutoFunctionROIUseBrightness.TrySetValue(true);   // ROI 1 is used for brightness control
    camera.AutoFunctionROISelector.SetValue(AutoFunctionROISelector_ROI2);
    camera.AutoFunctionROIUseBrightness.TrySetValue(false);   // ROI 2 is not used for brightness control

    // Set the ROI (in this example the complete sensor is used)
    camera.AutoFunctionROISelector.SetValue(AutoFunctionROISelector_ROI1);  // configure ROI 1
    camera.AutoFunctionROIOffsetX.SetToMinimum();
    camera.AutoFunctionROIOffsetY.SetToMinimum();
    camera.AutoFunctionROIWidth.SetToMaximum();
    camera.AutoFunctionROIHeight.SetToMaximum();

    // Specify the target value
    camera.AutoTargetBrightness.SetValue(0.6);
    // Select auto function ROI 1
    camera.AutoFunctionROISelector.SetValue(AutoFunctionROISelector_ROI1);
    // Enable the 'Brightness' auto function (Gain Auto + Exposure Auto)
    // for the auto function ROI selected
    camera.AutoFunctionROIUseBrightness.SetValue(true);

    camera.ExposureMode.SetValue(ExposureMode_Timed);

    if (exposureTime == -1) {
        const double minLowerLimit = camera.AutoExposureTimeLowerLimit.GetMin();
        // Maximal exposure time computed from given frame rate with additional time for image handling.
        const double handlingTime = 5000.0; // 5 ms
        const double maxUpperLimit = (1000000.0 / parser.GetFrameRate()) - handlingTime;
        camera.AutoExposureTimeLowerLimit.SetValue(minLowerLimit);
        camera.AutoExposureTimeUpperLimit.SetValue(maxUpperLimit);

        // Reset the exposure time value because it stays the same if it is out of the specified range.
        camera.ExposureAuto.SetValue(ExposureAuto_Off);
        camera.ExposureTime.SetToMinimum();

        // Enable Exposure Auto by setting the operating mode to Continuous
        camera.ExposureAuto.SetValue(ExposureAuto_Continuous);
        exposureTime = camera.ExposureTime.GetValue();
    } else {
        camera.ExposureAuto.SetValue(ExposureAuto_Off);
        camera.ExposureTime.SetValue(exposureTime);
    }

    if (gain == -1) {
        // Set the the Gain Auto auto function to its minimum lower limit and its maximum upper limit
        auto minLowerLimit = camera.AutoGainLowerLimit.GetMin();
        auto maxUpperLimit = camera.AutoGainUpperLimit.GetMax();
        camera.AutoGainLowerLimit.SetValue(minLowerLimit);
        camera.AutoGainUpperLimit.SetValue(maxUpperLimit);

        // Enable Gain Auto by setting the operating mode to Continuous
        camera.GainAuto.SetValue(GainAuto_Continuous);
        gain = camera.Gain.GetValue();
    } else {
        camera.GainAuto.SetValue(GainAuto_Off);
        camera.Gain.SetValue(gain);
    }

    camera.BalanceRatioSelector.SetValue(BalanceRatioSelector_Red);
    if (balanceWhiteRed == -1) {
        // Select auto function ROI 2
        camera.AutoFunctionROISelector.SetValue(AutoFunctionROISelector_ROI2);
        // Enable the Balance White Auto auto function
        // for the auto function ROI selected
        camera.AutoFunctionROIUseWhiteBalance.SetValue(true);
        // Enable Balance White Auto by setting the operating mode to Continuous.
        camera.BalanceWhiteAuto.SetValue(BalanceWhiteAuto_Continuous);
        balanceWhiteRed = camera.BalanceRatio.GetValue();
    } else {
        camera.BalanceWhiteAuto.SetValue(BalanceWhiteAuto_Off);
        camera.BlackLevel.SetValue(0);
        camera.BalanceRatio.SetValue(balanceWhiteRed);
    }

    camera.BalanceRatioSelector.SetValue(BalanceRatioSelector_Green);
    if (balanceWhiteGreen == -1) {
        // Select auto function ROI 2
        camera.AutoFunctionROISelector.SetValue(AutoFunctionROISelector_ROI2);
        // Enable the Balance White Auto auto function
        // for the auto function ROI selected
        camera.AutoFunctionROIUseWhiteBalance.SetValue(true);
        // Enable Balance White Auto by setting the operating mode to Continuous.
        camera.BalanceWhiteAuto.SetValue(BalanceWhiteAuto_Continuous);
        balanceWhiteGreen = camera.BalanceRatio.GetValue();
    } else {
        camera.BalanceWhiteAuto.SetValue(BalanceWhiteAuto_Off);
        camera.BlackLevel.SetValue(0);
        camera.BalanceRatio.SetValue(balanceWhiteGreen);
    }

    camera.BalanceRatioSelector.SetValue(BalanceRatioSelector_Blue);
    if (balanceWhiteBlue == -1) {
        // Select auto function ROI 2
        camera.AutoFunctionROISelector.SetValue(AutoFunctionROISelector_ROI2);
        // Enable the Balance White Auto auto function
        // for the auto function ROI selected
        camera.AutoFunctionROIUseWhiteBalance.SetValue(true);
        // Enable Balance White Auto by setting the operating mode to Continuous.
        camera.BalanceWhiteAuto.SetValue(BalanceWhiteAuto_Continuous);
        balanceWhiteBlue = camera.BalanceRatio.GetValue();
    } else {
        camera.BalanceWhiteAuto.SetValue(BalanceWhiteAuto_Off);
        camera.BlackLevel.SetValue(0);
        camera.BalanceRatio.SetValue(balanceWhiteBlue);
    }

    width = (int) (camera.Width.GetValue() - camera.OffsetX.GetValue());
    height = (int) (camera.Height.GetValue() - camera.OffsetY.GetValue());

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
    string fileDatetimeString = NanosecondsToFileDatetime(timeGrabbingStarts);
    string logNameString = outDir + "/log/cam" + cameraSerialNum + "_log" + fileDatetimeString + ".csv";
    logFile.open(logNameString, fstream::out);
    logFile << R"("index","mode","camera","file_path","timestamp_in_ms","iso_datetime",)"
            << R"("exposure_time","gain","white_balance_r","white_balance_g","white_balance_b")" << endl;
}

void ImageEventHandler::OpenVidOutput(double frameRate) {
    mode = "vid";
    string fileDatetimeString = NanosecondsToFileDatetime(timeGrabbingStarts);
    fileNameString = outDir + "/" + mode + "/cam" + cameraSerialNum + "_" + mode + fileDatetimeString + ".avi";
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
    cout << GetDateTime() << "- Initial exposure time is " << exposureTime << "." << endl;
    cout << GetDateTime() << "- Initial gain is " << gain << "." << endl;
    cout << GetDateTime() << "- Initial white balance (red) is " << balanceWhiteRed << "." << endl;
    cout << GetDateTime() << "- Initial white balance (green) is " << balanceWhiteGreen << "." << endl;
    cout << GetDateTime() << "- Initial white balance (blue) is " << balanceWhiteBlue << "." << endl;
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

        camera.BalanceRatioSelector.SetValue(BalanceRatioSelector_Red);
        balanceWhiteRed = camera.BalanceRatio.GetValue();
        camera.BalanceRatioSelector.SetValue(BalanceRatioSelector_Green);
        balanceWhiteGreen = camera.BalanceRatio.GetValue();
        camera.BalanceRatioSelector.SetValue(BalanceRatioSelector_Blue);
        balanceWhiteBlue = camera.BalanceRatio.GetValue();

        if (image) {
            string fileDatetimeString = NanosecondsToFileDatetime(timestamp);
            fileNameString = outDir + "/" + mode + "/cam" + cameraSerialNum + "_" + mode + fileDatetimeString + ".jpg";
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
