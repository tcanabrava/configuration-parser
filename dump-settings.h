#pragma once

#include <string>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(dumpHeader)
Q_DECLARE_LOGGING_CATEGORY(dumpSource)

/* Generates the header file for the configuration */
void dump_header(const std::string& filename);

/* generates the source fole for the configuration */
void dump_source(const std::string& filename);
