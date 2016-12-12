#include "string-helpers.h"
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(stringDbg, "stringDbg")

std::string camel_case_to_underscore(const std::string& s) {
    std::string ret;
    for(char x : s){
        if (x >= 'A' && x <= 'Z') {
            ret += '_';
            ret += (char) (x | 32);
        } else {
            ret += x;
        }
    }
    return ret;
}

std::string underscore_to_camel_case(const std::string& s) {
    std::string ret;
    for(int i = 0, end = s.size()-1; i < end; ++i) {
        if (s[i] == '_') {
            ret += s[i+1] ^ 32;
            continue;
        }
        ret += s[i];
    }
    return ret;
}

std::string capitalize(const std::string& s, int pos) {
    std::string ret = s;
    ret[pos] ^= 32;
    return ret;
}

void clear_empty(std::ifstream& f) {
    while(f.peek() == ' '  || f.peek() == '\n'){
        f.ignore();
    }
}
