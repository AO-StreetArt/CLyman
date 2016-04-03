#include <iostream>
#include <string>

#include "event_dispatcher.h"
#include "FastDelegate.h"
#include "lyman_utils.h"

void simple_func(std::string str)
{
std::cout << str << std::endl;
}

void complex_func(std::string str)
{
std::cout << str << std::endl;
}

int main ()
{
//Declare the test variables
fastdelegate::FastDelegate1<std::string> signal[12];

//Bind the global functions
signal[OBJ_CRT].bind(&simple_func);
signal[OBJ_UPD].bind(&complex_func);

//Emit an object create signal, then an object update signal
signal[OBJ_CRT]("Hello");
signal[OBJ_UPD]("World");

return 0;
}

//In order to bind to an Obj3, we have to call a method
//from that object and pass it by reference in the bind call
