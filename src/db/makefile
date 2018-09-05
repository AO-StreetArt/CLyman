# Only responsible for compiling the database_manager

OBJS = core_database_manager.o property_database_manager.o obj3_database_manager.o
INCL_DIRS = -I$(CURDIR)/../ -Iinclude -I/usr/local/include -I/usr/local/include/mongocxx/v_noabi -I/usr/local/include/libmongoc-1.0 -I/usr/local/include/bsoncxx/v_noabi -I/usr/local/include/libbson-1.0

default: $(OBJS)

core_database_manager.o: include/core_database_manager.h
	$(CC) $(CFLAGS) -o $@ -c core_database_manager.cpp $(STD) $(INCL_DIRS)

property_database_manager.o: include/property_database_manager.h core_database_manager.o
	$(CC) $(CFLAGS) -o $@ -c property_database_manager.cpp $(STD) $(INCL_DIRS)

obj3_database_manager.o: include/obj3_database_manager.h property_database_manager.o
	$(CC) $(CFLAGS) -o $@ -c obj3_database_manager.cpp $(STD) $(INCL_DIRS)
