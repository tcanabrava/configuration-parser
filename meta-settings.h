#pragma once

#include <string>
#include <functional>
#include <fstream>
#include <vector>
#include <memory>
#include <map>
#include <iostream>

struct MetaClass;
struct MetaProperty;

struct MetaProperty {
    typedef std::shared_ptr<MetaProperty> Ptr;
    std::shared_ptr<MetaClass> parent;
    std::string name;
    std::string default_value;
    std::string type;
    std::map<std::string, std::string> setters;
};

struct MetaClass {
    typedef std::shared_ptr<MetaClass> Ptr;
    std::vector<MetaProperty::Ptr> properties;
    std::vector<Ptr> subclasses;
    std::shared_ptr<MetaClass> parent;
    std::string name;
    bool is_array;
};

extern std::vector<std::string> includes;
extern std::shared_ptr<MetaClass> top_level_class;
