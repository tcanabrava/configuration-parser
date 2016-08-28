#include <iostream>
#include <fstream>
#include <functional>
#include <map>
#include <vector>
#include <string>
#include <memory>

#include "meta-settings.h"
#include "string-helpers.h"
#include "statemachine.h"
#include "dump-settings.h"

void show_usage() {
    std::cout << "usage: qobject-compiler if=file.conf of=file.cpp" << std::endl;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        show_usage();
        return 0;
    }

    callback_t state = guess_state;
    std::cout << "file to read: " << argv[1] << std::endl;

    std::ifstream file("test.conf");
    if ( (file.rdstate() & std::ifstream::failbit) != 0) {
        std::cerr << "could not open file." << std::endl;
    }

    // Start the state machine
    while( state ) {
        state = state(file);
    }
    dump_header();
    dump_source();
}
