API_TEST_FILES = unit/api/object_list_test.cpp
MODEL_TEST_FILES = unit/model/transform_test.cpp
USER_TEST_FILES = unit/user/account_manager_test.cpp
TEST_FILES = $(API_TEST_FILES) $(MODEL_TEST_FILES) $(USER_TEST_FILES)
INCL_DIRS = -I/usr/local/include -I. -I../src -I../src/model -I../src/api -I../src/user
SRC_LOC = ../src/
MODEL_LOC = $(SRC_LOC)/model
API_LOC = $(SRC_LOC)/api
MODEL_SUB_OBJECTS = $(MODEL_LOC)/object_document.o
API_SUB_OBJECTS = $(API_LOC)/json_object_list.o
SUB_OBJECTS = $(MODEL_SUB_OBJECTS) $(API_SUB_OBJECTS)

tests: test_main.o
	$(CC) $(CFLAGS) test_main.o $(TEST_FILES) $(SUB_OBJECTS) -o $@ $(LIBS) $(STD) $(INCL_DIRS)

test_main.o: test_main.cpp catch.hpp
	$(CC) $(CFLAGS) test_main.cpp -c
