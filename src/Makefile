
RPC_INTERFACE=ims.h

#-------------------------------------------------------------
# Directories
#-------------------------------------------------------------
PROJECT_DIR=..
SRC_DIR=$(PROJECT_DIR)/src
CLIENT_DIR=$(SRC_DIR)/client
SERVER_DIR=$(SRC_DIR)/server


#-------------------------------------------------------------
# Configuration
#-------------------------------------------------------------

all: client server

rpc_client:
	soapcpp2 -c -C -d$(CLIENT_DIR)/ $(RPC_INTERFACE)

rpc_server:
	soapcpp2 -c -S -d$(SERVER_DIR)/ $(RPC_INTERFACE)	

client: rpc_client	
	@$(MAKE) -C $(CLIENT_DIR) all

server: rpc_server
	@$(MAKE) -C $(SERVER_DIR) all


#-------------------------------------------------------------
# Clean
#-------------------------------------------------------------

clean_all: clean_client clean_server

clean_client:
	@$(MAKE) -C $(CLIENT_DIR) clean

clean_server:
	@$(MAKE) -C $(SERVER_DIR) clean
