/* State Machine to handle the configuration file. */
#include "statemachine.h"
#include <iostream>
#include <memory>
#include <boost/algorithm/string.hpp>
#include "string-helpers.h"

Meta *current_object;
MetaProperty *current_property;
MetaClass *current_class;

std::string global_string;
std::string last_comment;
bool should_be_array;
std::string array_value;
std::vector<std::string> includes;
std::unique_ptr<MetaClass> top_level_class = nullptr;


/* reads #include directives. */
callback_t state_include(std::ifstream& f) {
    char include_name[80];
    f.ignore(256, '<');
    f.getline(include_name, 80, '>'); // read untill >
    includes.push_back(include_name);
    std::cout << "include added: " << include_name << std::endl;
    return initial_state;
}

callback_t multi_purpose_string_state(std::ifstream& f) {
    std::vector<char> delimiters = {'{', '[', '=', ' ', '\n' };
    while(std::find(delimiters.begin(), delimiters.end(), f.peek()) == delimiters.end()) {
        global_string += f.get();
    }

    std::cout << "stirng found: " << global_string << " next character: " << (char) f.peek() << std::endl;
    return current_property ? nullptr // create a state for them.
        :  current_class ? class_state
        :  initial_state;
}

callback_t guess_documentation_state(std::ifstream& f) {
        f.ignore();
        char c = f.peek();
        switch(c) {
            case '*' : return multi_line_documentation_state;
            case '/' : return single_line_documentation_state;
        }
        return nullptr;
}

callback_t multi_line_documentation_state(std::ifstream& f) {
    std::string comment;
    char c;
    while(f.peek() != EOF) {
        c = f.get();
        if (c == '*' && f.peek() == '/') {
            goto exit;
        }
        comment += c;
    }
    last_comment = comment;

    exit:
    return current_property ? property_state
            : current_class ? class_state
            : initial_state;

}

callback_t single_line_documentation_state(std::ifstream& f) {
    return nullptr;
}

MetaClass *parent_class(Meta *object) {
        while(object != nullptr) {
            auto ptr = dynamic_cast<MetaClass*>(object);
            if (ptr)
                return ptr;
            object = object->parent;
        }
        return nullptr;
}

callback_t begin_class_state(std::ifstream& f) {
    std::cout << "Starting class: " << global_string << std::endl;

    f.ignore(); // eat the '{' character.
    clear_empty(f);
    boost::trim(global_string);

    if (!top_level_class) {
        top_level_class = std::make_unique<MetaClass>();
        top_level_class->parent = nullptr;
        current_class = top_level_class.get();
    } else {
        auto *old_parent = current_class;
        current_class->subclasses.push_back(std::make_unique<MetaClass>());
        current_class = current_class->subclasses.back().get();
        current_class->parent = old_parent;
    }
    current_class->name  = global_string;
    current_class->is_array = should_be_array;
    should_be_array = false;

    std::cout << "class found: " << current_class->name << ", is_array = " << current_class->is_array
              <<", value = " << array_value << std::endl;

    global_string.clear();
    return class_state;
}

callback_t end_class_state(std::ifstream& f) {
    f.ignore();
    std::cout << "finishing class " << current_class->name << std::endl;
    current_class = static_cast<MetaClass*>(current_class-> parent);
    if (current_class)
        return class_state;
    return nullptr;
}

callback_t begin_property_state(std::ifstream& f)
{
    // here we know that we have at least the type of the property,
    // but it can be three kinds of string.

    std::string property_name;
    clear_empty(f);

    current_property = new MetaProperty();
    current_property->type = global_string;
    current_property->parent = current_class;
    global_string.clear();

    // find the name of the property
    std::vector<char> delimiters = {'=', '\n' };
    while(std::find(delimiters.begin(), delimiters.end(), f.peek()) == delimiters.end()) {
        property_name += f.get();
    }
    current_property->name = property_name;
    std::cout << "Starting property " << property_name << " ";

    // find next userfull tocken:
    while(f.peek() != '\n' && f.peek() != '=') {
        f.ignore();
    }

    // easy, line finished, next property or class.
    if (f.peek() == '\n') {
        current_property = nullptr;
        std::cout << "finishing property" << std::endl;
        return class_state;
    } else if (f.peek() == '='){
        f.ignore();
        clear_empty(f);
        if (f.peek() == '{') {
            std::cout << "starting the set property set" << std::endl;
            return begin_property_set_state;
        } else {
            char buffer[80];
            f.getline(buffer,80, '\n');
            current_property->default_value = buffer;
            std::cout << "value = " << current_property->default_value << std::endl;
            current_property = nullptr;
            return class_state;
        }
    }
    std::cout << std::endl;


    return nullptr;
}

callback_t begin_property_set_state(std::ifstream& f) {

}

callback_t property_state(std::ifstream& f) {

}

callback_t begin_array_state(std::ifstream& f) {
    f.ignore();
    should_be_array = true;
    return array_state;
}

callback_t array_state(std::ifstream& f) {
        clear_empty(f);
        char c = f.peek();
        if (c >= '0' && c <= '9') {
            char value[10];
            f.getline(value, 10, ']');
            array_value = value;
            return end_array_state;
        } else if (c == ']') {
            return end_array_state;
        }
        //TODO: error handling.
        return nullptr;
}

callback_t end_array_state(std::ifstream& f) {
    f.ignore();
    //TODO: Property State Handling.
    //     if (current_property) {
    //         return property_state;
    //     }
    if (current_class) {
        return class_state;
    }
}

/* Start the default stuff -- classes,  includes and documentation. */
callback_t initial_state(std::ifstream& f) {
    clear_empty(f);

    char c = f.peek();
    std::cout << "Peeking" << c << std::endl;
    switch(c) {
        case '#' : return state_include;
        case '{' : return begin_class_state;
        case '[' : return begin_array_state;
        case '/' : return guess_documentation_state;
        case EOF : return nullptr;
        default :  return multi_purpose_string_state;
    }
    return nullptr;
}

/* starts the class stuff */
callback_t class_state(std::ifstream& f) {
    clear_empty(f);
    char c = f.peek();
    switch(c) {
        case '{' : return begin_class_state;
        case '}' : return end_class_state;
        case '[' : return begin_array_state;
        case '/' : return guess_documentation_state;
    }
    return global_string.size() ? begin_property_state : multi_purpose_string_state;
}
