#pragma once

#include "meta-settings.h"

/* handles the beginning of the parsing, #includes, comments and classes. */
callback_t initial_state(std::ifstream& f);

/* handles the definition of new classes */
callback_t begin_class_state(std::ifstream& f);

/* deals with the internals of a new class */
callback_t class_state(std::ifstream& f);

/* finishes the current class */
callback_t end_class_state(std::ifstream& f);

/* handles the creation of properties for the current class */
callback_t begin_property_state(std::ifstream& f);

/* finishes the current property */
callback_t end_property_state(std::ifstream& f);

/* marks the current object (which can be a property or a class) as an array. */
callback_t begin_array_state(std::ifstream& f);

/* finishes the array. */
callback_t end_array_state(std::ifstream& f);

/* starts a list of possible property values. */
callback_t begin_property_set_state(std::ifstream& f);

/* handles the definition of a new set value for the property. */
callback_t property_set_value_state(std::ifstream& f);

/* finishes the handling of the property. */
callback_t end_property_set_state(std::ifstream& f);

/* reads a include string. */
callback_t state_include(std::ifstream& f);

/* reads a documentation line */
callback_t single_line_documentation_state(std::ifstream& f);

/*reads a documentation block */
callback_t multi_line_documentation_state(std::ifstream& f);

/* reads a string that we don't know what will do with it yet, maybe it's a class, a property, a type.*/
callback_t multi_purpose_string_state(std::ifstream& f);

callback_t guess_documentation_state(std::ifstream& f);
