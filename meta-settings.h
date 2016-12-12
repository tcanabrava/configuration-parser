#pragma once

#include <string>
#include <functional>
#include <fstream>
#include <vector>
#include <memory>
#include <map>

struct MetaClass;
struct MetaProperty;

struct MetaProperty {
    typedef std::unique_ptr<MetaProperty> Ptr;
    MetaClass *parent;
    std::string name;
    std::string default_value;
    std::string type;
    std::map<std::string, std::string> setters;
};

struct MetaClass {
    typedef std::unique_ptr<MetaClass> Ptr;
    std::vector<MetaProperty::Ptr> properties;
    std::vector<Ptr> subclasses;

    MetaClass *parent;
    std::string name;
    bool is_array;
};

template< typename... T > struct RecursiveHelper {
    typedef std::function< RecursiveHelper(T...) > type;
    RecursiveHelper( type f ) : func(f) {}
    operator type () { return func; }
    type func;
};

typedef RecursiveHelper<std::ifstream&,int&>::type callback_t;

extern std::string global_string;
extern std::vector<std::string> includes;
extern std::unique_ptr<MetaClass> top_level_class;
