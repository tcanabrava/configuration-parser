#pragma once

#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

struct MetaClass;
struct MetaProperty;

/* struct that represents a variable in the configuration file. */
struct MetaProperty {
  typedef std::shared_ptr<MetaProperty> Ptr;
  std::shared_ptr<MetaClass> parent = nullptr;
  std::string name;
  std::string default_value;
  std::string type;
  std::map<std::string, std::string> setters;
  bool is_enum;
};

/* struct that represents a {} entity in the configuration file. */
struct MetaClass {
  typedef std::shared_ptr<MetaClass> Ptr;
  std::vector<MetaProperty::Ptr> properties;
  std::vector<Ptr> subclasses;
  std::shared_ptr<MetaClass> parent = nullptr;
  std::string name;
};

/* struct that represents the whole configuration file. */
struct MetaConfiguration {
  std::vector<std::string> includes;
  std::shared_ptr<MetaClass> top_level_class = nullptr;
  std::string conf_namespace;
};