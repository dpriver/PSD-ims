

ECHO_FLAGS=

#-------------------------------------------------------------
# Directories
#-------------------------------------------------------------
PROJECT_DIR=..
SRC_DIR=$(PROJECT_DIR)/src
CLIENT_DIR=$(SRC_DIR)/client
SERVER_DIR=$(SRC_DIR)/server
COMMON_DIR=$(SRC_DIR)/common
RPC_DIR=$(SRC_DIR)/rpc
TEST_DIR=$(SRC_DIR)/test


#-------------------------------------------------------------
# Configuration
#-------------------------------------------------------------

all: rpc common client server

rpc_generate: clean_rpc
	@$(MAKE) -C $(RPC_DIR) generate

rpc: rpc_message
	@$(MAKE) -C $(RPC_DIR) all
	@echo $(ECHO_FLAGS) "\t====== RPC built succesfully ======\n"

common: common_message
	@$(MAKE) -C $(COMMON_DIR) all
	@echo $(ECHO_FLAGS) "\t====== COMMON built succesfully ======\n"

client: client_message
	@$(MAKE) -C $(CLIENT_DIR) all
	@echo $(ECHO_FLAGS) "\t====== CLIENT built succesfully ======\n"

server: server_message
	@$(MAKE) -C $(SERVER_DIR) all
	@echo $(ECHO_FLAGS) "\t====== SERVER built succesfully ======\n"

tests: test_message
	@$(MAKE) -C $(TEST_DIR) all
	@echo $(ECHO_FLAGS) "\t====== TESTS built succesfully ======\n"

common_debug_trace: common_message
	@$(MAKE) -C $(COMMON_DIR) EXTRA_CFLAGS="-DDEBUG -DDEBUG_TRACE -DDEBUG_INFO -DDEBUG_FAILURE"  all
	@echo $(ECHO_FLAGS) "\t====== COMMON (DEBUG TRACE) built succesfully ======\n"

client_debug_trace: client_message
	@$(MAKE) -C $(CLIENT_DIR) EXTRA_CFLAGS="-DDEBUG -DDEBUG_TRACE -DDEBUG_INFO -DDEBUG_FAILURE" all
	@echo $(ECHO_FLAGS) "\t====== CLIENT (DEBUG TRACE) built succesfully ======\n"

server_debug_trace: server_message
	@$(MAKE) -C $(SERVER_DIR) EXTRA_CFLAGS="-DDEBUG -DDEBUG_TRACE -DDEBUG_INFO -DDEBUG_FAILURE"  all
	@echo $(ECHO_FLAGS) "\t====== SERVER (DEBUG TRACE) built succesfully ======\n"

common_debug: common_message
	@$(MAKE) -C $(COMMON_DIR) EXTRA_CFLAGS="-DDEBUG -DDEBUG_INFO -DDEBUG_FAILURE"  all
	@echo $(ECHO_FLAGS) "\t====== COMMON (DEBUG) built succesfully ======\n"

client_debug: client_message
	@$(MAKE) -C $(CLIENT_DIR) EXTRA_CFLAGS="-DDEBUG -DDEBUG_INFO -DDEBUG_FAILURE" all
	@echo $(ECHO_FLAGS) "\t====== CLIENT (DEBUG) built succesfully ======\n"

server_debug: server_message
	@$(MAKE) -C $(SERVER_DIR) EXTRA_CFLAGS="-DDEBUG -DDEBUG_INFO -DDEBUG_FAILURE"  all
	@echo $(ECHO_FLAGS) "\t====== SERVER (DEBUG) built succesfully ======\n"


#-------------------------------------------------------------
# Messages
#-------------------------------------------------------------

rpc_message:
	@echo $(ECHO_FLAGS) "\n\t====== Building rpc files ======"

common_message:
	@echo $(ECHO_FLAGS) "\n\t====== Building common files ======"

client_message:
	@echo $(ECHO_FLAGS) "\n\t====== Building CLIENT ======"

server_message:
	@echo $(ECHO_FLAGS) "\n\t====== Building SERVER ======"

test_message:
	@echo $(ECHO_FLAGS) "\n\t====== Building TESTS ======"


#-------------------------------------------------------------
# Clean
#-------------------------------------------------------------

clean_all: clean_rpc clean_common clean_client clean_server clean_tests

clean_client:
	@$(MAKE) -C $(CLIENT_DIR) clean

clean_server:
	@$(MAKE) -C $(SERVER_DIR) clean

clean_common:
	@$(MAKE) -C $(COMMON_DIR) clean

clean_tests:
	@$(MAKE) -C $(TEST_DIR) clean

clean_rpc:
	@$(MAKE) -C $(RPC_DIR) clean
