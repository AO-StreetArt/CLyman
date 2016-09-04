#include <sstream>
#include <string>
#include <strings.h>
#include <stdint.h>

#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>

#ifndef LYMAN_UTILS
#define LYMAN_UTILS

//This defines a set of universal event types

const int OBJ_UPD = 0;
const int OBJ_CRT = 1;
const int OBJ_GET = 2;
const int OBJ_DEL = 3;
const int OBJ_PUSH = 4;
const int ERR = 100;
const int KILL = 999;
const int PING = 555;

//Internal Message Types
const int OBJ_UPD_GLOBAL = 5;

//Trim Strings
// trim from start
static inline std::string &ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
            std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
            std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
    return ltrim(rtrim(s));
}

/*Reliable To String Method*/
#define SSTR( x ) static_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

#endif
