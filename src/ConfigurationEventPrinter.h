/**
 * Image Grabber
 * Application for grabbing images from Basler cameras using Pylon API.
 * @file            ConfigurationEventPrinter.h
 * @version         1.0
 * @author          Daniel Konecny (xkonec75)
 * @organisation    Brno University of Technology - Faculty of Information Technologies
 * @date            02. 04. 2021
 */ 

#include <iostream>
#include <pylon/ConfigurationEventHandler.h>

using namespace Pylon;


class ConfigurationEventPrinter : public CConfigurationEventHandler {
public:
    void OnAttach (CInstantCamera& /*camera*/) override;

    void OnAttached (CInstantCamera& camera) override;

    void OnOpen (CInstantCamera& camera) override;

    void OnOpened (CInstantCamera& camera) override;

    void OnGrabStart (CInstantCamera& camera) override;

    void OnGrabStarted (CInstantCamera& camera) override;

    void OnGrabStop (CInstantCamera& camera) override;

    void OnGrabStopped (CInstantCamera& camera) override;

    void OnClose (CInstantCamera& camera) override;

    void OnClosed (CInstantCamera& camera) override;

    void OnDestroy (CInstantCamera& camera) override;

    void OnDestroyed (CInstantCamera& /*camera*/) override;

    void OnDetach (CInstantCamera& camera) override;

    void OnDetached (CInstantCamera& camera) override;

    void OnGrabError (CInstantCamera& camera, const char* errorMessage) override;

    void OnCameraDeviceRemoved (CInstantCamera& camera) override;
};
