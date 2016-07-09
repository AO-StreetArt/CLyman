#include "http_admin.h"
#include <string>
#include <stdlib.h>
#include <iostream>
#include <assert.h>

std::string data;

//This is the callback that gets called when we recieve the response to the
//Curl Request
size_t writeCallback(char * buf, size_t size, size_t nmemb, void* up)
{

  std::cout << "Callback Triggered" << std::endl;

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

    //-------------------------------GET--------------------------------------//

    //We set up the structure to store the return data
    data.clear();

    curl_easy_setopt(ha.get_instance(), CURLOPT_WRITEFUNCTION, &writeCallback);

    //Send the request
    bool success = ha.get(argv[0], 5);
    if (!success)
    {
      //We now have the full response
      assert(false);
    }
    else
    {
      std::cout << data << std::endl;
    }

    //-------------------------------PUT--------------------------------------//

    success = ha.put(argv[0], 5);
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
