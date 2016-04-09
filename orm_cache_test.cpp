#include "orm_cache.h"
#include <string>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds
#include <iostream>

int main()
{

//Cache Pair Tests
std::time_t result = std::time(nullptr);
std::string test_string = "Test1";
Cache_Pair<std::string> cp ( result, test_string& );

//Sleep for 10 seconds
std::this_thread::sleep_for (std::chrono::seconds(10));

//Compare the stored time to the current time
std::time_t res2 = std::time(nullptr);
std::time_t elapsed = res2-result;

std::cout << "Time Elapsed: " << elapsed << std::endl;

std::string tstr;
tstr = cp.get_obj();

std::cout << "Stored String: " << tstr << std::endl;

//Basic Cache Tests
Cache<std::string> c (10);

c.append ( "test-key", "Test2" );
c.append ( "test-key2", "Test3" );
c.append ( "test-key3", "Test4" );

std::cout << "Test String 1: " << c.get ("test-key") << endl;
std::cout << "Test String 2: " << c.get ("test-key2") << endl;
std::cout << "Test String 3: " << c.get ("test-key3") << endl;

return 0;
}
