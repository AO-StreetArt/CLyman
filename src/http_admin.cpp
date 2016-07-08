#include "http_admin.h"

//Needs a Read function and pointer registered
void HttpAdmin::put(char * url)
{
  curl = curl_easy_init();

  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
}

//Needs a Read function and pointer registered
void HttpAdmin::get(char * url)
{
  curl = curl_easy_init();

  curl_easy_setopt(curl, CURLOPT_URL, url);
}

//Needs a write function registered
void HttpAdmin::post(char * url, char * data)
{
  curl = curl_easy_init();

  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_POST, 1L);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
}

void HttpAdmin::del(char * url)
{
  curl = curl_easy_init();

  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
}

bool HttpAdmin::send(int timeout)
{
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);
  bool ret = false;
  if (CURLE_OK == curl_easy_perform(curl))
  {
  //Successful request performed
  ret = true;
  }

  curl_easy_cleanup(curl);
  return ret;
}
