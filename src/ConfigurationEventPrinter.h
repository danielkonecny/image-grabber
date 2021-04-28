/**
 * Image Grabber
 * Application for grabbing images from Basler cameras using Pylon API.
 * @file            ConfigurationEventPrinter.h
 * @author          Daniel Konecny (xkonec75)
 * @organisation    Brno University of Technology - Faculty of Information Technologies
 * @date            29. 04. 2021
 */

#include <iostream>
#include <pylon/ConfigurationEventHandler.h>

using namespace std;
using namespace Pylon;

string GetDateTime();

class ConfigurationEventPrinter : public CConfigurationEventHandler {
public:
    void OnOpened(CInstantCamera &camera) override;

    void OnGrabStarted(CInstantCamera &camera) override;

    void OnGrabStopped(CInstantCamera &camera) override;

    void OnDestroy(CInstantCamera &camera) override;
};
