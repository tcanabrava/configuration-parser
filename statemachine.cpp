#include "statemachine.h"

#include <iostream>


std::string global_string;
std::vector<std::string> includes;
std::unique_ptr<MetaClass> top_level_class = nullptr;
MetaClass *current_class = nullptr;

callback_t state_class(std::ifstream& f) {
    std::cout << "Starting class: " << global_string << std::endl;
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
//    std::cout << "string added: " << global_string << std::endl;
    return guess_state;
}

callback_t state_end_class(std::ifstream& f) {
    f.ignore();
    std::cout << "class finished: " << current_class->name << std::endl;
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

callback_t state_include(std::ifstream& f) {
    char include_name[80];
    f.ignore(256, '<');
    f.getline(include_name, 80, '>'); // read untill >
    includes.push_back(include_name);
    std::cout << "include added: " << include_name << std::endl;
    return guess_state;
}
