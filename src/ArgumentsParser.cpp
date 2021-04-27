/**
 * Image Grabber
 * Application for grabbing images from Basler cameras using Pylon API.
 * @file            ArgumentsParser.cpp
 * @version         2.0
 * @author          Daniel Konecny (xkonec75)
 * @organisation    Brno University of Technology - Faculty of Information Technologies
 * @date            27. 04. 2021
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
         "-f (--framerate)  Set framerate (fps) of recording (default: " << DEFAULT_FRAME_RATE << ")." << endl <<
         "-h (--help)       Show help." << endl <<
         "-i (--image)      Save images instead of video. Optional argument setting the image quality." << endl <<
         "                  Quality has to be between 0 and 100, the higher is the better (default: " <<
         DEFAULT_IMG_QUALITY << ")." << endl <<
         "-o (--output)     Set folder for video/image and log output (default: " << DEFAULT_OUT_DIR << ")." << endl <<
         "                  This folder has to contain folders: img, log, vid." << endl <<
         "-v (--verbose)    Print information about the camera state." << endl;
}

bool ArgumentsParser::ProcessArguments(int argc, char *argv[]) {
    const char *const short_opts = "f:hi::o:v";

    const option long_opts[] = {
            {"framerate", required_argument, nullptr, 'f'},
            {"help",      no_argument,       nullptr, 'h'},
            {"image",     optional_argument, nullptr, 'i'},
            {"output",    required_argument, nullptr, 'o'},
            {"verbose",   no_argument,       nullptr, 'v'},
            {nullptr,     no_argument,       nullptr, 0}
    };

    while (true) {
        const auto opt = getopt_long(argc, argv, short_opts, long_opts, nullptr);

        if (opt == -1) {
            break;
        }

        switch (opt) {
            case 'f':
                try {
                    frameRate = (unsigned int) LoadNumber(optarg);
                }
                catch (...) {
                    return false;
                }
                if (frameRate < 0) {
                    cerr << "ERROR: Frame rate has to be greater than 0. Currently " << frameRate <<
                         ". Frame rate automatically set to default value " << DEFAULT_FRAME_RATE << "." << endl;
                    frameRate = DEFAULT_FRAME_RATE;
                }
                break;

            case 'v':
                verbose = true;
                break;

            case 'i':
                image = true;
                if (optarg == nullptr && argv[optind] != nullptr && argv[optind][0] != '-') {
                    imgQuality = (int) LoadNumber(argv[optind]);
                    optind++;
                } else {
                    try {
                        imgQuality = (int) LoadNumber(optarg);
                    }
                    catch (...) {
                        return false;
                    }
                }
                if (imgQuality < 0 || imgQuality > 100) {
                    cerr << "ERROR: Image quality has to be between 0 and 100. Currently " << imgQuality <<
                         ". Quality automatically set to default value " << DEFAULT_IMG_QUALITY << "." << endl;
                    imgQuality = DEFAULT_IMG_QUALITY;
                }
                break;

            case 'o':
                outDir = optarg;
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

int ArgumentsParser::GetImgQuality() const {
    return imgQuality;
}

unsigned int ArgumentsParser::GetFrameRate() const {
    return frameRate;
}