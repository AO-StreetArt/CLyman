#include "http_admin.h"
#include <string>
#include <stdlib.h>
#include <iostream>
#include <assert.h>

std::string readdata;

std::string data;

//This is the callback that gets called when we recieve the response to the
//Curl Request
size_t writeCallback(char * buf, size_t size, size_t nmemb, void* up)
{

//Put the response into a string
for (int c = 0; c<size*nmemb; c++)
{
	data.push_back(buf[c]);
}

return size*nmemb;
}

int main(int argc, char* argv[])
{
  if (argc > 0)
  {
    HttpAdmin ha;

    //-------------------------------PUT--------------------------------------//

    ha.put(argv[0]);
    bool success = ha.send(5);
    if (!success)
    {
      //We now have the full response
      assert(false);
    }

    //-------------------------------GET--------------------------------------//

    //Set up a get request
    ha.get(argv[0]);

    //We set up the structure to store the return data
    data.clear();

    //Send the request
    success = ha.send(5);
    if (!success)
    {
      //We now have the full response
      assert(false);
    }

    //-------------------------------POST-------------------------------------//

    ha.post(argv[0], "CLYMAN");
    success = ha.send(5);
    if (!success)
    {
      //We now have the full response
      assert(false);
    }

    //------------------------------DELETE------------------------------------//

    ha.del(argv[0]);
    success = ha.send(5);
    if (!success)
    {
      //We now have the full response
      assert(false);
    }

    ha.shutdown();

  }

  return 0;
}
