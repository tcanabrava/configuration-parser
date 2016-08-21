#include <iostream>
#include <fstream>
#include <functional>
#include <map>
#include <vector>
#include <string>
#include <memory>

struct MetaProperty {
    std::string name;
    std::string default_value;
    std::string type;
};

struct MetaClass {
        std::vector<std::unique_ptr<MetaProperty>> properties;
        std::vector<std::unique_ptr<MetaClass>> subclasses;
        std::string name;
        MetaClass *parent;
};

template< typename... T > struct RecursiveHelper {
    typedef std::function< RecursiveHelper(T...) > type;
    RecursiveHelper( type f ) : func(f) {}
    operator type () { return func; }
    type func;
};

typedef RecursiveHelper<std::ifstream&>::type callback_t;

bool global_debug = false;
std::string global_string;
std::vector<std::string> includes;
std::unique_ptr<MetaClass> top_level_class = nullptr;
MetaClass *current_class = nullptr;

callback_t guess_state(std::ifstream& f);
void clear_empty(std::ifstream& f);
callback_t guess_class_state(std::ifstream& f);

std::string camel_case_to_underscore(std::string s) {
    std::string ret;
    for(char x : s){
        if (x >= 'A' && x <= 'Z') {
            ret += '_';
            ret += (char) (x | 32);
        } else {
            ret += x;
        }
    }
    return ret;
}

std::string underscore_to_camel_case(std::string s) {
    std::string ret;
    for(int i = 0, end = s.size(); i < end; ++i) {
        if (s[i] == '_' && i != end) {
            s[i+1] ^= 32;
            continue;
        }
        ret += s[i];
    }
    return ret;
}

std::string capitalize(std::string s, int pos) {
    s[pos] ^= 32;
    return s;
}
callback_t state_include(std::ifstream& f) {
    char include_name[80];
    f.ignore(256, '<');
    f.getline(include_name, 80, '>'); // read untill >
    includes.push_back(include_name);
    if (global_debug) std::cout << "include added: " << include_name << std::endl;
    return guess_state;
}

void clear_empty(std::ifstream& f) {
    while(f.peek() == ' '  || f.peek() == '\n'){
        f.ignore();
    }
}

callback_t state_class(std::ifstream& f) {
    if (global_debug) std::cout << "Starting class: " << global_string << std::endl;
    f.ignore();
    if (!top_level_class) {
        top_level_class = std::make_unique<MetaClass>();
        current_class = top_level_class.get();
        current_class->parent = nullptr;
    } else {
        MetaClass *old_parent = current_class;
        current_class->subclasses.push_back(std::make_unique<MetaClass>());
        current_class = current_class->subclasses.back().get();
        current_class->parent = old_parent;
    }
    current_class->name  = global_string;
    global_string.clear();
    return guess_class_state;;
}

callback_t state_string(std::ifstream& f) {
    f >> global_string;
//    if (global_debug) std::cout << "string added: " << global_string << std::endl;
    return guess_state;
}

callback_t state_end_class(std::ifstream& f) {
    f.ignore();
    if (global_debug) std::cout << "class finished: " << current_class->name << std::endl;
    if (current_class->parent) {
        current_class = current_class->parent;
        return guess_class_state;
    }

    return guess_state;
}

callback_t state_property(std::ifstream& f) {
    std::string property_type = global_string;
    std::string property_name;
    f >> property_name;
    std::cout << "Property type:  " << property_type << ", name " << property_name << std::endl;
    clear_empty(f);
    char c = f.peek();

    current_class->properties.push_back(std::make_unique<MetaProperty>());
    auto property = current_class->properties.back().get();

    property->name = property_name;
    property->type = property_type;
    if (c == '=') {
         std::string default_value;
         f.ignore();
         clear_empty(f);
         c = f.peek();
         //TODO: extract this to a extract string function.
         // If it's a string.
         if (c == '"') {
             char value[256];
            f.ignore(256, '"');
            f.getline(value, 256, '"');
            default_value += '"';
            default_value += value;
            default_value += "\"\0";
        } else {
            f >> default_value;
        }
        std::cout << "Reading default value =" << default_value << std::endl;
        property->default_value = default_value;
        return guess_class_state;
    } else {
        return guess_class_state;
    }
}

callback_t state_class_string(std::ifstream& f) {
        f >> global_string;
        clear_empty(f);

        if (f.peek() == '{') {
            return state_class;
        } else {
            return state_property;
        }
}

callback_t guess_class_state(std::ifstream& f) {
    char c = f.peek();
    switch(c) {
        case ' ' :
        case '\n' :
            clear_empty(f);
            return guess_class_state;
        case '}' : return state_end_class;
        case '{' : return state_class;
        default : return state_class_string;
    }
    return nullptr;
}

callback_t guess_state(std::ifstream& f) {
    char c = f.peek();
    switch(c) {
        case '#' : return state_include;
        case '{' : return state_class;
        case '}' : return state_end_class;
        case EOF : return nullptr;
        case ' ' :
        case '\n' :
            clear_empty(f);
            return guess_state;
        default : return state_string;
    }
    return nullptr;
}

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
    }
}

void dump_class_header(MetaClass *top, std::ofstream& file) {
    file << "#include <QObject>" << std::endl;

    for(auto&& child : top->subclasses) {
        dump_class_header(child.get(), file);
    }

    file << "class " << top->name << " : public QObject {"  << std::endl;
    file << "Q_OBJECT" << std::endl;

    // Q_PROPERTY declarations
    for(auto&& p : top->properties) {
        file << "Q_PROPERTY(" << p->type << " " << camel_case_to_underscore(p->name) << " READ " << p->name << " WRITE set" << capitalize(p->name, 0) << " NOTIFY " << p->name << "Changed)" << std::endl;
    }
    for(auto&& child : top->subclasses) {
        file <<"Q_PROPERTY(QObject* " << camel_case_to_underscore(child->name) << " MEMBER _" << child->name << " CONSTANT);" << std::endl;
    }

    file << std::endl;
    file << "public:" << std::endl;
    file <<"\t" << top->name <<"(QObject *parent);" << std::endl;

    for(auto&& child : top->subclasses) {
        file  << "\t" << child->name << " *_" << child->name << ";" << std::endl;
    }

    if (top->properties.size()) {
        for(auto&& p : top->properties) {
            file << "\t" << p->type << " " << p->name << "() const;" << std::endl;
            file << "\tvoid set" << capitalize(p->name,0) << "Rule(std::function<bool(" << p->type << ")> rule);" << std::endl;
        }

        file << std::endl;
        file <<"public slots:" <<std::endl;
        for(auto&& p : top->properties) {
            file << "\tvoid set" << capitalize(p->name,0) << "(" << p->type <<" value);" << std::endl;
        }

        file <<"signals:" <<std::endl;
        for(auto&& p : top->properties) {
            file << "\tvoid " << p->name << "Changed(" << p->type <<" value);" << std::endl;
        }

        file <<"private:" <<std::endl;
        for(auto&& p : top->properties) {
            file << "\t" << p->type << " _" << p->name <<";" << std::endl;
            file << "\tstd::function<bool(" << p->type << ")> " << p->name << "Rule;" << std::endl;
        }
    }

    if (!top->parent) {
        file << "\tvoid sync();" <<std::endl;
        file << "\tvoid load();" <<std::endl;
        file << "\tstatic " << top->name << "* self();" << std::endl;
    }
    file << "};" <<std::endl << std::endl;
}

//TODO: remember to fix the filenames.
void dump_header() {
    std::ofstream header("test.h");
    header << "#pragma once" << std::endl;
    header << "#include <functional>" << std::endl;

    header << std::endl;

    for(auto include : includes) {
        header << "#include <" << include << ">" << std::endl;
    }
    header << std::endl;
    dump_class_header(current_class, header);
}

void dump_source() {
    std::ofstream source("test.cpp");
    source << "#include <test.h>" << std::endl;
    source << "#include <QSettings>" << std::endl;
    source << std::endl;
    dump_class_source(current_class, source);
}

void show_usage() {
    std::cout << "usage: qobject-compiler if=file.conf of=file.cpp" << std::endl;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        show_usage();
        return 0;
    }

    if (argc == 3)
        global_debug = true;

    callback_t state = guess_state;
    std::cout << "file to read: " << argv[1] << std::endl;

    std::ifstream file("test.conf");
    if ( (file.rdstate() & std::ifstream::failbit) != 0) {
        std::cerr << "could not open file." << std::endl;
    }

    // Start the state machine
    while( state ) {
        state = state(file);
    }
    dump_header();
    dump_source();
}
