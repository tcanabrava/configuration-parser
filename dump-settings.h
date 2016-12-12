#pragma once

#include <string>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(dumpHeader)
Q_DECLARE_LOGGING_CATEGORY(dumpSource)

void dump_header(const std::string& s);
void dump_source(const std::string& s);
