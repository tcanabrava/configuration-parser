#include "dump-settings.h"
#include "meta-settings.h"
#include "string-helpers.h"
#include <boost/filesystem.hpp>
#include <cassert>

Q_LOGGING_CATEGORY(dumpSource, "dumpSource")
Q_LOGGING_CATEGORY(dumpHeader, "dumpHeader")

void dump_source_class_settings_set_values(MetaClass *top,
                                           std::ofstream &file) {
  static std::string tabs;

  tabs += '\t';
  for (auto &&p : top->properties) {
    std::string callchain;
    file << tabs << "s.setValue(\"" << camel_case_to_underscore(p->name)
         << "\",";
    auto tmp = p->parent;
    if (tmp && tmp->parent) {
      while (tmp->parent) {
        std::string s = decapitalize(tmp->name, 0) + "()->";
        callchain.insert(0, s);
        tmp = tmp->parent;
      }
    }
    if (p->is_enum) {
      file << "(int) ";
    }
    if (callchain.size())
      file << callchain;
    file << p->name << "());" << std::endl;
  }
  tabs.erase(0, 1);
  if (top->parent) {
    file << tabs << "s.endGroup();" << std::endl;
  }
}

void dump_source_class_settings_get_values(MetaClass *top,
                                           std::ofstream &file) {
  static std::string tabs;

  if (top->parent) {
    file << tabs << "s.beginGroup(\"" << top->name << "\");" << std::endl;
  }

  tabs += '\t';
  for (auto &&s : top->subclasses) {
    dump_source_class_settings_get_values(s.get(), file);
  }
  tabs.erase(0, 1);

  tabs += '\t';
  for (auto &&p : top->properties) {
    std::string callchain;
    auto tmp = p->parent;
    if (tmp && tmp->parent) {
      while (tmp->parent) {
        std::string s = decapitalize(tmp->name, 0) + "()->";
        callchain.insert(0, s);
        tmp = tmp->parent;
      }
    }
    file << tabs << callchain;
    std::string type = p->is_enum ? "int" : p->type;
    file << "set" << capitalize(p->name, 0) << "(";

    if (p->is_enum)
      file << "(" << p->type << ")";

    file << "s.value(\"" << camel_case_to_underscore(p->name) << "\").value<"
         << p->type << ">());" << std::endl;
  }
  tabs.erase(0, 1);
  if (top->parent) {
    file << tabs << "s.endGroup();" << std::endl;
  }
}

bool class_or_subclass_have_properties(MetaClass *top) {
  if (!top)
    return false;
  if (top->properties.size() != 0) {
    return true;
  }

  for (auto child : top->subclasses) {
    if (class_or_subclass_have_properties(child.get())) {
      return true;
    }
  }
  return false;
}

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

  file << '{' << std::endl;
  file << '}' << std::endl;
  file << std::endl;

  // get - methods.
  for (auto &&p : top->properties) {
    file << p->type << ' ' << top->name << "::" << p->name << "() const"
         << std::endl;
    file << '{' << std::endl;
    file << "\treturn _" << p->name << ';' << std::endl;
    file << '}' << std::endl;
    file << std::endl;
  }

  // set-methods
  for (auto &&p : top->properties) {
    file << "void " << top->name << "::set" << capitalize(p->name, 0) << '('
         << p->type << " value)" << std::endl;
    file << '{' << std::endl;
    file << "\tif(_" << p->name << "=="
         << "value)" << std::endl;
    file << "\t\treturn;" << std::endl;
    file << "\t _" << p->name << " = value;" << std::endl;
    file << "\temit " << p->name << "Changed(value);" << std::endl;
    file << '}' << std::endl;
    file << std::endl;
  }

  // main preferences class
}

void dump_header_properties(
    std::ofstream &file,
    const std::vector<std::shared_ptr<MetaProperty>> &properties) {
  if (!properties.size())
    return;

  for (auto &&p : properties) {
    file << "\t" << p->type << " " << p->name << "() const;" << std::endl;
  }

  file << std::endl << "public slots:" << std::endl;
  for (auto &&p : properties)
    file << "\tvoid set" << capitalize(p->name, 0) << "(" << p->type
         << " value);" << std::endl;

  file << std::endl << "signals:" << std::endl;
  for (auto &&p : properties)
    file << "\tvoid " << p->name << "Changed(" << p->type << " value);"
         << std::endl;

  file << std::endl << "private:" << std::endl;
  for (auto &&p : properties) {
    file << "\t" << p->type << " _" << p->name << ";" << std::endl;
  }
}

void dump_header_class(MetaClass *top, std::ofstream &file) {
  qCDebug(dumpHeader) << "Dumping class header";

  file << "class " << top->name << " : public QObject {" << std::endl;
  file << "Q_OBJECT" << std::endl;

  // Q_PROPERTY declarations
  qCDebug(dumpHeader) << "Class has:" << top->properties.size()
                      << "properties.";
  for (auto &&p : top->properties) {
    file << "Q_PROPERTY(" << p->type << " " << camel_case_to_underscore(p->name)
         << " READ " << p->name << " WRITE set" << capitalize(p->name, 0)
         << " NOTIFY " << p->name << "Changed)" << std::endl;
  }

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
  header << "#include <QObject>" << std::endl;

  for (auto include : conf.includes) {
    header << "#include <" << include << ">" << std::endl;
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

  end_header_guards(header, filename);
}

void dump_source(const MetaConfiguration &conf, const std::string &filename) {
  boost::filesystem::path path(filename);
  std::ofstream source(path.filename().generic_string());
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
