#pragma once

#include "meta-settings.h"
#include <memory>

bool pass_as_const_ref(const std::string& type);

void dump_source_get_methods(std::ofstream& f, MetaClass *currClass);
void dump_source_set_methods(std::ofstream& f, MetaClass *currClass);
void dump_header_properties(std::ofstream &file, const std::vector<std::shared_ptr<MetaProperty>> &properties);
void dump_header_q_properties(std::ofstream& f,  const std::vector<std::shared_ptr<MetaProperty>> &properties);
void dump_parameter(std::ofstream& file, const std::shared_ptr<MetaProperty>& property);
void dump_notice(std::ofstream& f);
