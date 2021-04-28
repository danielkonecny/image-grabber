/**
 * Image Grabber
 * Application for grabbing images from Basler cameras using Pylon API.
 * @file            ConfigurationEventPrinter.cpp
 * @author          Daniel Konecny (xkonec75)
 * @organisation    Brno University of Technology - Faculty of Information Technologies
 * @date            02. 04. 2021
 */

#include <iostream>
#include <pylon/PylonIncludes.h>
#include <pylon/ConfigurationEventHandler.h>

#include "ConfigurationEventPrinter.h"

using namespace std;
using namespace Pylon;


void ConfigurationEventPrinter::OnAttach(CInstantCamera & /*camera*/) {
    cout << "OnAttach event" << endl;
}

void ConfigurationEventPrinter::OnAttached(CInstantCamera &camera) {
    cout << "OnAttached event for device " << camera.GetDeviceInfo().GetModelName() << endl;
}

void ConfigurationEventPrinter::OnOpen(CInstantCamera &camera) {
    cout << "OnOpen event for device " << camera.GetDeviceInfo().GetModelName() << endl;
}

void ConfigurationEventPrinter::OnOpened(CInstantCamera &camera) {
    cout << "OnOpened event for device " << camera.GetDeviceInfo().GetModelName() << endl;
}

void ConfigurationEventPrinter::OnGrabStart(CInstantCamera &camera) {
    cout << "OnGrabStart event for device " << camera.GetDeviceInfo().GetModelName() << endl;
}

void ConfigurationEventPrinter::OnGrabStarted(CInstantCamera &camera) {
    cout << "OnGrabStarted event for device " << camera.GetDeviceInfo().GetModelName() << endl;
}

void ConfigurationEventPrinter::OnGrabStop(CInstantCamera &camera) {
    cout << "OnGrabStop event for device " << camera.GetDeviceInfo().GetModelName() << endl;
}

void ConfigurationEventPrinter::OnGrabStopped(CInstantCamera &camera) {
    cout << "OnGrabStopped event for device " << camera.GetDeviceInfo().GetModelName() << endl;
}

void ConfigurationEventPrinter::OnClose(CInstantCamera &camera) {
    cout << "OnClose event for device " << camera.GetDeviceInfo().GetModelName() << endl;
}

void ConfigurationEventPrinter::OnClosed(CInstantCamera &camera) {
    cout << "OnClosed event for device " << camera.GetDeviceInfo().GetModelName() << endl;
}

void ConfigurationEventPrinter::OnDestroy(CInstantCamera &camera) {
    cout << "OnDestroy event for device " << camera.GetDeviceInfo().GetModelName() << endl;
}

void ConfigurationEventPrinter::OnDestroyed(CInstantCamera & /*camera*/) {
    cout << "OnDestroyed event" << endl;
}

void ConfigurationEventPrinter::OnDetach(CInstantCamera &camera) {
    cout << "OnDetach event for device " << camera.GetDeviceInfo().GetModelName() << endl;
}

void ConfigurationEventPrinter::OnDetached(CInstantCamera &camera) {
    cout << "OnDetached event for device " << camera.GetDeviceInfo().GetModelName() << endl;
}

void ConfigurationEventPrinter::OnGrabError(CInstantCamera &camera, const char *errorMessage) {
    cout << "OnGrabError event for device " << camera.GetDeviceInfo().GetModelName() << endl;
    cout << "Error Message: " << errorMessage << endl;
}

void ConfigurationEventPrinter::OnCameraDeviceRemoved(CInstantCamera &camera) {
    cout << "OnCameraDeviceRemoved event for device " << camera.GetDeviceInfo().GetModelName() << endl;
}