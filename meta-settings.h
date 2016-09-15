#pragma once

#include <string>
#include <functional>
#include <fstream>
#include <vector>
#include <memory>
#include <map>

// Used only to be able to dynamic cast.
struct Meta {
public:
    virtual ~Meta(){}
    Meta *parent;
    std::string name;
    bool is_array;
};

struct MetaProperty : public Meta {
    std::string default_value;
    std::string type;
    std::map<std::string, std::string> setters;
};

struct MetaClass : public Meta {
        std::vector<std::unique_ptr<MetaProperty>> properties;
        std::vector<std::unique_ptr<MetaClass>> subclasses;
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
extern Meta *current_object;
