# define some Makefile variables for the compiler and compiler flags
# to use Makefile variables later in the Makefile: $()
#
CC = g++
SLC = ar rcs
CFLAGS  = -g -Wall
STD = -std=c++11
OBJS = src/Obj3.pb.cc src/configuration_manager.o src/lyman_utils.o src/globals.o src/obj3.o src/document_manager.o main.o
TESTS = redis_test obj3_test couchbase_test configuration_test
BENCHMARKS = couchbase_benchmark obj3_benchmark redis_benchmark main_benchmark
LIBS = -lpthread -llog4cpp
FULL_LIBS = -laossl -lcurl -lpthread -lxredis -lzmq -lcouchbase -llog4cpp -luuid `pkg-config --cflags --libs protobuf hiredis`

PROTOC = protoc
PROTO_OPTS = -I=src
PROTO = src/Obj3.proto

# -------------------------- Central Targets --------------------------------- #

lyman: $(OBJS) scripts/Obj3_pb2.py
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(FULL_LIBS) $(STD)

test: $(TESTS)

benchmarks: $(BENCHMARKS)

clean: clean_local clean_tests clean_benchmarks

# ------------------------------- Tests -------------------------------------- #

redis_test: src/redis_test.o
	$(CC) $(CFLAGS) -o $@ $^ $(FULL_LIBS) $(STD)

src/redis_test.o: src/test/redis_test.cpp src/obj3.cpp src/obj3.h src/Obj3.proto
	$(CC) $(CFLAGS) -o $@ -c src/test/redis_test.cpp $(STD)

obj3_test: src/Obj3.pb.cc src/obj3.o src/obj3_test.o
	$(CC) $(CFLAGS) -o $@ $^ $(FULL_LIBS) $(STD)

src/obj3_test.o: src/test/obj3_test.cpp src/obj3.cpp src/obj3.h src/Obj3.proto
	$(CC) $(CFLAGS) -o $@ -c src/test/obj3_test.cpp $(STD)

couchbase_test: src/Obj3.pb.cc src/lyman_utils.o src/couchbase_test.o src/obj3.o
	$(CC) $(CFLAGS) -o $@ $^ $(FULL_LIBS) $(STD)

src/couchbase_test.o: src/test/couchbase_test.cpp src/obj3.cpp src/obj3.h src/Obj3.proto
	$(CC) $(CFLAGS) -o $@ -c src/test/couchbase_test.cpp $(STD)

configuration_test: src/configuration_manager.o src/configuration_test.o
	$(CC) $(CFLAGS) -o $@ $^ $(FULL_LIBS) $(STD)

src/configuration_test.o: src/test/configuration_test.cpp src/configuration_manager.cpp src/configuration_manager.h
	$(CC) $(CFLAGS) -o $@ -c src/test/configuration_test.cpp $(STD)

# ----------------------------- Benchmarks ----------------------------------- #

couchbase_benchmark: src/Obj3.pb.cc src/lyman_utils.o src/couchbase_benchmark.o src/obj3.o
	$(CC) $(CFLAGS) -o $@ $^ $(FULL_LIBS) $(STD)

src/couchbase_benchmark.o: src/test/couchbase_benchmark.cpp src/obj3.cpp src/obj3.h src/Obj3.proto
	$(CC) $(CFLAGS) -o $@ -c src/test/couchbase_benchmark.cpp $(STD)

obj3_benchmark: src/Obj3.pb.cc src/obj3.o src/obj3_benchmark.o
	$(CC) $(CFLAGS) -o $@ $^ $(FULL_LIBS) $(STD)

src/obj3_benchmark.o: src/test/obj3_benchmark.cpp src/obj3.cpp src/obj3.h src/Obj3.proto
	$(CC) $(CFLAGS) -o $@ -c src/test/obj3_benchmark.cpp $(STD)

redis_benchmark: src/redis_benchmark.o
	$(CC) $(CFLAGS) -o $@ $^ $(FULL_LIBS) $(STD)

src/redis_benchmark.o: src/test/redis_benchmark.cpp src/obj3.cpp src/obj3.h src/Obj3.proto
	$(CC) $(CFLAGS) -o $@ -c src/test/redis_benchmark.cpp $(STD)

main_benchmark: src/Obj3.pb.cc src/configuration_manager.o src/lyman_utils.o src/globals.o src/obj3.o src/document_manager.o main_benchmark.o
	$(CC) $(CFLAGS) -o $@ $^ $(FULL_LIBS) $(STD)

main_benchmark.o: main_benchmark.cpp src/obj3.cpp src/obj3.h src/Obj3.proto src/configuration_manager.cpp src/configuration_manager.h src/document_manager.cpp src/document_manager.h src/lyman_utils.cpp src/lyman_utils.h src/globals.cpp src/globals.h
	$(CC) $(CFLAGS) -o $@ -c main_benchmark.cpp $(STD)

# ---------------------------- Main Project ---------------------------------- #

scripts/Obj3_pb2.py: $(PROTO)
	$(PROTOC) $(PROTO_OPTS) --python_out=scripts $(PROTO)

src/configuration_manager.o: src/configuration_manager.cpp src/configuration_manager.h
	$(CC) $(CFLAGS) -o $@ -c src/configuration_manager.cpp $(STD)

src/Obj3.pb.cc: $(PROTO)
	$(PROTOC) $(PROTO_OPTS) --cpp_out=src $(PROTO)

src/obj3.o: src/obj3.cpp src/obj3.h src/Obj3.pb.cc
	$(CC) $(CFLAGS) -o $@ -c src/obj3.cpp $(STD)

src/lyman_utils.o: src/lyman_utils.cpp src/lyman_utils.h
	$(CC) $(CFLAGS) -o $@ -c src/lyman_utils.cpp $(STD)

src/globals.o: src/globals.cpp src/globals.h
	$(CC) $(CFLAGS) -o $@ -c src/globals.cpp $(STD)

src/document_manager.o: src/document_manager.cpp src/document_manager.h
	$(CC) $(CFLAGS) -o $@ -c src/document_manager.cpp $(STD)

main.o: main.cpp
	$(CC) $(CFLAGS) -o $@ -c main.cpp $(STD)

# --------------------------- Clean Project ---------------------------------- #

clean_local:
	$(RM) lyman *.o src/*.o *~ *.log *.log.*

clean_tests:
	$(RM) *_test

clean_benchmarks:
	$(RM) *_benchmark
