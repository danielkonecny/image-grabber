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
            catch (const runtime_error &e) {
                cerr << "Could not grab an image." << endl;
                exitCode = 1;
            }
            catch (...) {
                cerr << "ERROR." << endl;
                exitCode = 1;
            }

            PylonTerminate();
        } else {
            exitCode = 1;
        }
    }
    catch (const runtime_error &e) {
        cerr << "Runtime exception occurred." << endl;
        exitCode = 1;
    }
    catch (const user_exit &e) {
        cout << "Closing." << endl;
        exitCode = 1;
    }
    catch (...) {
        cerr << "ERROR." << endl;
        exitCode = 1;
    }

    return exitCode;
}
