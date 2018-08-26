# Only responsible for compiling the database_manager

INCL_DIRS = -I$(CURDIR)/../ -Iinclude -I/usr/local/include -I/usr/local/include/mongocxx/v_noabi -I/usr/local/include/libmongoc-1.0 -I/usr/local/include/bsoncxx/v_noabi -I/usr/local/include/libbson-1.0

database_manager.o: include/database_manager.h
	$(CC) $(CFLAGS) -o $@ -c database_manager.cpp $(STD) $(INCL_DIRS)
