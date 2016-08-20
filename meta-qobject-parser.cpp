#include <iostream>
#include <fstream>
#include <functional>
#include <map>
#include <vector>
#include <string>

typedef std::string property;
typedef std::string type;
typedef std::string setter;

struct MetaClass {
        std::vector<std::pair<property, type>> properties;
        std::map<property, setter> setters;
        std::vector<MetaClass*> subgroups;
        std::string className;
};

template< typename... T > struct RecursiveHelper {
    typedef std::function< RecursiveHelper(T...) > type;
    RecursiveHelper( type f ) : func(f) {}
    operator type () { return func; }
    type func;
};

typedef RecursiveHelper<std::ifstream&>::type callback_t;

bool global_debug = false;
std::string temporary;
std::vector<std::string> includes;

callback_t guess_state(std::ifstream& f);
callback_t state_empty(std::ifstream& f);

callback_t state_include(std::ifstream& f) {
    char include_name[80];
    f.ignore(256, '<');
    f.getline(include_name, 80, '>'); // read untill >
    includes.push_back(include_name);
    if (global_debug) std::cout << "include added: " << include_name << std::endl;
    return guess_state;
}

callback_t state_empty(std::ifstream& f) {
    while(f.peek() == ' '  || f.peek() == '\n'){
        f.ignore();
    }
    return guess_state;
}

callback_t state_class(std::ifstream& f) {
    return nullptr;
}

callback_t state_string(std::ifstream& f) {
    return nullptr;
}

callback_t guess_state(std::ifstream& f) {
    char c = f.peek();
    switch(c) {
        case '#' : return state_include;
        case ' ' :
        case '\n' : return state_empty;
        case '{' : return state_class;
        case EOF : return nullptr;
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
