#ifdef _WIN32
#include <Windows.h>
#else
#include <sys/time.h>
#include <ctime>
#endif
#include <sstream>

#include <zmq.hpp>
#include <string>
#include <strings.h>
#include <stdint.h>
#include <stdio.h>

#ifndef LYMAN_UTILS
#define LYMAN_UTILS

//Trims Strings
inline std::string left_trim_string (std::string str) {
    size_t start_pos = str.find_first_not_of(" \t");
    if (std::string::npos != start_pos) {
        str = str.substr(start_pos);
    }
    return str;
}

//Convert a ZMQ Message to a std::string
inline std::string hexDump ( zmq::message_t &aMessage ) {
return std::string(static_cast<char*>(aMessage.data()), aMessage.size());
}

/* Big Ints to do time calculations */
typedef long long int64; typedef unsigned long long uint64;

/*Reliable To String Method*/
#define SSTR( x ) static_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

/* Returns the amount of milliseconds elapsed since the UNIX epoch. Works on both
 * windows and linux. */

inline uint64 GetTimeMs64()
{
#ifdef _WIN32
 /* Windows */
 FILETIME ft;
 LARGE_INTEGER li;

 /* Get the amount of 100 nano seconds intervals elapsed since January 1, 1601 (UTC) and copy it
  * to a LARGE_INTEGER structure. */
 GetSystemTimeAsFileTime(&ft);
 li.LowPart = ft.dwLowDateTime;
 li.HighPart = ft.dwHighDateTime;

 uint64 ret = li.QuadPart;
 ret -= 116444736000000000LL; /* Convert from file time to UNIX epoch time. */
 ret /= 10000; /* From 100 nano seconds (10^-7) to 1 millisecond (10^-3) intervals */

 return ret;
#else
 /* Linux */
 struct timeval tv;

 gettimeofday(&tv, NULL);

 uint64 ret = tv.tv_usec;
 /* Convert from micro seconds (10^-6) to milliseconds (10^-3) */
 ret /= 1000;

 /* Adds the seconds (10^0) after converting them to milliseconds (10^-3) */
 ret += (tv.tv_sec * 1000);

 return ret;
#endif
}

#endif
