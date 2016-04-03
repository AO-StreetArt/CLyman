//Event Dispatcher
//This defines a set of universal event types for the service
//It also exposes signals which allow for the native api calls
//to FastDelegates to be utilized
#ifndef EVENT_DISPATCHER_H
#define EVENT_DISPATCHER_H

#include <string>

#include "FastDelegate.h"

//This defines a set of universal event types
//Another file can retrieve these with the following
//added at the top of the file:
//
// extern const std::string OBJ_UPD;
// etc...

extern const int OBJ_UPD;
extern const int OBJ_CRT;
extern const int OBJ_GET;
extern const int OBJ_DEL;
extern const int OBJ_PUSH;

//We can create an event dispatcher like this:
//fastdelegate::FastDelegate1<Obj3> signal[12];

//We can bind a function to an event with the below syntax:
//signal[event_type_int].bind(func_pointer);

//We can emit an event with the below syntax:
//signal[event_type_int](*obj);

#endif
