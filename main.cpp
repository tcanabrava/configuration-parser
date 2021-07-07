#include <fstream>
#include <iostream>
#include <string>

#include "dump-settings.h"
#include "meta-settings.h"
#include "statemachine.h"
#include "string-helpers.h"

#include <QDebug>
#include <QFileInfo>
#include <filesystem>

void show_usage(const char appname[]) {
  qDebug() << "usage" << appname << "file.conf";
  qDebug() << "\t this will generate a header / source pair";
  qDebug() << "\t with everything you need to start coding.";
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    show_usage(argv[0]);
    return 0;
  }

  std::string filename(argv[1]);
  if (filename == "-h" || filename == "--help") {
    show_usage(argv[0]);
    return 0;
  }

  QFileInfo info(argv[1]);
  if (!info.exists()) {
    qDebug() << "The specified file doesn't exist: " << info.fileName();
    return 0;
  }

  std::ifstream file(filename);
  if ((file.rdstate() & std::ifstream::failbit) != 0) {
    qDebug() << "could not open file.";
    return 0;
  }

  MetaConfiguration conf = parse_configuration(file);

  std::string outfile = std::filesystem::path(filename).filename().string();

  size_t substrSize = outfile.find_last_of('.');
  std::string name_without_ext = outfile.substr(0, substrSize);

  dump_header(conf, name_without_ext + ".h");
  dump_source(conf, name_without_ext + ".cpp");

  qDebug() << "files generated" << std::filesystem::absolute(name_without_ext + ".h").string();
  qDebug() << "files generated" << std::filesystem::absolute(name_without_ext + ".cpp").string();
}
