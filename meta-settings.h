#pragma once

#include <string>
#include <functional>
#include <fstream>
#include <vector>
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
