/**
 * Image Grabber
 * Application for grabbing images from Basler cameras using Pylon API.
 * @file            SignalHandler.h
 * @author          Daniel Konecny (xkonec75)
 * @organisation    Brno University of Technology - Faculty of Information Technologies
 * @date            27. 04. 2021
 */

#include <csignal>

using namespace std;

class user_exit : public exception {
public:
    user_exit();
};

void HandleExit(int s);
