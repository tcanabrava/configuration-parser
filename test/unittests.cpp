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

bool check_file_exists(std::string filename, const std::vector<std::string>& extensions) {
    for(const auto& extension : extensions) {
        if(!filesystem::exists(filename + extension)) {
            std::cerr << "Filename informed doesn't exists: " << filename;
            return false;
        }
    }
    return true;
}

bool test_specific_file(const std::string& filename,
                        const std::pair<std::string, std::string>& extensions) {
    std::ifstream generated(filename + extensions.first);
    std::ifstream expected(filename + extensions.second);
    std::string gen, exp;
    while(!generated.eof() || !expected.eof()) {
        generated >> gen;
        expected >> exp;
        if (gen != exp) {
            std::cout << "Error at" << exp;
            return false;
        }
    }
    return true;
}

int test_file(const std::string& filename) {
    std::ifstream file(filename + ".conf");
    int error;

    callback_t state = initial_state;
    while( state ) {
        state = state(file, error);
    }

    if (check_file_exists(filename, {".h", "-header.expected"})) {
        dump_header(filename + ".h");
        if (!test_specific_file(filename, {".h", "-header.expected"})) {
            return -1;
        }
    }

    if (check_file_exists(filename, {".cpp", "-source.expected"})) {
        dump_source(filename + ".cpp");
        if (!test_specific_file(filename, {".cpp", "-source.expected"})){
            return -1;
        }
    }
    return 0;
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
        assert(test_file(file) == 0);
    }
    return 0;
}
