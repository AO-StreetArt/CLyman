#include "http_admin.h"
#include <string>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <assert.h>

//A String to store response data
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

    //Variables to store URL's
    char * POSTURL;
    char * PUTURL;
    char * GETTURL;
    char * DELETEURL;

    //Read the configuration file with URL info
    std::string line;
    std::ifstream file (argv[0]);

    if (file.is_open()) {
      while (getline (file, line) ) {
        //Read a line from the property file

        //Figure out if we have a blank or comment line
        bool keep_going = true;
        if (line.length() > 0) {
          if (line[0] == '/' && line[1] == '/') {
            keep_going=false;
          }
        }
        else {
          keep_going=false;
        }

        if (keep_going==true) {
          int eq_pos = line.find("=", 0);
          std::string var_name = line.substr(0, eq_pos);
          std::string var_value = line.substr(eq_pos+1, line.length() - eq_pos);
          char *cstr = new char[var_value.length() + 1];
          strcpy(cstr, var_value.c_str());
          if (var_name=="Put_URL") {
            PUTURL=cstr;
          }
          if (var_name=="Post_URL") {
            POSTURL=cstr;
          }
          else if (var_name=="Delete_URL") {
            DELETEURL=cstr;
          }
          else if (var_name=="Get_URL") {
            GETTURL=cstr;
          }
          delete cstr;
        }
      }
    }



    HttpAdmin ha;

    //-------------------------------GET--------------------------------------//

    //We set up the structure to store the return data
    data.clear();

    curl_easy_setopt(ha.get_instance(), CURLOPT_WRITEFUNCTION, &writeCallback);

    //Send the request
    bool success = ha.get(GETURL, 5);
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

    success = ha.put(PUTURL, 5);
    if (!success)
    {
      //We now have the full response
      assert(false);
    }

    //-------------------------------POST-------------------------------------//

    success = ha.post(POSTURL, "CLYMAN", 5);
    if (!success)
    {
      //We now have the full response
      assert(false);
    }

    //------------------------------DELETE------------------------------------//

    success = ha.del(DELETEURL, 5);
    if (!success)
    {
      //We now have the full response
      assert(false);
    }

    ha.shutdown();

  }

  return 0;
}
