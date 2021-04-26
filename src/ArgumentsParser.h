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

class ArgumentsParser {
private:
    bool image = false;
    string outDir = "out";
    unsigned long long int waitTime = 10;
    bool verbose = false;

    static void PrintHelp();

    static long long LoadNumber(char *numberAsChars);

public:
    bool ProcessArguments(int argc, char *argv[]);

    bool IsVerbose() const;

    bool IsImage() const;

    string GetOutDir();

    unsigned long long int GetWaitTime() const;
};
