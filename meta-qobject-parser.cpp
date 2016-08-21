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

void dump_class_header(MetaClass *top, std::ofstream& file) {
    for(auto&& child : top->subclasses) {
        dump_class_header(child.get(), file);
    }

    file << "class " << top->name << " : public QObject {"  << std::endl;
    file << "Q_OBJECT" << std::endl;

    // Q_PROPERTY declarations
    for(auto&& p : top->properties) {
        file << "Q_PROPERTY(" << p->type << " " << p->name << " READ " << p->name << " WRITE set" << p->name << " NOTIFY " << p->name << "Changed)" << std::endl;
    }
    for(auto&& child : top->subclasses) {
        file <<"Q_PROPERTY(QObject* " << child->name << " MEMBER _" << child->name << " CONSTANT);" << std::endl;
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
        }

        file << std::endl;
        file <<"public slots:" <<std::endl;
        for(auto&& p : top->properties) {
            file << "\tvoid set" << p->name << "(" << p->type <<" value);" << std::endl;
        }

        file <<"signals:" <<std::endl;
        for(auto&& p : top->properties) {
            file << "\tvoid " << p->name << "Changed(" << p->type <<" value);" << std::endl;
        }

        file <<"private:" <<std::endl;
        for(auto&& p : top->properties) {
            file << "\t" << p->type << " _" << p->name <<";" << std::endl;
        }
    }
    file << "};" <<std::endl << std::endl;
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

    // the class-tree is ready.
    std::ofstream header("test.h");
    header << "#pragma once" << std::endl;
    header << std::endl;

    for(auto include : includes) {
        header << "#include <" << include << ">" << std::endl;
    }
    header << std::endl;

    dump_class_header(current_class, header);
    header.close();
}
