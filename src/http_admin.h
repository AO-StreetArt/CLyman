//The HTTP Administrator
//Responsible for placing HTTP Requests using curl

#include <string>
#include <curl/curl.h>

class HttpAdmin
{
CURL* curl;
bool send(int timeout);
public:
  //Initializer
  HttpAdmin() {curl_global_init(CURL_GLOBAL_ALL);curl = curl_easy_init();}
  //Shutdown the admin
  void shutdown() {curl_global_cleanup();}
  //Return the instance to bind callbacks against
  CURL* get_instance() {return curl;}
  //HTTP Methods
  void put(char * url, int timeout);
  void get(char * url, int timeout);
  void post(char * url, char * data, int timeout);
  void del(char * url, int timeout);
};
