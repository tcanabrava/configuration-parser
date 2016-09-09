#include "statemachine.h"

#include <iostream>

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



