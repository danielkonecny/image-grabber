/**
 * Image Grabber
 * Application for grabbing images from Basler cameras using Pylon API.
 * @file            main.cpp
 * @version         3.2
 * @author          Daniel Konecny (xkonec75)
 * @organisation    Brno University of Technology - Faculty of Information Technologies
 * @date            28. 04. 2021
 */

#include <iostream>
#include <csignal>
#include <pylon/PylonIncludes.h>

#include "main.h"
#include "ImageGrabber.h"
#include "SignalHandler.h"

using namespace std;

int main(int argc, char *argv[]) {
    int exitCode = 0;

    try {
        signal(SIGINT, HandleExit);
        signal(SIGTERM, HandleExit);

        ArgumentsParser parser;

        if (parser.ProcessArguments(argc, argv)) {
            PylonInitialize();

            ImageGrabber imageGrabber(parser);

            try {
                imageGrabber.Grab(parser);
            }
            catch (const GenericException &e) {
                cerr << "Could not grab an image: " << endl << e.GetDescription() << endl;
                exitCode = 1;
            }

            PylonTerminate();
        } else {
            exitCode = 1;
        }
    }
    catch (const GenericException &e) {
        cerr << "An exception occurred." << endl << e.GetDescription() << endl;
        exitCode = 1;
    }
    catch (const user_exit &e) {
        cout << "Closing." << endl;
    }

    return exitCode;
}
