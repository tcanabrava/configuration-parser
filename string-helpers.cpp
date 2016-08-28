#include "string-helpers.h"

std::string camel_case_to_underscore(std::string s) {
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

std::string underscore_to_camel_case(std::string s) {
    std::string ret;
    for(int i = 0, end = s.size(); i < end; ++i) {
        if (s[i] == '_' && i != end) {
            s[i+1] ^= 32;
            continue;
        }
        ret += s[i];
    }
    return ret;
}

std::string capitalize(std::string s, int pos) {
    s[pos] ^= 32;
    return s;
}
