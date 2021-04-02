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
    void OnAttach (CInstantCamera& /*camera*/);

    void OnAttached (CInstantCamera& camera);

    void OnOpen (CInstantCamera& camera);

    void OnOpened (CInstantCamera& camera);

    void OnGrabStart (CInstantCamera& camera);

    void OnGrabStarted (CInstantCamera& camera);

    void OnGrabStop (CInstantCamera& camera);

    void OnGrabStopped (CInstantCamera& camera);

    void OnClose (CInstantCamera& camera);

    void OnClosed (CInstantCamera& camera);

    void OnDestroy (CInstantCamera& camera);

    void OnDestroyed (CInstantCamera& /*camera*/);

    void OnDetach (CInstantCamera& camera);

    void OnDetached (CInstantCamera& camera);

    void OnGrabError (CInstantCamera& camera, const char* errorMessage);

    void OnCameraDeviceRemoved (CInstantCamera& camera);
};
