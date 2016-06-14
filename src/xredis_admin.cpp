#include "xredis_admin.h"

// AP Hash Function

unsigned int APHash(const char *str) {
	unsigned int hash = 0;
	int i;
	for (i=0; *str; i++) {
		if ((i&  1) == 0) {
			hash ^= ((hash << 7) ^ (*str++) ^ (hash >> 3));
		} else {
			hash ^= (~((hash << 11) ^ (*str++) ^ (hash >> 5)));
		}
	}
	return (hash&  0x7FFFFFFF);
}

//Initialization
xRedisAdmin::xRedisAdmin(RedisNode conn_list[], int conn_list_size)
{
enum {
CACHE_TYPE_1,
CACHE_TYPE_2,
CACHE_TYPE_MAX,
};

xRed.Init(CACHE_TYPE_MAX);
bool bret = xRed.ConnectRedisCache(conn_list, conn_list_size, CACHE_TYPE_1);
if (!bret) {
logging->error("Error connecting to Redis DB");
}
}

//Exists
bool xRedisAdmin::exists(const char * key)
{
  enum {
  CACHE_TYPE_1,
  CACHE_TYPE_2,
  CACHE_TYPE_MAX,
  };
RedisDBIdx d(&xRed);
d.CreateDBIndex(key, APHash, CACHE_TYPE_1);
char szKey[256] = {0};
sprintf(szKey, key);
return xRed.exists(d, szKey);
}

//Load
const char * xRedisAdmin::load(const char * key)
{
  enum {
  CACHE_TYPE_1,
  CACHE_TYPE_2,
  CACHE_TYPE_MAX,
  };
RedisDBIdx d(&xRed);
d.CreateDBIndex(key, APHash, CACHE_TYPE_1);
char szKey[256] = {0};
sprintf(szKey, key);
std::string strValue;
bool bret = xRed.get(d, szKey, strValue);
if (bret) {
return strValue.c_str();
}
else {
logging->error("Error Loading from Redis DB");
logging->error(d.GetErrInfo());
return NULL;
}
}

//Save
bool xRedisAdmin::save(const char * key, const char * val)
{
  enum {
  CACHE_TYPE_1,
  CACHE_TYPE_2,
  CACHE_TYPE_MAX,
  };
RedisDBIdx d(&xRed);
d.CreateDBIndex(key, APHash, CACHE_TYPE_1);
char szKey[256] = {0};
sprintf(szKey, key);
bool ret_val = xRed.set(d, szKey, val);
if (!ret_val) {
logging->error("Error writing to Redis DB");
logging->error(d.GetErrInfo());
}
return ret_val;
}

//Delete
bool xRedisAdmin::del(const char * key)
{
  enum {
  CACHE_TYPE_1,
  CACHE_TYPE_2,
  CACHE_TYPE_MAX,
  };
RedisDBIdx d(&xRed);
d.CreateDBIndex(key, APHash, CACHE_TYPE_1);
char szKey[256] = {0};
sprintf(szKey, key);
bool bret = xRed.del(d, szKey);
if (!bret) {
logging->error("Error Deleting from Redis DB");
logging->error(d.GetErrInfo());
}
return bret;
}

//Close
xRedisAdmin::~xRedisAdmin()
{

}