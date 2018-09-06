API_TEST_FILES = unit/api/object_list_test.cpp unit/api/property_list_test.cpp
MODEL_TEST_FILES = unit/model/transform_test.cpp unit/model/object_test.cpp unit/model/property_test.cpp unit/model/graph_handle_test.cpp
USER_TEST_FILES = unit/user/account_manager_test.cpp
DB_TEST_FILES = integration/db/db_manager_test.cpp
TEST_FILES = $(API_TEST_FILES) $(MODEL_TEST_FILES) $(USER_TEST_FILES) $(DB_TEST_FILES)
PROJECT_INCL_DIRS = -I../src -I../src/model -I../src/api -I../src/user -I../src/db -I../src/app
MONGO_INCL_DIRS = -I/usr/local/include/mongocxx/v_noabi -I/usr/local/include/libmongoc-1.0 -I/usr/local/include/bsoncxx/v_noabi -I/usr/local/include/libbson-1.0
INCL_DIRS = -I/usr/local/include -I. $(PROJECT_INCL_DIRS) $(MONGO_INCL_DIRS)
SRC_LOC = ../src/
MODEL_LOC = $(SRC_LOC)/model
API_LOC = $(SRC_LOC)/api
DB_LOC = $(SRC_LOC)/db
MODEL_SUB_OBJECTS = $(MODEL_LOC)/object_document.o $(MODEL_LOC)/animation_property.o
API_SUB_OBJECTS = $(API_LOC)/json_object_list.o $(API_LOC)/json_property_list.o
DB_SUB_OBJECTS = $(DB_LOC)/core_database_manager.o $(DB_LOC)/property_database_manager.o $(DB_LOC)/obj3_database_manager.o
SUB_OBJECTS = $(MODEL_SUB_OBJECTS) $(API_SUB_OBJECTS) $(DB_SUB_OBJECTS)

tests: test_main.o $(TEST_FILES)
	$(CC) $(CFLAGS) test_main.o $(TEST_FILES) $(SUB_OBJECTS) -o $@ $(LIBS) $(STD) $(INCL_DIRS)

test_main.o: test_main.cpp catch.hpp
	$(CC) $(CFLAGS) test_main.cpp -c
