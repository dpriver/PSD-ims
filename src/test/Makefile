SOURCES=test_client_friend_list.c
HEADERS=

TARGET=$(SOURCES:%.c=$(TEST_BIN_DIR)/%)
COBJS=$(SOURCES:%.c=$(TEST_OBJ_DIR)/%.o)
CHEADS=$(HEADERS)
# LINK_OBJS=$(CLIENT_OBJ_DIR)/* $(SERVER_OBJ_DIR)/* $(COMMON_OBJ_DIR)/*

ECHO_FLAGS=

#-------------------------------------------------------------
# Directories
#-------------------------------------------------------------
PROJECT_DIR=../..
CLIENT_HEAD_DIR=$(PROJECT_DIR)/src/client
SERVER_HEAD_DIR=$(PROJECT_DIR)/src/server
COMMON_HEAD_DIR=$(PROJECT_DIR)/src/common

CLIENT_OBJ_DIR=$(PROJECT_DIR)/build/client
SERVER_OBJ_DIR=$(PROJECT_DIR)/build/server
COMMON_OBJ_DIR=$(PROJECT_DIR)/build/common
TEST_OBJ_DIR=$(PROJECT_DIR)/build/test

TEST_BIN_DIR=$(PROJECT_DIR)/bin/test


#-------------------------------------------------------------
# Configuration
#-------------------------------------------------------------
CFLAGS=-I$(GSOAP_INCLUDE) -I$(COMMON_HEAD_DIR) -I$(CLIENT_HEAD_DIR) -I$(SERVER_HEAD_DIR)
LDFLAGS=-L$(GSOAP_LIB) -L$(CLIENT_OBJ_DIR) -L$(SERVER_OBJ_DIR) -L$(COMMON_OBJ_DIR)
LDLIBS=-lgsoap $(SSL_LIBS) -lclient

SSL_LIBS=-lssl -lcrypto
SSL_FLAGS=-DWITH_OPENSSL



all: $(TARGET)

$(TARGET): $(CHEADS) $(COBJS) | $(TEST_BIN_DIR)

$(COBJS): | $(TEST_OBJ_DIR)

$(TEST_OBJ_DIR)/%.o: %.c
	@echo $(ECHO_FLAGS) "\tCC" $@
	@gcc -c -o $@ $< $(CFLAGS)

$(TEST_BIN_DIR)/%:
	@echo $(ECHO_FLAGS) "\tLD " $(TARGET)	
	@gcc -o $@ $(COBJS) $(COMMON_OBJ_DIR)/*.o $(LDFLAGS) $(SSL_FLAGS) $(LDLIBS)

#------------------------------------------------------------------------------------
#		Directories 
#------------------------------------------------------------------------------------
$(TEST_BIN_DIR):
	@echo $(ECHO_FLAGS) "\tCreating dir $(TEST_BIN_DIR)"
	@mkdir -p $(TEST_BIN_DIR)

$(TEST_OBJ_DIR):
	@echo $(ECHO_FLAGS) "\tCreating dir $(TEST_OBJ_DIR)"
	@mkdir -p $(TEST_OBJ_DIR)

clean:
	@rm -rf *.o

