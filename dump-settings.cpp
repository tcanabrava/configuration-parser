#include "dump-settings.h"
#include "meta-settings.h"
#include "string-helpers.h"
#include <cassert>

Q_LOGGING_CATEGORY(dumpSource, "dumpSource")
Q_LOGGING_CATEGORY(dumpHeader, "dumpHeader")

void dump_class_settings(MetaClass *top, std::ofstream& file) {
    static std::string tabs;

    if (top->parent) {
        file << tabs << "s.beginGroup(\"" << top->name << "\");" << std::endl;
    }

    tabs += '\t';
    for(auto&& s : top->subclasses) {
        dump_class_settings(s.get(), file);
    }
    tabs.erase(0,1);

    tabs += '\t';
    for(auto&& p : top->properties) {
        std::string callchain = '_' + top->name;
        file << tabs << "s.setValue(\"" << camel_case_to_underscore(p->name) << "\",";
        auto tmp = top->parent;
        if (tmp && tmp->parent) {
            while(tmp->parent ) {
                std::string s = '_' + tmp->name + "->";
                callchain.insert(0,s);
                tmp = tmp->parent;
            }
        }
        file << callchain << "->" << p->name << "());" << std::endl;
    }
    tabs.erase(0,1);
    if (top->parent) {
        file << tabs << "s.endGroup();" << std::endl;
    }
}

void load_class_settings(MetaClass *top, std::ofstream& file) {
    static std::string tabs;

    if (top->parent) {
        file << tabs << "s.beginGroup(\"" << top->name << "\");" << std::endl;
    }

    tabs += '\t';
    for(auto&& s : top->subclasses) {
        load_class_settings(s.get(), file);
    }
    tabs.erase(0,1);

    tabs += '\t';
    for(auto&& p : top->properties) {
        std::string callchain = '_' + top->name;
        auto tmp = top->parent;
        if (tmp && tmp->parent) {
            while(tmp->parent ) {
                std::string s = '_' + tmp->name + "->";
                callchain.insert(0,s);
                tmp = tmp->parent;
            }
        }
        file << tabs << callchain << "->set" << capitalize(p->name,0)
              << "(s.value(\"" << camel_case_to_underscore(p->name) << "\").value<" << p->type << ">());" << std::endl;
    }
    tabs.erase(0,1);
    if (top->parent) {
        file << tabs << "s.endGroup();" << std::endl;
    }
}

void dump_class_source(MetaClass *top, std::ofstream& file) {
    for(auto&& child : top->subclasses) {
        dump_class_source(child.get(), file);
    }

    //Constructors.
    file << top->name << "::" << top->name << "(QObject *parent) : QObject(parent)";
    for(auto&& c : top->subclasses) {
        file << ',' << std::endl << "\t_" << c->name << "(new " << c->name << "(this))";
    }
    for(auto&& p : top->properties) {
        if (p->default_value.size()) {
            file << ',' << std::endl << "\t_" << p->name << '(' << p->default_value << ')';
        }
    }
    file << '{' << std::endl;
    file << '}' << std::endl;

    //get - methods.
    for(auto&& p : top->properties) {
        file << p->type << ' ' << top->name << "::" << p->name << "() const" << std::endl;
        file << '{' << std::endl;
        file << "\treturn _" << p->name << ';' << std::endl;
        file << '}' << std::endl;
        file << std::endl;
    }

    //set-methods
    for(auto&& p : top->properties) {
        file << "void " << top->name << "::set" << capitalize(p->name,0) << '(' << p->type << " value)" << std::endl;
        file << '{' << std::endl;
        file << "\tif(" << p->name << "Rule && !" << p->name << "Rule(value))" << std::endl;
        file << "\t\treturn;" << std::endl;
        file << "\t _" << p->name << " = value;" << std::endl;
        file << "\temit " << p->name << "Changed(value);" << std::endl;
        file << '}' << std::endl;
        file << std::endl;
    }

    //rule-methods {
    for(auto&& p : top->properties) {
        file << "void " << top->name << "::set" << capitalize(p->name,0) << "Rule(std::function<bool(" << p->type << ")> rule)" << std::endl;
        file << '{' << std::endl;
        file << "\t" << p->name << "Rule = rule;" << std::endl;
        file << '}' << std::endl;
        file << std::endl;
    }

    // main preferences class
    if (!top->parent) {
        file << "void " << top->name << "::sync()" << std::endl;
        file << '{' << std::endl;
        file << "\tQSettings s;" << std::endl;

        dump_class_settings(top, file);
        file << '}' << std::endl;

        file << "void " << top->name << "::load()" << std::endl;
        file << '{' << std::endl;
        file << "\tQSettings s;" << std::endl;

        load_class_settings(top, file);
        file << '}' << std::endl;
        file << top->name << "* " << top->name <<"::self()" << std::endl;
        file << "{" << std::endl;
        file << "\tstatic " << top->name << " s;" << std::endl;
        file << "\treturn &s;" <<std::endl;
        file << "}" << std::endl;
    }
}

void dump_header_properties(std::ofstream& file,const  std::vector<std::shared_ptr<MetaProperty>>& properties, bool& has_private){
    if (!properties.size())
        return;

    if (!has_private)
        file << "private:" << std::endl;

    for(auto&& p : properties) {
        file << "\t" << p->type << " " << p->name << "() const;" << std::endl;
        file << "\tvoid set" << capitalize(p->name,0) << "Rule(std::function<bool(" << p->type << ")> rule);" << std::endl;
    }

    file << std::endl << "public slots:" << std::endl;
    for(auto&& p : properties)
        file << "\tvoid set" << capitalize(p->name,0) << "(" << p->type <<" value);" << std::endl;

    file <<std::endl << "signals:" << std::endl;
    for(auto&& p : properties)
        file << "\tvoid " << p->name << "Changed(" << p->type <<" value);" << std::endl;

    file << std::endl << "private:" << std::endl;
    for(auto&& p : properties) {
        file << "\t" << p->type << " _" << p->name <<";" << std::endl;
        file << "\tstd::function<bool(" << p->type << ")> " << p->name << "Rule;" << std::endl;
    }
}

void dump_header_subclasses(std::ofstream& file, const std::vector<std::shared_ptr<MetaClass>>& subclasses, bool& has_private) {
    if (!subclasses.size())
        return;

    if (!has_private)
        file << "private:" << std::endl;

    for(auto&& child : subclasses)
        file  << "\t" << child->name << " *_" << decapitalize(child->name, 0) << ";" << std::endl;
}

void dump_class_header(MetaClass *top, std::ofstream& file) {
    qCDebug(dumpHeader) << "Dumping class header";
    for(auto&& child : top->subclasses) {
        dump_class_header(child.get(), file);
    }

    file << "class " << top->name << " : public QObject {"  << std::endl;
    file << "Q_OBJECT" << std::endl;

    // Q_PROPERTY declarations
    qCDebug(dumpHeader) << "Class has:"  << top->properties.size() <<"properties.";
    for(auto&& p : top->properties) {
        file << "Q_PROPERTY(" << p->type << " "
             << camel_case_to_underscore(p->name)
             << " READ " << p->name
             << " WRITE set" << capitalize(p->name, 0)
             << " NOTIFY " << p->name << "Changed)"
             << std::endl;
    }

    for(auto&& child : top->subclasses) {
        file <<"Q_PROPERTY(QObject* "
             << camel_case_to_underscore(child->name)
             << " MEMBER _" << decapitalize(child->name, 0)
             << " CONSTANT);"
             << std::endl;
    }

    file << std::endl;
    file << "public:" << std::endl;
    if (top->parent) {
        file <<"\t" << top->name <<"(QObject *parent = 0);" << std::endl;
    } else  {
        file << "\tvoid sync();" <<std::endl;
        file << "\tvoid load();" <<std::endl;
        file << "\tstatic " << top->name << "* self();" << std::endl;
    }

    for(auto&& child : top->subclasses) {
        file  << "\t" << child->name << " *" << decapitalize(child->name, 0) << "();" << std::endl;
    }

    bool has_private = false;
    dump_header_properties(file, top->properties, has_private);
    dump_header_subclasses(file, top->subclasses, has_private);

    if (!top->parent) {
        if (!has_private) {
            file << std::endl << "private:" << std::endl;
            has_private = true;
        }
        file <<"\t" << top->name <<"(QObject *parent = 0);" << std::endl;
    }
    file << "};" <<std::endl << std::endl;
}

void dump_header(const std::string& filename) {
    qCDebug(dumpHeader) << "Starting to dump the source file into" << filename;

    std::ofstream header(filename);
    header << "#pragma once" << std::endl;
    header << std::endl;
    header << "#include <functional>" << std::endl;
    header << "#include <QObject>" << std::endl;

    header << std::endl;

    for(auto include : includes) {
        header << "#include <" << include << ">" << std::endl;
    }
    if (includes.size()) {
        header << std::endl;
    }
    if (top_level_class) {
        dump_class_header(top_level_class.get(), header);
    }
}

void dump_source(const std::string& filename) {
    std::ofstream source(filename);
    source << "#include <test.h>" << std::endl;
    source << "#include <QSettings>" << std::endl;
    source << std::endl;
    // something based on dump_header here.
}
