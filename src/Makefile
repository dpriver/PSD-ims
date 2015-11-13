
RPC_INTERFACE=ims.h

ECHO_FLAGS=

#-------------------------------------------------------------
# Directories
#-------------------------------------------------------------
PROJECT_DIR=..
SRC_DIR=$(PROJECT_DIR)/src
CLIENT_DIR=$(SRC_DIR)/client
SERVER_DIR=$(SRC_DIR)/server
COMMON_DIR=$(SRC_DIR)/common


#-------------------------------------------------------------
# Configuration
#-------------------------------------------------------------

all: common client server

rpc: rpc_client rpc_server

rpc_client:
	@echo $(ECHO_FLAGS) "\tSOAP" client
	@soapcpp2 -c -C -d$(CLIENT_DIR)/ $(RPC_INTERFACE)

rpc_server:
	@echo $(ECHO_FLAGS) "\tSOAP" server
	@soapcpp2 -c -S -d$(SERVER_DIR)/ $(RPC_INTERFACE)

common: common_message
	@$(MAKE) -C $(COMMON_DIR) all	

client: client_message rpc_client	
	@$(MAKE) -C $(CLIENT_DIR) all
	@echo $(ECHO_FLAGS) "\t====== CLIENT built succesfully ======\n"

server: server_message rpc_server
	@$(MAKE) -C $(SERVER_DIR) all
	@echo $(ECHO_FLAGS) "\t====== SERVER built succesfully ======\n"

common_message:
	@echo $(ECHO_FLAGS) "\n\t====== Building common files ======"

client_message:
	@echo $(ECHO_FLAGS) "\n\t====== Building CLIENT ======"

server_message:
	@echo $(ECHO_FLAGS) "\n\t====== Building SERVER ======"

#-------------------------------------------------------------
# Clean
#-------------------------------------------------------------

clean_all: clean_client clean_server

clean_client:
	@$(MAKE) -C $(CLIENT_DIR) clean

clean_server:
	@$(MAKE) -C $(SERVER_DIR) clean
