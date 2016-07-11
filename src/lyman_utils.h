#include <sstream>
#include <zmq.hpp>
#include <string>
#include <strings.h>
#include <stdint.h>
#include <stdio.h>

#ifndef LYMAN_UTILS
#define LYMAN_UTILS

//This defines a set of universal event types

extern const int OBJ_UPD;
extern const int OBJ_CRT;
extern const int OBJ_GET;
extern const int OBJ_DEL;
extern const int OBJ_PUSH;

//Trims Strings
inline std::string left_trim_string (std::string str) {
    size_t start_pos = str.find_first_not_of(" \t");
    if (std::string::npos != start_pos) {
        str = str.substr(start_pos);
    }
    return str;
}

/*Reliable To String Method*/
#define SSTR( x ) static_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

#endif
