#include <fstream>
#include <string>

#include "meta-settings.h"
#include "string-helpers.h"
#include "statemachine.h"
#include "dump-settings.h"

int main() {
    callback_t state = initial_state;
    std::string filename("test-scopped-preferences.conf");
    std::ifstream file(filename);

    while( state ) {
        state = state(file);
    }

    dump_header("test-scopped-preferences.h");
}
