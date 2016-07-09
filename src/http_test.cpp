#include "http_admin.h"
#include <string>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <assert.h>

//----------------------------HTTP Callbacks----------------------------------//

//A String to store response data
std::string writedata;

//This is the callback that gets called when we recieve the response to the
//Get Curl Request
size_t writeCallback(char * buf, size_t size, size_t nmemb, void* up)
{

  logging->debug("Callback Triggered");

//Put the response into a string
for (int c = 0; c<size*nmemb; c++)
{
	writedata.push_back(buf[c]);
}

return size*nmemb;
}

//This is the callback that gets called when we build the message for the
//Put Curl Request
const char data[]="this is what we post to the silly web server";

struct WriteThis {
  const char *readptr;
  long sizeleft;
};

static size_t read_callback(void *ptr, size_t size, size_t nmemb, void *userp)
{
  struct WriteThis *pooh = (struct WriteThis *)userp;

  if(size*nmemb < 1)
    return 0;

  if(pooh->sizeleft) {
    *(char *)ptr = pooh->readptr[0]; /* copy one single byte */
    pooh->readptr++;                 /* advance pointer */
    pooh->sizeleft--;                /* less data left */
    return 1;                        /* we return 1 byte at a time! */
  }

  return 0;                          /* no more data left to deliver */
}

//-----------------------------MAIN METHOD------------------------------------//

int main(int argc, char* argv[])
{
  if (argc > 0)
  {

    //----------------------------TEST SETUP----------------------------------//

    //Variables to store URL's
    std::string post = "http://httpbin.org/post";
    std::string put = "http://httpbin.org/put";
    std::string get = "http://httpbin.org/get";
    std::string del = "http://httpbin.org/delete";

    char *POSTURL = new char[post.length() + 1];
    strcpy(POSTURL, post.c_str());

    char *PUTURL = new char[put.length() + 1];
    strcpy(PUTURL, put.c_str());

    char *GETURL = new char[get.length() + 1];
    strcpy(GETURL, get.c_str());

    char *DELETEURL = new char[del.length() + 1];
    strcpy(DELETEURL, del.c_str());

    //Logging

    std::string initFileName = "log4cpp.properties";
    try {
    	log4cpp::PropertyConfigurator::configure(initFileName);
    }
    catch ( log4cpp::ConfigureFailure &e ) {
    	printf("[log4cpp::ConfigureFailure] caught while reading Logging Configuration File");
    	printf(e.what());
    	exit(1);
    }

    log4cpp::Category& root = log4cpp::Category::getRoot();

    log4cpp::Category& sub1 = log4cpp::Category::getInstance(std::string("sub1"));

    log4cpp::Category& log = log4cpp::Category::getInstance(std::string("sub1.log"));

    logging = &log;

    //----------------------------MAIN TEST-----------------------------------//

    HttpAdmin ha;

    logging->debug("Get URL");
    logging->debug(GETURL);
    logging->debug("Put URL");
    logging->debug(PUTURL);
    logging->debug("Post URL");
    logging->debug(POSTURL);
    logging->debug("Delete URL");
    logging->debug(DELETEURL);

    //-------------------------------GET--------------------------------------//

    //We set up the structure to store the return data
    writedata.clear();

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
      logging->debug("Retrieved:");
      logging->debug(writedata);
    }

    //-------------------------------PUT--------------------------------------//

    //Set up the info to send on the body of the put request
    curl_easy_setopt(ha.get_instance(), CURLOPT_READDATA, data);
    curl_easy_setopt(ha.get_instance(), CURLOPT_READFUNCTION, &read_callback);

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

    logging->debug("Tests completed");

    ha.shutdown();

  }

  return 0;
}
