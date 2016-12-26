#include <iostream>
#include <fstream>
#include <string>

#include "meta-settings.h"
#include "string-helpers.h"
#include "statemachine.h"
#include "dump-settings.h"

void show_usage() {
    std::cout << "usage: qobject-compiler file (without the .conf)" << std::endl;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        show_usage();
        return 0;
    }

    std::string filename(argv[1]);
    std::ifstream file(filename);

    if ( (file.rdstate() & std::ifstream::failbit) != 0) {
        std::cerr << "could not open file." << std::endl;
    }

    MetaConfiguration conf = parse_configuration(file);

    int substrSize = filename.find_last_of('.');
    std::string name_without_ext = filename.substr(0, substrSize);

    dump_header(conf, name_without_ext + ".h");
    dump_source(conf, name_without_ext + ".cpp");
}
