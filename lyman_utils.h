#ifdef _WIN32
#include <Windows.h>
#else
#include <sys/time.h>
#include <ctime>
#endif
#include <sstream>

#include "FastDelegate.h"
#include <zmq.hpp>
#include <string>
#include <strings.h>
#include <stdint.h>

//Convert a zmq message to std::string
std::string hexDump( zmq::message_t & aMessage ) {
  // I'm going to build a hex/ascii dump like you've seen so many times before
  std::string  msg;
  std::string  ascii;
  // get the pointer to the start of the message's payload - and it's size
  char *buff = (char *)aMessage.data();
  int   size = aMessage.size();
  char *end  = buff + size - 1;
  // see if it's the trivial case
  if (buff == NULL) {
    msg.append("NULL");
  } else {
    // get a place to hold the conversion of each byte
    char   hex[3];
    bzero(hex, 3);
    // run through the valid data in the buffer
    for (const char *p = buff; p <= end; ++p) {
      // generate the hex code for the byte and add it
      snprintf(hex, 3, "%02x", (uint8_t)(*p));
      msg.append(hex).append(" ");
      // if it's printable, add it to the ascii part, otherwise, put a '.'
      if (isprint(*p)) {
        ascii.append(p, 1);
      } else {
        ascii.append(".");
      }
      // see if we have a complete line
      if (ascii.size() >= 19) {
        msg.append(" ").append(ascii).append("\n");
        ascii.clear();
      }
    }
    // if we have anything left, put it on the line as well
    if (ascii.size() > 0) {
      msg.append((19 - ascii.length())*3 + 1, ' ').append(ascii);
    }
  }

  return msg;
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


