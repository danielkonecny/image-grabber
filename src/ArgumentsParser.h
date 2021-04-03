/**
 * Image Grabber
 * Application for grabbing images from Basler cameras using Pylon API.
 * @file            ArgumentsParser.h
 * @version         1.0
 * @author          Daniel Konecny (xkonec75)
 * @organisation    Brno University of Technology - Faculty of Information Technologies
 * @date            02. 04. 2021
 */

class ArgumentsParser {
private:
	bool verbose = false;
	unsigned long long int imageCount = 0;

	void PrintHelp ();

	long long LoadNumber (char* numberAsChars);

public:	
    bool ProcessArguments (int argc, char* argv[]);

    bool IsVerbose();

    unsigned long long int GetImageCount ();
};
