SOURCES=leak_detector_c.c list.c
HEADERS=bool.h leak_detector_c.h debug_def.h list.h

COBJS=$(SOURCES:%.c=$(OBJ_DIR)/%.o)
CHEADS=$(HEADERS)

ECHO_FLAGS=

#-------------------------------------------------------------
# Directories
#-------------------------------------------------------------
PROJECT_DIR=../..
SUBPROJECT=common
SRC_DIR=$(PROJECT_DIR)/src/$(SUBPROJECT)
OBJ_DIR=$(PROJECT_DIR)/build/$(SUBPROJECT)



#-------------------------------------------------------------
# Configuration
#-------------------------------------------------------------
CFLAGS=
LDFLAGS=
LDLIBS=

SSL_LIBS=-lssl -lcrypto
SSL_FLAGS=-DWITH_OPENSSL

#gcc $(SSL_FLAGS) -o $(CLIENT) $(CLIENT_SRC) -I$(GSOAP_INCLUDE) -lgsoap $(SSL_LIBS) -L$(GSOAP_LIB)

all: $(COBJS)


$(OBJ_DIR)/%.o : %.c
	@echo $(ECHO_FLAGS) "\tCC" $@
	@gcc -c -o $@ $< $(CFLAGS) $(EXTRA_CFLAGS)


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

