/**
 * Image Grabber
 * Application for grabbing images from Basler cameras using Pylon API.
 * @file            ArgumentsParser.cpp
 * @version         1.0
 * @author          Daniel Konecny (xkonec75)
 * @organisation    Brno University of Technology - Faculty of Information Technologies
 * @date            03. 04. 2021
 */


#include <iostream>
#include <getopt.h>

#include "ArgumentsParser.h"

using namespace std;


long long ArgumentsParser::LoadNumber(char* numberAsChars) {
    string::size_type length;
    string numberAsString = numberAsChars;
    int numberAsInt;

    try {
        numberAsInt = stoi(numberAsString, &length);
    }
    catch (const invalid_argument &ia) {
        cerr << "Invalid number conversion" << endl;
        throw;
    }
    catch (const out_of_range &oor) {
        cerr << "Number out of range" << endl;
        throw;
    }
    if (numberAsString.substr(length).compare("") != 0) {
        cerr << "Invalid number format" << endl;
        throw;
    }

    return numberAsInt;
}

void ArgumentsParser::PrintHelp () {
    cout << "IMAGE GRABBER" << endl <<
            "-v (verbose)       Print information about the camera state." << endl << 
            "-t (time)          Set time (in ms) between images grabbed." << endl <<
            "-h (help)          Show help." << endl;
}

bool ArgumentsParser::ProcessArguments (int argc, char* argv[]) {
    const char* const short_opts = "vt:h";

    const option long_opts[] = {
        {"verbose", no_argument, nullptr, 'v'},
        {"time", required_argument, nullptr, 't'},
        {"help", no_argument, nullptr, 'h'},
        {nullptr, no_argument, nullptr, 0}
    };

    while (true) {
        const auto opt = getopt_long(argc, argv, short_opts, long_opts, nullptr);

        if (opt == -1) {
            break;
        }

        switch (opt) {
            case 'v':
                verbose = true;
                break;

            case 't':
                try {
                    waitTime = (unsigned long long)LoadNumber(optarg);
                }
                catch (...) {
                    return false;
                }
                break;

            case 'h':
            case '?':
            default:
                PrintHelp();
                return false;
        }
    }
	return true;
}


bool ArgumentsParser::IsVerbose() {
    return verbose;
}

unsigned long long int ArgumentsParser::GetWaitTime () {
    return waitTime;
}