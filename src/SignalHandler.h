#include <csignal>

using namespace std;

class user_exit : public exception {
public:
    user_exit();
};

void HandleExit(int s);
