#pragma once

#include <string>

struct MetaConfiguration;

/* Generates the header file for the configuration */
void dump_header(
    const MetaConfiguration &config,
    const std::string &filename,
    const std::string &exportHeader);

/* generates the source fole for the configuration */
void dump_source(const MetaConfiguration &config, const std::string &filename);
