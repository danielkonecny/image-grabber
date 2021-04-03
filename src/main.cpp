/**
 * Image Grabber
 * Application for grabbing images from Basler cameras using Pylon API.
 * @file            main.cpp
 * @version         2.0
 * @author          Daniel Konecny (xkonec75)
 * @organisation    Brno University of Technology - Faculty of Information Technologies
 * @date            03. 04. 2021
 */ 

#include <iostream>
#include <pylon/PylonIncludes.h>

#include "main.h"
#include "ImageGrabber.h"

using namespace std;


int main (int argc, char* argv[]) {
    int exitCode = 0;

    ArgumentsParser parser;

    if (parser.ProcessArguments(argc, argv)) {
        PylonInitialize();

        try {
            ImageGrabber imageGrabber(parser);

            try {
                imageGrabber.Grab(parser);
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
    }
    else {
        exitCode = 1;
    }

    return exitCode;
}
