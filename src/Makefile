CLIENT=client
SERVER=server

CLIENT_SRC=client.c soapC.c soapClient.c
SERVER_SRC=server.c soapC.c soapServer.c

RPC_INTERFACE=ims.h

SSL_LIBS=-lssl -lcrypto
SSL_FLAGS=-DWITH_OPENSSL

all:	rpc client server

rpc:
	soapcpp2 -c $(RPC_INTERFACE) 

client:	
	gcc $(SSL_FLAGS) -o $(CLIENT) $(CLIENT_SRC) -I$(GSOAP_INCLUDE) -lgsoap $(SSL_LIBS) -L$(GSOAP_LIB)

server:	
	gcc $(SSL_FLAGS) -o $(SERVER) $(SERVER_SRC) -I$(GSOAP_INCLUDE) -lgsoap $(SSL_LIBS) -L$(GSOAP_LIB)

clean:	
	@rm -f client server
	@rm -f soapStub.h soapServerLib.c soapH.h soapServer.c soapClientLib.c soapClient.c soapC.c
	@rm -f *.xml *.nsmap *.wsdl *.xsd
