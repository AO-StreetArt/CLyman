#include "event_dispatcher.h"

//Integer Constant Declarations
//These are standard constants for event type
const int OBJ_UPD = 0;
const int OBJ_CRT = 1;
const int OBJ_GET = 2;
const int OBJ_DEL = 3;
const int OBJ_PUSH = 4;

//Event Dispatcher Declaration
DocumentDelegate doc_dispatch[12];
