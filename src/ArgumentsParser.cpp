/**
 * Image Grabber
 * Application for grabbing images from Basler cameras using Pylon API.
 * @file            ArgumentsParser.cpp
 * @author          Daniel Konecny (xkonec75)
 * @organisation    Brno University of Technology - Faculty of Information Technologies
 * @date            27. 04. 2021
 */


#include <iostream>
#include <getopt.h>

#include "ArgumentsParser.h"

using namespace std;

long long ArgumentsParser::LoadInteger(char *numberAsChars) {
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

double ArgumentsParser::LoadDouble(char *numberAsChars) {
    string::size_type length;
    string numberAsString = numberAsChars;
    double numberAsDouble;

    try {
        numberAsDouble = stod(numberAsString, &length);
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

    return numberAsDouble;
}

void ArgumentsParser::PrintHelp() {
    cout << "IMAGE GRABBER" << endl <<
         "-e (--exposure)   Set exposure time in microseconds, range may vary (for example 28 - 1e7)." << endl <<
         "-f (--framerate)  Set framerate (fps) of recording (default: " << DEFAULT_FRAME_RATE << ")." << endl <<
         "-g (--gain)       Set gain, range may vary (for example 0 - 23.59)." << endl <<
         "-h (--help)       Show help." << endl <<
         "-i (--image)      Save images instead of video." << endl <<
         "-o (--output)     Set folder for video/image and log output (default: " << DEFAULT_OUT_DIR << ")." << endl <<
         "                  This folder has to contain folders: img, log, vid." << endl <<
         "-q (--quality)    Set image quality between 0 and 100, the higher is the better (default: " <<
         DEFAULT_IMG_QUALITY << ")." << endl <<
         "-v (--verbose)    Print information about the camera state." << endl;
}

bool ArgumentsParser::ProcessArguments(int argc, char *argv[]) {
    const char *const short_opts = "e:f:g:hio:q:v";

    const option long_opts[] = {
            {"exposure",  required_argument, nullptr, 'e'},
            {"framerate", required_argument, nullptr, 'f'},
            {"gain",      required_argument, nullptr, 'g'},
            {"help",      no_argument,       nullptr, 'h'},
            {"image",     no_argument,       nullptr, 'i'},
            {"output",    required_argument, nullptr, 'o'},
            {"quality",   required_argument, nullptr, 'q'},
            {"verbose",   no_argument,       nullptr, 'v'},
            {nullptr,     no_argument,       nullptr, 0}
    };

    while (true) {
        const auto opt = getopt_long(argc, argv, short_opts, long_opts, nullptr);

        if (opt == -1) {
            break;
        }

        switch (opt) {
            case 'e':
                try {
                    exposureTime = LoadDouble(optarg);
                }
                catch (...) {
                    return false;
                }
                break;

            case 'f':
                try {
                    frameRate = (unsigned int) LoadInteger(optarg);
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

            case 'g':
                try {
                    gain = LoadDouble(optarg);
                }
                catch (...) {
                    return false;
                }
                break;

            case 'v':
                verbose = true;
                break;

            case 'i':
                image = true;
                break;

            case 'o':
                outDir = optarg;
                break;

            case 'q':
                try {
                    imgQuality = (int) LoadInteger(optarg);
                }
                catch (...) {
                    return false;
                }
                if (imgQuality < 0 || imgQuality > 100) {
                    cerr << "ERROR: Image quality has to be between 0 and 100. Currently " << imgQuality <<
                         ". Quality automatically set to default value " << DEFAULT_IMG_QUALITY << "." << endl;
                    imgQuality = DEFAULT_IMG_QUALITY;
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

int ArgumentsParser::GetImgQuality() const {
    return imgQuality;
}

unsigned int ArgumentsParser::GetFrameRate() const {
    return frameRate;
}

double ArgumentsParser::getExposureTime() const {
    return exposureTime;
}

double ArgumentsParser::getGain() const {
    return gain;
}
