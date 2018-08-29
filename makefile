# define some Makefile variables for the compiler and compiler flags
# to use Makefile variables later in the Makefile: $()
#
export CC = g++
export CFLAGS  = -g -Wall
export STD = -std=c++11
export AOSSL_LIBS = -laossl -luuid -lcurl -lbsoncxx -lmongocxx
export POCO_LIBS = -lPocoNetSSL -lPocoCrypto -lPocoNet -lPocoUtil -lPocoFoundation
export LIBS = $(AOSSL_LIBS) $(POCO_LIBS) -lboost_system -lpthread
INCL_DIRS = -I. -I$(CURDIR)/src/ -I/usr/local/include -I/usr/local/include/mongocxx/v_noabi -I/usr/local/include/libmongoc-1.0 -I/usr/local/include/bsoncxx/v_noabi -I/usr/local/include/libbson-1.0
LINK_DIRS = -L/usr/local/lib
SUB_OBJECTS = src/model/object_document.o src/model/animation_property.o src/api/json_object_list.o src/api/json_property_list.o src/db/database_manager.o
INSTALL_CONFIG_DIR = /etc/clyman
INSTALL_LOG_DIR = /var/log/clyman
INSTALL_DIR = /usr/local/bin


.PHONY: mksrc test

# -------------------------- Central Targets --------------------------------- #

build: clyman

install: install_config install_controller install_service install_executable install_log

	# ------------------------- Build Executable --------------------------------- #

clyman: mksrc
	$(CC) $(CFLAGS) -o $@ main.cpp $(SUB_OBJECTS) $(LIBS) $(STD) $(INCL_DIRS) $(LINK_DIRS)

# ----------------------------- Submakes ------------------------------------- #

mksrc:
	@$(MAKE) -C src

test: mksrc
	@$(MAKE) -C tests

# -------------------------- Install Scripts --------------------------------- #

install_config: mk_config_dir
	cp release/app.properties $INSTALL_CONFIG_DIR/app.properties

mk_config_dir:
	mkdir -p $INSTALL_CONFIG_DIR

install_controller:
	cp scripts/linux/clyman_controller.sh $INSTALL_DIR/clyman_controller.sh

install_service:
	cp clyman.service /etc/systemd/system/clyman.service

install_executable:
	cp clyman $INSTALL_DIR/clyman

install_log:
	mkdir /var/log/clyman

# --------------------------- Clean Project ---------------------------------- #

clean:
	rm clyman src/*/*.o tests/tests

reset: clean
	rm tests/test_main.o

# ------------------------- Uninstall Project -------------------------------- #

uninstall:
	rm $INSTALL_CONFIG_DIR/app.properties $INSTALL_DIR/clyman_controller.sh /etc/systemd/system/clyman.service $INSTALL_DIR/clyman
