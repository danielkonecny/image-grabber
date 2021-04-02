#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <pylon/BaslerUniversalInstantCamera.h>
#include <pylon/BaslerUniversalInstantCameraArray.h>

using namespace std;
using namespace Pylon;

class CSampleImageEventHandler : public CBaslerUniversalImageEventHandler {
private:
    ofstream timestampFile;
    string dateString;
    unsigned long long int timeOffset;
    CPylonImage imgPylon;
    CImageFormatConverter formatConverter;

public:
    CSampleImageEventHandler ();

    ~CSampleImageEventHandler ();

    string NanosecondsToDatetime (unsigned long long int originalTime);

    virtual void OnImageGrabbed (CBaslerUniversalInstantCamera& camera,
                                const CBaslerUniversalGrabResultPtr& ptrGrabResult);
};
