/**
 * Image Grabber
 * Application for grabbing images from Basler cameras using Pylon API.
 * @file            ArgumentsParser.h
 * @version         1.0
 * @author          Daniel Konecny (xkonec75)
 * @organisation    Brno University of Technology - Faculty of Information Technologies
 * @date            03. 04. 2021
 */

class ArgumentsParser {
private:
	bool verbose = false;
	unsigned long long int waitTime = 10;

	static void PrintHelp ();

	static long long LoadNumber (char* numberAsChars);

public:	
    bool ProcessArguments (int argc, char* argv[]);

    bool IsVerbose() const;

    unsigned long long int GetWaitTime () const;
};
