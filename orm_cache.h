#include <unordered_map>
#include <string>
#include <ctime>

#ifndef ORMCACHE_H
#define ORMCACHE_H

//A pair of objects to be put into the cache
//with an object and at std::time_t object
//which is the time that object was last
//touched in the cache
template <class T>
class Cache_Pair
{
  //Internal Elements
  std::time_t last_update_time;
  T *obj;
  public:
    Cache_Pair ( std::time_t itime, T& iobj ) {last_update_time=itime;obj=iobj;}
    ~Cache_Pair () {delete obj;}
    std::time_t get_time () {return last_update_time;}
    void set_time ( std::time_t itime ) {last_update_time=itime;}
    T& get_obj () {return obj;}
};

//A Cache to hold arbitrary objects in memory
template <class T>
class Cache
{
  //Internal Elements
  std::unordered_map<std::string, Cache_Pair> int_array;
  int max_elements;
  int num_elements;  
  double max_time;
  bool max_elements_active;
  bool max_time_active;

  public:

    Cache(int imax_elements, double imax_time, bool imax_elements_active, bool imax_time_active) {
      max_time=imax_time;
      max_elements=imax_elements;
      max_elements_active=imax_elements_active;
      max_time_active=imax_time_active;
    }

    Cache(int imax_elements, double imax_time) {
      max_time=imax_time;
      max_elements=imax_elements;
      max_elements_active=true;
      max_time_active=true;
    }

    Cache(int imax_elements) {
      max_time=999.99;
      max_elements=imax_elements;
      max_elements_active=true;
      max_time_active=false;
    }

    ~Cache() {}

    T get (std::string ikey) {
      std::time_t result = std::time(nullptr);
      int_array[ikey].set_time(result);
      return int_array[ikey].get_obj();
    }

    void append (std::string ikey, T obj) {
        //Get the current time and append it with the object to the array
        std::time_t result = std::time(nullptr);
	Cache_Pair<T> cp ( result, obj& );
        int_array.emplace( ikey, cp );
        num_elements=num_elements+1;
    }

    T cull (int* arr) {
      //TO-DO: Cull the cache and push it down to the max
      //Number of objects. Return an array (arr[]) of the culled objects
      //so that these can be pushed to the DB.
    }
};
#endif
