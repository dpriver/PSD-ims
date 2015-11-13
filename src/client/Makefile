EXECUTABLE=client
SOURCES=soapC.c soapClient.c chats.c client.c friends.c messages.c persistence.c
HEADERS=soapH.h chats.h friends.h messages.h persistence.h

COMMON_LIBS=*

TARGET=$(EXECUTABLE:%=$(BIN_DIR)/%)
COBJS=$(SOURCES:%.c=$(OBJ_DIR)/%.o)
COMMON_OBJS=$(COMMON_LIBS:%=$(OBJ_COMMON_DIR)/%.o)
CHEADS=$(HEADERS)

ECHO_FLAGS=

#-------------------------------------------------------------
# Directories
#-------------------------------------------------------------
PROJECT_DIR=../..
SUBPROJECT=client
SRC_COMMON_DIR=$(PROJECT_DIR)/src/common
SRC_DIR=$(PROJECT_DIR)/src/$(SUBPROJECT)
OBJ_DIR=$(PROJECT_DIR)/build/$(SUBPROJECT)
OBJ_COMMON_DIR=$(PROJECT_DIR)/build/common
BIN_DIR=$(PROJECT_DIR)/bin



#-------------------------------------------------------------
# Configuration
#-------------------------------------------------------------
CFLAGS=-I$(SRC_COMMON_DIR) -I$(GSOAP_INCLUDE)
LDFLAGS=-L$(GSOAP_LIB)
LDLIBS=-lgsoap $(SSL_LIBS)

SSL_LIBS=-lssl -lcrypto
SSL_FLAGS=-DWITH_OPENSSL

#gcc $(SSL_FLAGS) -o $(CLIENT) $(CLIENT_SRC) -I$(GSOAP_INCLUDE) -lgsoap $(SSL_LIBS) -L$(GSOAP_LIB)

all: $(TARGET)

$(TARGET): $(BIN_DIR) $(COMMON_OBJS) $(COBJS)
	@echo $(ECHO_FLAGS) "\tLD " $(TARGET)	
	@gcc -o $@ $(COBJS) $(COMMON_OBJS) $(LDFLAGS) $(SSL_FLAGS) $(LDLIBS)


$(OBJ_DIR)/%.o : %.c
	@echo $(ECHO_FLAGS) "\tCC" $@
	@gcc -c -o $@ $< $(CFLAGS)


$(COBJS): | $(OBJ_DIR)

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

