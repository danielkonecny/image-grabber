/**
 * Image Grabber
 * Application for grabbing images from Basler cameras using Pylon API.
 * @file            ArgumentsParser.h
 * @version         1.0
 * @author          Daniel Konecny (xkonec75)
 * @organisation    Brno University of Technology - Faculty of Information Technologies
 * @date            03. 04. 2021
 */

using namespace std;

#define DEFAULT_IMG_QUALITY 95
#define DEFAULT_WAIT_TIME 10

class ArgumentsParser {
private:
    bool image = false;
    string outDir = "out";
    int imgQuality = DEFAULT_IMG_QUALITY;
    unsigned long long int waitTime = DEFAULT_WAIT_TIME;
    bool verbose = false;

    static void PrintHelp();

    static long long LoadNumber(char *numberAsChars);

public:
    bool ProcessArguments(int argc, char *argv[]);

    bool IsVerbose() const;

    bool IsImage() const;

    string GetOutDir();

    int GetImgQuality();

    unsigned long long int GetWaitTime() const;
};
