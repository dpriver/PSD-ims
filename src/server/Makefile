EXECUTABLE=server
SOURCES=soapC.c soapServer.c server.c persistence.c
HEADERS=soapH.h persistence.h

TARGET=$(EXECUTABLE:%=$(BIN_DIR)/%)
COBJS=$(SOURCES:%.c=%.o)
CHEADS=$(HEADERS)

#-------------------------------------------------------------
# Directories
#-------------------------------------------------------------
PROJECT_DIR=../..
SRC_DIR=$(PROJECT_DIR)/src/server
BIN_DIR=$(PROJECT_DIR)/bin
HEAD_DIR=$(PROJECT_DIR)/src/common_include



#-------------------------------------------------------------
# Configuration
#-------------------------------------------------------------
CFLAGS=-I$(GSOAP_INCLUDE) -I$(HEAD_DIR)
LDFLAGS=-L$(GSOAP_LIB)
LDLIBS=-lgsoap $(SSL_LIBS)

SSL_LIBS=-lssl -lcrypto
SSL_FLAGS=-DWITH_OPENSSL

#gcc $(SSL_FLAGS) -o $(CLIENT) $(CLIENT_SRC) -I$(GSOAP_INCLUDE) -lgsoap $(SSL_LIBS) -L$(GSOAP_LIB)

all: $(BIN_DIR) $(TARGET)

$(TARGET): $(SOURCES)
	gcc -o $@ $(SOURCES) $(CFLAGS) $(LDFLAGS) $(SSL_FLAGS) $(LDLIBS)


clean:
	@rm -rf *.xml *.nsmap *.wsdl *.xsd
	@rm -rf soap*

#------------------------------------------------------------------------------------
#		Directories 
#------------------------------------------------------------------------------------

$(BIN_DIR):
	@echo -e "creating dir $(BIN_DIR)"
	@mkdir -p $(BIN_DIR)

