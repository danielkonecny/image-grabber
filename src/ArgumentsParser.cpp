/**
 * Image Grabber
 * Application for grabbing images from Basler cameras using Pylon API.
 * @file            ArgumentsParser.cpp
 * @author          Daniel Konecny (xkonec75)
 * @organisation    Brno University of Technology - Faculty of Information Technologies
 * @date            20. 07. 2021
 */


#include <iostream>
#include <getopt.h>

#include "ArgumentsParser.h"

using namespace std;

long long int ArgumentsParser::LoadInteger(char *numberAsChars) {
    string::size_type length;
    string numberAsString = numberAsChars;
    long long int numberAsInt;

    try {
        numberAsInt = stoll(numberAsString, &length);
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
         "-R (--bwr)        Balance white (red channel), larger than 0, double precision," << endl <<
         "                  (default: auto continuous), (=1 red intensity unchanged, >1 increased, <1 decreased)."
         << endl <<
         "-G (--bwg)        Balance white (green channel), larger than 0, double precision," << endl <<
         "                  (default: auto continuous), (=1 green intensity unchanged, >1 increased, <1 decreased)."
         << endl <<
         "-B (--bwb)        Balance white (blue channel), larger than 0, double precision," << endl <<
         "                  (default: auto continuous), (=1 blue intensity unchanged, >1 increased, <1 decreased)."
         << endl <<
         "-e (--exposure)   Set exposure time in microseconds, range may vary, double precision," << endl <<
         "                  (for example 28 - 1e7), (default: auto continuous)." << endl <<
         "-f (--framerate)  Set framerate (fps) of recording, double precision, (default: " << DEFAULT_FRAME_RATE <<
         ")." << endl <<
         "-g (--gain)       Set gain, range may vary (for example 0 - 23.59), (default: auto continuous)." << endl <<
         "-h (--help)       Show help." << endl <<
         "-i (--image)      Save images instead of video." << endl <<
         "-o (--output)     Set folder for video/image and log output (default: " << DEFAULT_OUT_DIR << ")." << endl <<
         "-q (--quality)    Set image quality (use only together with `-i`) between 0 and 100," << endl <<
         "                  the higher is the better, integer precision, (default: " << DEFAULT_IMG_QUALITY << ")."
         << endl <<
         "-v (--verbose)    Print information about the camera state." << endl;
}

bool ArgumentsParser::ProcessArguments(int argc, char *argv[]) {
    const char *const short_opts = "R:G:B:e:f:g:hio:q:v";

    const option long_opts[] = {
            {"bwr",       required_argument, nullptr, 'R'},
            {"bwg",       required_argument, nullptr, 'G'},
            {"bwb",       required_argument, nullptr, 'B'},
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
            case 'R':
                try {
                    balanceWhiteRed = LoadDouble(optarg);
                }
                catch (...) {
                    return false;
                }
                if (balanceWhiteRed < 0) {
                    cerr << "ERROR: Balance white (red channel) has to be greater than 0. Currently " <<
                         balanceWhiteRed << ". Balance white (red channel) set automatically." << endl;
                    balanceWhiteRed = DEFAULT_BALANCE_WHITE_RED;
                }
                break;

            case 'G':
                try {
                    balanceWhiteGreen = LoadDouble(optarg);
                }
                catch (...) {
                    return false;
                }
                if (balanceWhiteGreen < 0) {
                    cerr << "ERROR: Balance white (green channel) has to be greater than 0. Currently " <<
                         balanceWhiteGreen << ". Balance white (green channel) set automatically." << endl;
                    balanceWhiteGreen = DEFAULT_BALANCE_WHITE_GREEN;
                }
                break;

            case 'B':
                try {
                    balanceWhiteBlue = LoadDouble(optarg);
                }
                catch (...) {
                    return false;
                }
                if (balanceWhiteBlue < 0) {
                    cerr << "ERROR: Balance white (blue channel) has to be greater than 0. Currently " <<
                         balanceWhiteBlue << ". Balance white (blue channel) set automatically." << endl;
                    balanceWhiteBlue = DEFAULT_BALANCE_WHITE_BLUE;
                }
                break;

            case 'e':
                try {
                    exposureTime = LoadDouble(optarg);
                }
                catch (...) {
                    return false;
                }
                if (exposureTime < 0) {
                    cerr << "ERROR: Exposure time has to be greater than 0. Currently " << exposureTime <<
                         ". Exposure time set automatically." << endl;
                    exposureTime = DEFAULT_EXPOSURE_TIME;
                }
                break;

            case 'f':
                try {
                    frameRate = LoadDouble(optarg);
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
                if (gain < 0) {
                    cerr << "ERROR: Gain has to be greater than 0. Currently " << gain << ". Gain set automatically."
                         << endl;
                    gain = DEFAULT_GAIN;
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

double ArgumentsParser::GetFrameRate() const {
    return frameRate;
}

double ArgumentsParser::GetExposureTime() const {
    return exposureTime;
}

double ArgumentsParser::GetGain() const {
    return gain;
}

double ArgumentsParser::GetBalanceWhiteRed() const {
    return balanceWhiteRed;
}

double ArgumentsParser::GetBalanceWhiteGreen() const {
    return balanceWhiteGreen;
}

double ArgumentsParser::GetBalanceWhiteBlue() const {
    return balanceWhiteBlue;
}
