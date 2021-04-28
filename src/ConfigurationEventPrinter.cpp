/**
 * Image Grabber
 * Application for grabbing images from Basler cameras using Pylon API.
 * @file            ConfigurationEventPrinter.cpp
 * @author          Daniel Konecny (xkonec75)
 * @organisation    Brno University of Technology - Faculty of Information Technologies
 * @date            29. 04. 2021
 */

#include <iostream>
#include <chrono>
#include <pylon/PylonIncludes.h>
#include <pylon/ConfigurationEventHandler.h>

#include "ConfigurationEventPrinter.h"

using namespace std;
using namespace Pylon;


string GetDateTime() {
    time_t date = chrono::system_clock::to_time_t(chrono::system_clock::now());
    ostringstream datetimeStream;
    datetimeStream << "[" << put_time(localtime(&date), "%F %X") << "] ";
    return datetimeStream.str();
}

void ConfigurationEventPrinter::OnOpened(CInstantCamera &camera) {
    cout << GetDateTime() << "Camera " << camera.GetDeviceInfo().GetModelName() << " with serial number "
         << camera.GetDeviceInfo().GetSerialNumber() << " connected." << endl;
}

void ConfigurationEventPrinter::OnGrabStarted(CInstantCamera &camera) {
    cout << GetDateTime() << "Camera " << camera.GetDeviceInfo().GetSerialNumber() << " started grabbing images."
         << endl;
}

void ConfigurationEventPrinter::OnGrabStopped(CInstantCamera &camera) {
    cout << GetDateTime() << "Camera " << camera.GetDeviceInfo().GetSerialNumber() << " stopped grabbing images."
         << endl;
}

void ConfigurationEventPrinter::OnDestroy(CInstantCamera &camera) {
    cout << GetDateTime() << "Camera " << camera.GetDeviceInfo().GetSerialNumber() << " removed." << endl;
}
