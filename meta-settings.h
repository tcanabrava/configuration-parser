#pragma once

#include <string>
#include <functional>
#include <fstream>
#include <vector>
#include <memory>

// Used only to be able to dynamic cast.
struct Meta {
public:
    virtual ~Meta(){}
};

struct MetaProperty : public Meta {
    std::string name;
    std::string default_value;
    std::string type;
};

struct MetaClass : public Meta {
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

extern std::string global_string;
extern std::vector<std::string> includes;
extern std::unique_ptr<MetaClass> top_level_class;
extern Meta *current_object;
