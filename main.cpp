#include <iostream>
#include <pylon/PylonIncludes.h>

#include "ImageGrabber.h"

using namespace std;


int main ( int /*argc*/, char* /*argv*/[] ) {
    int exitCode = 0;

    PylonInitialize();

    try {
        ImageGrabber imageGrabber;

        try {
            imageGrabber.Grab();
        }
        catch (const GenericException& e) {
            cerr << "Could not grab an image: " << endl << e.GetDescription() << endl;
            exitCode = 1;
        }
    }
    catch (const GenericException& e) {
        cerr << "An exception occurred." << endl << e.GetDescription() << endl;
        exitCode = 1;
    }

    PylonTerminate();

    return exitCode;
}
