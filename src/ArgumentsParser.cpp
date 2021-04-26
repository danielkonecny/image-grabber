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

long long ArgumentsParser::LoadNumber(char *numberAsChars) {
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
    if (!numberAsString.substr(length).empty()) {
        cerr << "Invalid number format" << endl;
        throw;
    }

    return numberAsInt;
}

void ArgumentsParser::PrintHelp() {
    cout << "IMAGE GRABBER" << endl <<
         "-h (help)          Show help." << endl <<
         "-i (image)         Save images instead of video. Optional argument setting the image quality." << endl <<
         "                   Quality has to be between 0 and 100, the higher is the better (default: " <<
         DEFAULT_IMG_QUALITY << ")." << endl <<
         "                   Quality has to be written right after the option without space (e. g. -i42)." << endl <<
         "-o (output)        Set folder for video/image and log output (default: " << DEFAULT_OUT_DIR << ")." << endl <<
         "                   This folder has to contain folders: img, log, vid." << endl <<
         "-t (time)          Set time (in ms) between images grabbed (default: " << DEFAULT_WAIT_TIME << ")." << endl <<
         "-v (verbose)       Print information about the camera state." << endl;
}

bool ArgumentsParser::ProcessArguments(int argc, char *argv[]) {
    const char *const short_opts = "hi::o:t:v";

    const option long_opts[] = {
            {"help",    no_argument,       nullptr, 'h'},
            {"image",   optional_argument, nullptr, 'i'},
            {"output",  required_argument, nullptr, 'o'},
            {"time",    required_argument, nullptr, 't'},
            {"verbose", no_argument,       nullptr, 'v'},
            {nullptr,   no_argument,       nullptr, 0}
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

            case 'i':
                image = true;
                if (optarg) {
                    try {
                        imgQuality = (int) LoadNumber(optarg);
                    }
                    catch (...) {
                        return false;
                    }
                    if (imgQuality < 0 || imgQuality > 100) {
                        cerr << "ERROR: Image quality has to be between 0 and 100. Currently " << imgQuality <<
                             ". Quality automatically set to default value " << DEFAULT_IMG_QUALITY << "." << endl;
                        imgQuality = DEFAULT_IMG_QUALITY;
                    }
                }
                break;

            case 'o':
                outDir = optarg;
                break;

            case 't':
                try {
                    waitTime = (unsigned long long) LoadNumber(optarg);
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


bool ArgumentsParser::IsVerbose() const {
    return verbose;
}

bool ArgumentsParser::IsImage() const {
    return image;
}

string ArgumentsParser::GetOutDir() {
    return outDir;
}

int ArgumentsParser::GetImgQuality() {
    return imgQuality;
}

unsigned long long int ArgumentsParser::GetWaitTime() const {
    return waitTime;
}