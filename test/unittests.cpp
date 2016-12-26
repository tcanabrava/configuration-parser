#include <fstream>
#include <string>
#include <vector>
#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/range/adaptor/filtered.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include "string-helpers.h"
#include "meta-settings.h"
#include "statemachine.h"
#include "dump-settings.h"

using namespace boost;
using boost::filesystem::directory_entry;
using boost::filesystem::directory_iterator;

Q_DECLARE_LOGGING_CATEGORY(unittests)
Q_LOGGING_CATEGORY(unittests, "unittests")

/* Check if the file represented by filename + one of the extensions exists on the filesystem */
bool check_file_exists(std::string filename, const std::vector<std::string>& extensions) {
    for(const auto& extension : extensions) {
        if(!filesystem::exists(filename + extension)) {
            std::cout << "Filename informed doesn't exist: " << (filename + extension) << std::endl;
            return false;
        }
    }
    return true;
}

/* Open the generated file and a existing, handmande file and verify them line-by-line,
 * the files should be exactly the same, if they are not then the generation
 * did something wrong and we need to fix that.
 * */
bool test_specific_file(const std::string& filename,
                        const std::pair<std::string, std::string>& extensions) {
    std::ifstream generated(filename + extensions.first);
    std::ifstream expected(filename + extensions.second);
    std::string gen, exp;
    while(!generated.eof() || !expected.eof()) {
        generated >> gen;
        expected >> exp;
        if (gen != exp) {
            std::cout << "Error at " << exp;
            return false;
        }
    }
    return true;
}

/* generates the header and sources files and call them
 * test_specific_file for each of them.
 * */
int test_file(const std::string& filename) {
    qCDebug(unittests) << "Starting the test for:" << filename;
    std::ifstream file(filename + ".conf");

    MetaConfiguration conf = parse_configuration(file);

    if (check_file_exists(filename, {".conf.h"})) {
        dump_header(conf, filename + ".h");
        if (!test_specific_file(filename, {".h", ".conf.h"})) {
            qCDebug(unittests) << "Error on" << filename << ".h";
            return -1;
        }
    }

    if (check_file_exists(filename, {".conf.cpp"})) {
        dump_source(conf, filename + ".cpp");
        if (!test_specific_file(filename, {".cpp", ".conf.cpp"})){
            qCDebug(unittests) << "Error on" << filename << ".cpp";
            return -1;
        }
    }
    qCDebug(unittests) << "Finished test for: " << filename << " without errors";
    return 0;
}

/* Uses all the files passed thru parameters or, if no file
 * was avaliable via parameter, tries to find all the .conf
 * files in this folder and returns them.
 */
std::vector<std::string> find_filenames(int argc, char *argv[]) {
    std::vector<std::string> filenames;
    if (argc == 1) {
        auto files = adaptors::filter(directory_iterator(filesystem::absolute(".")),
                                    [](directory_entry& s){
            return algorithm::ends_with(s.path().generic_string(), ".conf");
        });
        for(const auto& file : files) {
            // extract the .conf
            std::string filename = file.path().generic_string();
            int substrSize = filename.find_last_of('.');
            filenames.push_back(filename.substr(0, substrSize));
        }
    } else for (int i = 1; i < argc; i++) {
        if (algorithm::ends_with(argv[i], ".conf")) {
            std::string file(argv[i]);
            filenames.push_back(file.substr(0, file.find_last_of('.')));
        }
    }
    return filenames;
}

/* runs all configuration files on the test cases*/
int main(int argc, char *argv[]) {
    qCDebug(unittests) << "Starting unittests";
    std::vector<std::string> filenames = find_filenames(argc, argv);
    assert(filenames.size());
    qCDebug(unittests) << "Testing the following files:" << filenames;

    for(const auto& file : filenames) {
        assert(test_file(file) == 0);
    }
    return 0;
}
