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
#include <fstream>
#include <chrono>
#include <ctime>
#include <pylon/PylonIncludes.h>
#include <pylon/BaslerUniversalInstantCamera.h>
#include <pylon/BaslerUniversalInstantCameraArray.h>
#include <opencv2/opencv.hpp>

#include "ImageEventHandler.h"

using namespace std;
using namespace Pylon;
using namespace cv;


ImageEventHandler::ImageEventHandler () {
    timestampFile.open("out/timestamps.csv");
    timestampFile << "\"image_file\",\"timestamp_in_ms\",\"iso_datetime\"\n";

    time_t date = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    ostringstream dateStream;
    dateStream << put_time(std::localtime(&date), "%FT");
    dateString = dateStream.str();

    formatConverter.OutputPixelFormat = PixelType_BGR8packed;
    
    timeOffset = 0;
}

ImageEventHandler::~ImageEventHandler () {
    timestampFile.close();
}

string ImageEventHandler::NanosecondsToDatetime (unsigned long long int originalTime) {
    unsigned long long int mimoseconds = originalTime % 1000;
    unsigned long long int rest = originalTime / 1000;
    unsigned long long int seconds = rest % 60;
    rest /= 60;
    unsigned long long int minutes = rest % 60;
    rest /= 60; 
    unsigned long long int hours = rest % 24;

    ostringstream datetimeStream;
    datetimeStream << dateString << std::setfill('0') << std::setw(2) << hours
                   << ":" << std::setw(2) << minutes << ":" << std::setw(2) << seconds
                   << "." << std::setw(3) << mimoseconds;
    return datetimeStream.str();
}

void ImageEventHandler::OnImageGrabbed (
    CBaslerUniversalInstantCamera& camera,
    const CBaslerUniversalGrabResultPtr& ptrGrabResult) {

    if (ptrGrabResult->GrabSucceeded()) {
        if (!(ptrGrabResult->ChunkTimestamp.IsReadable()))
            return;

        // Timestamp computation.
        unsigned long long int timestamp = ptrGrabResult->ChunkTimestamp.GetValue();
        if (timeOffset == 0) {
            auto nowTime = std::chrono::duration_cast<std::chrono::nanoseconds>
                (std::chrono::system_clock::now().time_since_epoch()).count();
            timeOffset = nowTime - timestamp;
            //cout << "Offset set to " << timeOffset << " ns." << endl;
        }
        timestamp = (timestamp + timeOffset)/1000000;

        // Determine the file name and ISO datetime.
        ostringstream imgNameStream;
        imgNameStream << "out/img/cam" << camera.GetDeviceInfo().GetSerialNumber()
                      << "img" << std::to_string(timestamp) << ".jpg";
        string imgNameString = imgNameStream.str();
        string datetimeString = NanosecondsToDatetime(timestamp);
        
        // Convert and save image.
        formatConverter.Convert(imgPylon, ptrGrabResult);
        Mat imgMat = Mat(ptrGrabResult->GetHeight(), ptrGrabResult->GetWidth(),
                              CV_8UC3, (uint8_t*)imgPylon.GetBuffer());
        imwrite(imgNameString, imgMat);

        // Write info to CSV file.
        timestampFile << "\"" << imgNameString << "\"," << timestamp
                      << ",\"" << datetimeString << "\"\n";
        cout << "Grabbed image " << imgNameString << " at " << datetimeString << endl;
    }
    else {
        cout << "Error: " << std::hex << ptrGrabResult->GetErrorCode()
             << std::dec << " " << ptrGrabResult->GetErrorDescription() << endl;
    }
}
