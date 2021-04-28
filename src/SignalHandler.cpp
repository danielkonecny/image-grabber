/**
 * Image Grabber
 * Application for grabbing images from Basler cameras using Pylon API.
 * @file            SignalHandler.cpp
 * @author          Daniel Konecny (xkonec75)
 * @organisation    Brno University of Technology - Faculty of Information Technologies
 * @date            29. 04. 2021
 */

#include <iostream>

#include "SignalHandler.h"

using namespace std;

user_exit::user_exit() = default;

void HandleExit(int s) {
    (void) s;
    throw user_exit();
}
