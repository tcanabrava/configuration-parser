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
    std::ifstream file(filename + ".conf");

    if ( (file.rdstate() & std::ifstream::failbit) != 0) {
        std::cerr << "could not open file." << std::endl;
    }

    MetaConfiguration conf = parse_configuration(file);

    dump_header(conf, filename + ".h");
    dump_source(conf, filename + ".cpp");
}
