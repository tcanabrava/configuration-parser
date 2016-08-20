#include <iostream>
#include <fstream>
#include <functional>
#include <map>
#include <vector>
#include <string>
#include <memory>

typedef std::string property;
typedef std::string type;
typedef std::string setter;

struct MetaProperty {
    std::string name;
    std::string default_value;
    std::string type;
    std::string setter;
};

struct MetaClass {
        std::vector<std::unique_ptr<MetaProperty>> properties;
        std::vector<std::unique_ptr<MetaClass>> subgroups;
        std::string className;
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
        current_class->subgroups.push_back(std::make_unique<MetaClass>());
        current_class = current_class->subgroups.back().get();
        current_class->parent = old_parent;
    }
    current_class->className  = global_string;
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
    if (global_debug) std::cout << "class finished: " << current_class->className << std::endl;
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
    if (f.peek() == ':') {
        std::cout << "starting to add complex properties" << std::endl;
        return nullptr;
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
}
