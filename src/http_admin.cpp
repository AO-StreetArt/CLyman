#include "http_admin.h"

bool HttpAdmin::send(char * url, int timeout)
{
  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);
  bool ret = false;
  if (CURLE_OK == curl_easy_perform(curl))
  {
  //Successful request performed
  ret = true;
  }

  curl_easy_cleanup(curl);
  curl = curl_easy_init();
  return ret;
}

//Needs a Read function and pointer registered
bool HttpAdmin::put(char * url, int timeout)
{
  curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
  return send(url, timeout);
}

//Needs a Read function and pointer registered
bool HttpAdmin::get(char * url, int timeout)
{
  return send(url, timeout);
}

//Needs a write function registered
bool HttpAdmin::post(char * url, char * data, int timeout)
{
  curl_easy_setopt(curl, CURLOPT_POST, 1L);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
  return send(url, timeout);
}

bool HttpAdmin::del(char * url, int timeout)
{
  curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
  return send(url, timeout);
}
