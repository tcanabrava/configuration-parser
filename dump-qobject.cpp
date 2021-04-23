#include "dump-settings.h"
#include "dump_common.h"
#include "meta-settings.h"
#include "string-helpers.h"
#include <boost/filesystem.hpp>
#include <cassert>

Q_LOGGING_CATEGORY(dumpSource, "dumpSource")
Q_LOGGING_CATEGORY(dumpHeader, "dumpHeader")

void dump_source_class(MetaClass *top, std::ofstream &file) {
  // Constructors.
  file << top->name << "::" << top->name
       << "(QObject *parent) : QObject(parent)";
  for (auto &&p : top->properties) {
    if (p->default_value.size()) {
      file << ',' << std::endl
           << "\t_" << p->name << '(' << p->default_value << ')';
    }
  }
  file << std::endl;
  file << '{' << std::endl;
  file << '}' << std::endl;
  file << std::endl;

  dump_source_get_methods(file, top);
  dump_source_set_methods(file, top);
}

void dump_header_class(MetaClass *top, std::ofstream &file) {
  qCDebug(dumpHeader) << "Dumping class header";

  file << "class " << top->name << " : public QObject {" << std::endl;
  file << "Q_OBJECT" << std::endl;

  // Q_PROPERTY declarations
  qCDebug(dumpHeader) << "Class has:" << top->properties.size()
                      << "properties.";
  dump_header_q_properties(file, top->properties);

  file << std::endl;
  file << "public:" << std::endl;
  file << "\t" << top->name << "(QObject *parent = 0);" << std::endl;

  dump_header_properties(file, top->properties);
  file << "};" << std::endl << std::endl;
}

void dump_header(const MetaConfiguration &conf, const std::string &filename) {
  qCDebug(dumpHeader) << "Starting to dump the source file into" << filename;

  std::ofstream header(filename);
  begin_header_guards(header, filename);
  header << std::endl;

  dump_notice(header);

  header << "#include <QObject>" << std::endl;

  char delimiter_begin;
  char delimiter_end;
  for (auto include : conf.includes) {
    if(include.is_global) {
	    delimiter_begin = '<';
	    delimiter_end = '>';
    }
    else delimiter_begin, delimiter_end = '"';

    header << "#include " << delimiter_begin << include.name << delimiter_end << std::endl;
  }

  if (conf.includes.size()) {
    header << std::endl;
  }

  if (conf.conf_namespace.size()) {
    header << "namespace " << conf.conf_namespace << " {" << std::endl;
  }

  if (conf.includes.size()) {
    header << std::endl;
  }

  if (conf.top_level_class) {
    dump_header_class(conf.top_level_class.get(), header);
  }

  if (conf.conf_namespace.size()) {
    header << "}" << std::endl;
  }
}

void dump_source(const MetaConfiguration &conf, const std::string &filename) {
  boost::filesystem::path path(filename);
  std::ofstream source(path.filename().generic_string());

  dump_notice(source);

  source << "#include \"" << path.stem().generic_string() << ".h\""
         << std::endl;
  source << "#include <QSettings>" << std::endl;
  source << std::endl;

  if (conf.conf_namespace.size()) {
    source << "namespace " << conf.conf_namespace << " {" << std::endl
           << std::endl;
  }

  if (conf.top_level_class) {
    dump_source_class(conf.top_level_class.get(), source);
  }

  if (conf.conf_namespace.size()) {
    source << "}" << std::endl;
  }
}
