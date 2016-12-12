#pragma once

#include <string>
#include <fstream>

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(stringDbg)

/**
 * clear empty characters (spaces, line breaks)
 * from the file being read)
 *
 * @param file the file being stripped of empty characters.
 * */
void clear_empty(std::ifstream& file);

/**
 * receive a string in CamelCase and transform it
 * into camel_case
 *
 * @param s the string to be converted.
 * @return the converted string
 * */
std::string camel_case_to_underscore(const std::string& s);

/**
 * Receive a string in under_score and transform it into
 * UnderScore
 *
 * @param s the string to be converted
 * @return the converted string
 * */
std::string underscore_to_camel_case(const std::string& s);

/**
 * Capitalize the letter on the string defined by the
 * position 'pos'
 *
 * @param s the string that will have a letter capitalized
 * @param pos the index of the letter to be capitalized
 * @return the capitalized string.
 *
 * */
std::string capitalize(const std::string& s, int pos = 0);
