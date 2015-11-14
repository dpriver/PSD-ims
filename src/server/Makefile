MAIN_SRC=server.c
SOURCES=
HEADERS=persistence.h

COMMON_LIBS=*
RPC_LIBS=soapC soapServer

TARGET=$(MAIN_SRC:%.c=$(BIN_DIR)/%)
TEST_LIB=$(MAIN_SRC:%.c=$(OBJ_DIR)/lib%.a)
MAIN_OBJ=$(MAIN_SRC:%.c=$(OBJ_DIR)/%.o)
COBJS=$(SOURCES:%.c=$(OBJ_DIR)/%.o)
COMMON_OBJS=$(COMMON_LIBS:%=$(OBJ_COMMON_DIR)/%.o)
RPC_OBJS=$(RPC_LIBS:%=$(OBJ_RPC_DIR)/%.o)
CHEADS=$(HEADERS)

ECHO_FLAGS=

#-------------------------------------------------------------
# Directories
#-------------------------------------------------------------
PROJECT_DIR=../..
SUBPROJECT=server
SRC_DIR=$(PROJECT_DIR)/src/$(SUBPROJECT)
SRC_COMMON_DIR=$(PROJECT_DIR)/src/common
SRC_RPC_DIR=$(PROJECT_DIR)/src/rpc
OBJ_DIR=$(PROJECT_DIR)/build/$(SUBPROJECT)
OBJ_COMMON_DIR=$(PROJECT_DIR)/build/common
OBJ_RPC_DIR=$(PROJECT_DIR)/build/rpc
BIN_DIR=$(PROJECT_DIR)/bin



#-------------------------------------------------------------
# Configuration
#-------------------------------------------------------------
CFLAGS=-I$(SRC_COMMON_DIR) -I$(SRC_RPC_DIR) -I$(GSOAP_INCLUDE) $(mysql_config --cflags)
LDFLAGS=-L$(GSOAP_LIB) $(mysql_config --libs)
LDLIBS=-lgsoap $(SSL_LIBS)

SSL_LIBS=-lssl -lcrypto
SSL_FLAGS=-DWITH_OPENSSL

#gcc $(SSL_FLAGS) -o $(CLIENT) $(CLIENT_SRC) -I$(GSOAP_INCLUDE) -lgsoap $(SSL_LIBS) -L$(GSOAP_LIB)

all: $(TARGET)

$(TARGET): $(BIN_DIR) $(OBJ_DIR) $(COMMON_OBJS) $(COBJS) $(MAIN_OBJ)
	@ar rcs $(TEST_LIB) $(COBJS)
	@echo $(ECHO_FLAGS) "\tLD " $(TARGET)	
	@gcc -o $@ $(MAIN_OBJ) $(COBJS) $(COMMON_OBJS) $(RPC_OBJS) $(LDFLAGS) $(SSL_FLAGS) $(LDLIBS)
	

$(OBJ_DIR)/%.o : %.c
	@echo $(ECHO_FLAGS) "\tCC" $@
	@gcc -c -o $@ $< $(CFLAGS) $(EXTRA_CFLAGS)

$(COBJS): | $(OBJ_DIR)

$(TARGET): | $(BIN_DIR)

clean:
	@rm -rf *.xml *.nsmap *.wsdl *.xsd
	@rm -rf soap*

#------------------------------------------------------------------------------------
#		Directories 
#------------------------------------------------------------------------------------

$(OBJ_DIR):
	@echo $(ECHO_FLAGS) "\tCreating dir $(OBJ_DIR)"
	@mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	@echo $(ECHO_FLAGS) "\tCreating dir $(BIN_DIR)"
	@mkdir -p $(BIN_DIR)

