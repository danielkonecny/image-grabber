#include <iostream>

#include "SignalHandler.h"

using namespace std;

user_exit::user_exit() = default;

void HandleExit(int s) {
    cerr << "Caught signal " << s << "." << endl;
    throw user_exit();
}
