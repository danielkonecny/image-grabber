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
#define DEFAULT_FRAME_RATE 25
#define DEFAULT_OUT_DIR "out"
#define DEFAULT_IMAGE false
#define DEFAULT_VERBOSE false

class ArgumentsParser {
private:
    bool image = DEFAULT_IMAGE;
    string outDir = DEFAULT_OUT_DIR;
    int imgQuality = DEFAULT_IMG_QUALITY;
    unsigned int frameRate = DEFAULT_FRAME_RATE;
    bool verbose = DEFAULT_VERBOSE;

    static void PrintHelp();

    static long long LoadNumber(char *numberAsChars);

public:
    bool ProcessArguments(int argc, char *argv[]);

    bool IsVerbose() const;

    bool IsImage() const;

    string GetOutDir();

    int GetImgQuality() const;

    unsigned int GetFrameRate() const;
};
