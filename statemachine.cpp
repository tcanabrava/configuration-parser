/* State Machine to handle the configuration file. */
#include "statemachine.h"
#include <iostream>
#include <memory>
#include <boost/algorithm/string.hpp>
#include "string-helpers.h"

std::shared_ptr<MetaProperty> current_property;
std::shared_ptr<MetaClass> current_class;

std::string global_string;
std::string last_comment;
bool should_be_array;
std::string array_value;
std::vector<std::string> includes;
std::shared_ptr<MetaClass> top_level_class = nullptr;

/* reads #include directives. */
callback_t state_include(std::ifstream& f, int& error) {
    char include_name[80];
    f.ignore(256, '<');
    f.getline(include_name, 80, '>'); // read untill >
    includes.push_back(include_name);
    std::cout << "include added: " << include_name << std::endl;
    return initial_state;
}

callback_t multi_purpose_string_state(std::ifstream& f, int& error) {
    std::vector<char> delimiters = {'{', '[', '=', ' ', '\n' };
    while(std::find(delimiters.begin(), delimiters.end(), f.peek()) == delimiters.end()) {
        global_string += f.get();
    }

    std::cout << "stirng found: " << global_string << " next character: " << (char) f.peek() << std::endl;
    return current_property ? nullptr // create a state for them.
        :  current_class ? class_state
        :  initial_state;
}

callback_t guess_documentation_state(std::ifstream& f, int& error) {
        f.ignore();
        char c = f.peek();
        switch(c) {
            case '*' : return multi_line_documentation_state;
            case '/' : return single_line_documentation_state;
        }
        return nullptr;
}

callback_t multi_line_documentation_state(std::ifstream& f, int& error) {
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

callback_t single_line_documentation_state(std::ifstream& f, int& error) {
    return nullptr;
}

callback_t begin_class_state(std::ifstream& f, int& error) {
    std::cout << "Starting class: " << global_string << std::endl;

    f.ignore(); // eat the '{' character.
    clear_empty(f);
    boost::trim(global_string);

    if (!top_level_class) {
        top_level_class = std::make_shared<MetaClass>();
        top_level_class->parent = nullptr;
        current_class = top_level_class;
    } else {
        auto old_parent = current_class;
        current_class->subclasses.push_back(std::make_shared<MetaClass>());
        current_class = current_class->subclasses.back();
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

callback_t end_class_state(std::ifstream& f, int& error) {
    f.ignore();
    std::cout << "finishing class " << current_class->name << std::endl;
    current_class = current_class->parent;
    if (current_class)
        return class_state;
    return nullptr;
}

callback_t begin_property_state(std::ifstream& f, int& error)
{
    // here we know that we have at least the type of the property,
    // but it can be three kinds of string.

    std::string property_name;
    clear_empty(f);

    current_property = std::make_shared<MetaProperty>();
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
    return property_state;
}


callback_t begin_property_set_state(std::ifstream& f, int& error) {
    std::string name;
    std::string value;
    std::string tmp;
    f.ignore(); // ignoring {

    do {
        f >> name;
        if (name == "}")
            break;

        f >> tmp;
        if (tmp == "}")
            break;

        f >> value;
        if (value == "}")
            break;

        if (name == "value") {
            std::cout << "found value for property  "  << value;
            current_property->default_value = value;
        } else {
            std::cout << "fount setter " << name << " with value " << value;
            current_property->setters.insert(std::make_pair(name,value));
        }
        std::cout << std::endl;
    } while(true);
    return class_state;
}

callback_t property_state(std::ifstream& f, int& error) {
    while(f.peek() != '\n' && f.peek() != '=' && f.peek() != '[') {
        f.ignore();
    }

    if (f.peek() == '[')
        return begin_array_state;

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
}

callback_t begin_array_state(std::ifstream& f, int& error) {
    f.ignore();
    should_be_array = true;
    return array_state;
}

callback_t array_state(std::ifstream& f, int& error) {
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

callback_t end_array_state(std::ifstream& f, int& error) {
    f.ignore();
    if (current_property) {
        return property_state;
    } else if (current_class) {
        return class_state;
    }
    return nullptr;
}

/* Start the default stuff -- classes,  includes and documentation. */
callback_t initial_state(std::ifstream& f, int& error) {
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
callback_t class_state(std::ifstream& f, int& error) {
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
