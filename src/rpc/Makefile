CLIENT_SOURCES=soapC.c soapClient.c
SERVER_SOURCES=soapC.c soapServer.c
HEADERS=soapH.h
RPC_INTERFACE=ims.h

CLIENT_COBJS=$(CLIENT_SOURCES:%.c=$(OBJ_DIR)/%.o)
SERVER_COBJS=$(SERVER_SOURCES:%.c=$(OBJ_DIR)/%.o)
CHEADS=$(HEADERS)

ECHO_FLAGS=

#-------------------------------------------------------------
# Directories
#-------------------------------------------------------------
PROJECT_DIR=../..
SUBPROJECT=rpc
SRC_DIR=$(PROJECT_DIR)/src/$(SUBPROJECT)
OBJ_DIR=$(PROJECT_DIR)/build/$(SUBPROJECT)



#-------------------------------------------------------------
# Configuration
#-------------------------------------------------------------
GSOAP_IMPORTS=gsoap/import

CFLAGS=-I$(GSOAP_INCLUDE)
LDFLAGS=-L$(GSOAP_LIB)
LDLIBS=-lgsoap $(SSL_LIBS)

SSL_LIBS=-lssl -lcrypto
SSL_FLAGS=-DWITH_OPENSSL

#gcc $(SSL_FLAGS) -o $(CLIENT) $(CLIENT_SRC) -I$(GSOAP_INCLUDE) -lgsoap $(SSL_LIBS) -L$(GSOAP_LIB)

all: $(CLIENT_COBJS) $(SERVER_COBJS)

generate:
	@echo $(ECHO_FLAGS) "\tSOAP" client server
	@soapcpp2 -c -d$(SRC_DIR)/ -Igsoap/import:gsoap/custom:. $(RPC_INTERFACE)

$(OBJ_DIR)/%.o : %.c | $(OBJ_DIR)
	@echo $(ECHO_FLAGS) "\tCC" $@
	@gcc -c -o $@ $< $(CFLAGS)


$(CLIENT_COBJS): | $(OBJ_DIR)

$(SERVER_COBJS): | $(OBJ_DIR)

clean:
	@rm -rf *.xml *.nsmap *.wsdl *.xsd
	@rm -rf soap*

#------------------------------------------------------------------------------------
#		Directories 
#------------------------------------------------------------------------------------

$(OBJ_DIR):
	@echo $(ECHO_FLAGS) "\tCreating dir $(OBJ_DIR)"
	@mkdir -p $(OBJ_DIR)

