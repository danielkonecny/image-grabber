/**
 * Image Grabber
 * Application for grabbing images from Basler cameras using Pylon API.
 * @file            ArgumentsParser.h
 * @author          Daniel Konecny (xkonec75)
 * @organisation    Brno University of Technology - Faculty of Information Technologies
 * @date            27. 04. 2021
 */

using namespace std;

#define DEFAULT_IMG_QUALITY 95
#define DEFAULT_FRAME_RATE 25
#define DEFAULT_OUT_DIR "out"
#define DEFAULT_IMAGE false
#define DEFAULT_VERBOSE false
#define DEFAULT_EXPOSURE_TIME (-1)
#define DEFAULT_GAIN (-1)
#define DEFAULT_BALANCE_WHITE_RED (-1)
#define DEFAULT_BALANCE_WHITE_GREEN (-1)
#define DEFAULT_BALANCE_WHITE_BLUE (-1)


class ArgumentsParser {
private:
    bool image = DEFAULT_IMAGE;
    string outDir = DEFAULT_OUT_DIR;
    int imgQuality = DEFAULT_IMG_QUALITY;
    unsigned int frameRate = DEFAULT_FRAME_RATE;
    bool verbose = DEFAULT_VERBOSE;

    double exposureTime = DEFAULT_EXPOSURE_TIME;
    double gain = DEFAULT_GAIN;
    double balanceWhiteRed = DEFAULT_BALANCE_WHITE_RED;
    double balanceWhiteGreen = DEFAULT_BALANCE_WHITE_GREEN;
    double balanceWhiteBlue = DEFAULT_BALANCE_WHITE_BLUE;

    static void PrintHelp();

    static long long LoadInteger(char *numberAsChars);

    static double LoadDouble(char *numberAsChars);

public:
    bool ProcessArguments(int argc, char *argv[]);

    bool IsVerbose() const;

    bool IsImage() const;

    string GetOutDir();

    int GetImgQuality() const;

    unsigned int GetFrameRate() const;

    double GetExposureTime() const;

    double GetGain() const;

    double GetBalanceWhiteRed() const;

    double GetBalanceWhiteGreen() const;

    double GetBalanceWhiteBlue() const;
};
