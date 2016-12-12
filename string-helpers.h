#pragma once

#include <string>
#include <fstream>

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(stringDbg)

void clear_empty(std::ifstream& f);
std::string camel_case_to_underscore(const std::string& s);
std::string underscore_to_camel_case(const std::string& s);
std::string capitalize(const std::string& s, int pos = 0);
