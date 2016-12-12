#pragma once

#include <string>
#include <fstream>

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(stringDbg)

void clear_empty(std::ifstream& f);
std::string camel_case_to_underscore(std::string s);
std::string underscore_to_camel_case(std::string s);
std::string capitalize(std::string s, int pos = 0);
