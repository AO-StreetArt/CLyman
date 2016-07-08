//The HTTP Administrator
//Responsible for placing HTTP Requests using curl

#include <string>
#include <curl/curl.h>

class HttpAdmin
{
CURL* curl;
public:
  //Initializer
  HttpAdmin() {curl_global_init(CURL_GLOBAL_ALL);}
  //Shutdown the admin
  void shutdown() {curl_global_cleanup();}
  //Return the instance to bind callbacks against
  CURL* get_instance() {return curl;}
  //HTTP Methods
  void put(char * url);
  void get(char * url);
  void post(char * url, char * data);
  void del(char * url);
  bool send(int timeout);
};
