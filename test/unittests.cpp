#include <fstream>
#include <string>
#include <vector>
#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/range/adaptor/filtered.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include "meta-settings.h"
#include "string-helpers.h"
#include "statemachine.h"
#include "dump-settings.h"

using namespace boost;
using boost::filesystem::directory_entry;
using boost::filesystem::directory_iterator;

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

std::vector<std::string> find_filenames(int argc, char *argv[]) {
    std::vector<std::string> filenames;
    if (argc == 1) {
        auto files = adaptors::filter(directory_iterator(filesystem::absolute(".")),
                                    [](directory_entry& s){
            return algorithm::ends_with(s.path().generic_string(), ".conf");
        });
        for(const auto& file : files) {
            filenames.push_back(file.path().generic_string());
        }
    } else for (int i = 1; i < argc; i++) {
        if (algorithm::ends_with(argv[i], ".conf")) {
            filenames.push_back(argv[i]);
        }
    }
    return filenames;
}

int main(int argc, char *argv[]) {
    std::vector<std::string> filenames = find_filenames(argc, argv);
    assert(filenames.size());

    for(const auto& file : filenames) {
        std::cout << "Starting test of " << file << std::endl;
        int retcode = test_file(file);
        if (retcode)
            return retcode;
    }
    return 0;
}
