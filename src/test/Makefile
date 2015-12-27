SOURCES=
CLIENT_SOURCES=
SERVER_SOURCES=
HEADERS=

CLIENT_TARGET=$(CLIENT_SOURCES:%.c=$(CLIENT_TEST_BIN_DIR)/%)
SERVER_TARGET=$(SERVER_SOURCES:%.c=$(SERVER_TEST_BIN_DIR)/%)

CLIENT_COBJS=$(CLIENT_SOURCES:%.c=$(CLIENT_TEST_OBJ_DIR)/%.o)
SERVER_COBJS=$(SERVER_SOURCES:%.c=$(SERVER_TEST_OBJ_DIR)/%.o)
CHEADS=$(HEADERS)
# LINK_OBJS=$(CLIENT_OBJ_DIR)/* $(SERVER_OBJ_DIR)/* $(COMMON_OBJ_DIR)/*
CLIENT_RPC_LIBS=soapC soapClient
SERVER_RPC_LIBS=soapC soapServer

CLIENT_RPC_OBJS=$(CLIENT_RPC_LIBS:%=$(RPC_OBJ_DIR)/%.o)
SERVER_RPC_OBJS=$(SERVER_RPC_LIBS:%=$(RPC_OBJ_DIR)/%.o)

ECHO_FLAGS=

#-------------------------------------------------------------
# Directories
#-------------------------------------------------------------
PROJECT_DIR=../..
CLIENT_HEAD_DIR=$(PROJECT_DIR)/src/client
SERVER_HEAD_DIR=$(PROJECT_DIR)/src/server
COMMON_HEAD_DIR=$(PROJECT_DIR)/src/common
RPC_HEAD_DIR=$(PROJECT_DIR)/src/rpc

CLIENT_OBJ_DIR=$(PROJECT_DIR)/build/client
SERVER_OBJ_DIR=$(PROJECT_DIR)/build/server
COMMON_OBJ_DIR=$(PROJECT_DIR)/build/common
RPC_OBJ_DIR=$(PROJECT_DIR)/build/rpc
TEST_OBJ_DIR=$(PROJECT_DIR)/build/test
CLIENT_TEST_OBJ_DIR=$(TEST_OBJ_DIR)/client
SERVER_TEST_OBJ_DIR=$(TEST_OBJ_DIR)/server

TEST_BIN_DIR=$(PROJECT_DIR)/bin/test
CLIENT_TEST_BIN_DIR=$(PROJECT_DIR)/bin/test/client
SERVER_TEST_BIN_DIR=$(PROJECT_DIR)/bin/test/server

#-------------------------------------------------------------
# Configuration
#-------------------------------------------------------------
MYSQL_CFLAGS := $(shell mysql_config --cflags)
CLIENT_CFLAGS=-I$(GSOAP_INCLUDE) -I$(COMMON_HEAD_DIR) -I$(CLIENT_HEAD_DIR) -I$(RPC_HEAD_DIR) $(MYSQL_CFLAGS)
SERVER_CFLAGS=-I$(GSOAP_INCLUDE) -I$(COMMON_HEAD_DIR) -I$(SERVER_HEAD_DIR) -I$(RPC_HEAD_DIR) $(MYSQL_CFLAGS)
MYSQL_LDFLAGS := $(shell mysql_config --libs)
LDFLAGS=-L$(GSOAP_LIB) $(MYSQL_LDFLAGS)
LDLIBS=-lgsoap $(SSL_LIBS)

SSL_LIBS=-lssl -lcrypto
SSL_FLAGS=-DWITH_OPENSSL

# -L$(CLIENT_OBJ_DIR) -L$(SERVER_OBJ_DIR) -L$(COMMON_OBJ_DIR) -L$(RPC_OBJ_DIR)
# $(RPC_OBJ_DIR)/*.o


all: $(CLIENT_TARGET) $(SERVER_TARGET)

$(CLIENT_TARGET): $(CHEADS) $(CLIENT_COBJS) | $(TEST_BIN_DIR)

$(SERVER_TARGET): $(CHEADS) $(SERVER_COBJS) | $(TEST_BIN_DIR)

$(CLIENT_COBJS): | $(TEST_OBJ_DIR)

$(SERVER_COBJS): | $(TEST_OBJ_DIR)

$(SERVER_TEST_OBJ_DIR)/%.o: %.c
	@echo $(ECHO_FLAGS) "\tCC" $@
	@gcc -c -o $@ $< $(SERVER_CFLAGS)

$(CLIENT_TEST_OBJ_DIR)/%.o: %.c
	@echo $(ECHO_FLAGS) "\tCC" $@
	@gcc -c -o $@ $< $(CLIENT_CFLAGS)

$(SERVER_TEST_BIN_DIR)/%:
	@echo $(ECHO_FLAGS) "\tLD " $@	
	@gcc -o $@ $(@:$(SERVER_TEST_BIN_DIR)/%=$(SERVER_TEST_OBJ_DIR)/%.o) $(COMMON_OBJ_DIR)/*.o $(SERVER_OBJ_DIR)/*.o $(SERVER_RPC_OBJS) $(LDFLAGS) $(SSL_FLAGS) $(LDLIBS)

$(CLIENT_TEST_BIN_DIR)/%:
	@echo $(ECHO_FLAGS) "\tLD " $@	
	@gcc -o $@ $(@:$(CLIENT_TEST_BIN_DIR)/%=$(CLIENT_TEST_OBJ_DIR)/%.o) $(COMMON_OBJ_DIR)/*.o $(CLIENT_OBJ_DIR)/*.o $(CLIENT_RPC_OBJS) $(LDFLAGS) $(SSL_FLAGS) $(LDLIBS)

#------------------------------------------------------------------------------------
#		Directories 
#------------------------------------------------------------------------------------
$(TEST_BIN_DIR):
	@echo $(ECHO_FLAGS) "\tCreating dir $(CLIENT_TEST_BIN_DIR)"
	@mkdir -p $(CLIENT_TEST_BIN_DIR)
	@echo $(ECHO_FLAGS) "\tCreating dir $(SERVER_TEST_BIN_DIR)"
	@mkdir -p $(SERVER_TEST_BIN_DIR)

$(TEST_OBJ_DIR):
	@echo $(ECHO_FLAGS) "\tCreating dir $(CLIENT_TEST_OBJ_DIR)"
	@mkdir -p $(CLIENT_TEST_OBJ_DIR)
	@echo $(ECHO_FLAGS) "\tCreating dir $(SERVER_TEST_OBJ_DIR)"
	@mkdir -p $(SERVER_TEST_OBJ_DIR)

clean:
	@rm -rf *.o

