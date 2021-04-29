/**
 * Image Grabber
 * Application for grabbing images from Basler cameras using Pylon API.
 * @file            ArgumentsParser.h
 * @author          Daniel Konecny (xkonec75)
 * @organisation    Brno University of Technology - Faculty of Information Technologies
 * @date            30. 04. 2021
 */

using namespace std;

#define DEFAULT_IMG_QUALITY 95
#define DEFAULT_FRAME_RATE 25.0
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
    bool verbose = DEFAULT_VERBOSE;

    double frameRate = DEFAULT_FRAME_RATE;
    double exposureTime = DEFAULT_EXPOSURE_TIME;
    double gain = DEFAULT_GAIN;
    double balanceWhiteRed = DEFAULT_BALANCE_WHITE_RED;
    double balanceWhiteGreen = DEFAULT_BALANCE_WHITE_GREEN;
    double balanceWhiteBlue = DEFAULT_BALANCE_WHITE_BLUE;

    static void PrintHelp();

    static long long int LoadInteger(char *numberAsChars);

    static double LoadDouble(char *numberAsChars);

public:
    bool ProcessArguments(int argc, char *argv[]);

    [[nodiscard]] bool IsVerbose() const;

    [[nodiscard]] bool IsImage() const;

    string GetOutDir();

    [[nodiscard]] int GetImgQuality() const;

    [[nodiscard]] double GetFrameRate() const;

    [[nodiscard]] double GetExposureTime() const;

    [[nodiscard]] double GetGain() const;

    [[nodiscard]] double GetBalanceWhiteRed() const;

    [[nodiscard]] double GetBalanceWhiteGreen() const;

    [[nodiscard]] double GetBalanceWhiteBlue() const;
};
