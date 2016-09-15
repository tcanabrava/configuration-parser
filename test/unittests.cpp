#include <fstream>
#include <string>
#include <vector>
#include <iostream>

#include "meta-settings.h"
#include "string-helpers.h"
#include "statemachine.h"
#include "dump-settings.h"

int test_file(const std::string& filename) {
    std::ifstream file(filename + ".conf");
    int error;

    callback_t state = initial_state;
    while( state ) {
        state = state(file, error);
    }
    dump_header(filename + ".h");

    {
        std::ifstream generated(filename + ".h");
        std::ifstream expected(filename + "-header.expected");
        std::string gen, exp;
        while(!generated.eof() || !expected.eof()) {
            generated >> gen;
            expected >> exp;
            if (gen != exp) {
                std::cout << "Error at" << exp;
                return -1;
            }
        }
    }

    dump_source(filename + ".cpp");
    {
        std::ifstream generated(filename + ".cpp");
        std::ifstream expected(filename + "-source.expected");
        std::string gen, exp;
        while(!generated.eof() || !expected.eof()) {
            generated >> gen;
            expected >> exp;
            if (gen != exp) {
                std::cout << "Error at " << exp;
                return -1;
            }
        }
    }
}

int main() {
    std::vector<std::string> filenames = {
        "test-class-array",
         "test-empty-prefs",
         "test-includes",
         "test-properties",
         "test-scopped-preferences"
    };

    for(const auto& file : filenames) {
        std::cout << "Starting test of " << file << std::endl;
        int retcode = test_file(file);
        if (retcode)
            return retcode;
    }
    return 0;
}
