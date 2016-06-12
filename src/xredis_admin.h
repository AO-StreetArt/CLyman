//An admin using the xRedis C++ Drivers for Redis

#include "xredis/xRedisClient.h"
#include <string.h>
#include <string>
#include "logging.h"

class xRedisAdmin
{
xRedisClient xRedis;
public:
	xRedisAdmin(RedisNode conn_list[], int conn_list_size);
	~xRedisAdmin();
	const char * load ( const char * key );
	bool save ( const char * key, const char * msg );
	bool exists ( const char * key );
	bool del ( const char * key );
};
