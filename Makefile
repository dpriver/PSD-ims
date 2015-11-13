# main Makefile


help:
	@echo "================================= RULES ================================================"
	@echo " - help			-> prints this help"
	@echo " - all			-> compile everything (client and server)"
	@echo "========================================================================================"

force_all: clean rpc_regenerate all

all: common rpc client server

rpc_regenerate:
	@$(MAKE) -C src rpc_generate

rpc:
	@$(MAKE) -C src rpc

common:
	@$(MAKE) -C src common

client:
	@$(MAKE) -C src client

server:
	@$(MAKE) -C src server

tests:
	@$(MAKE) -C src tests

client_:
	@$(MAKE) -C src client

server_:
	@$(MAKE) -C src server

clean:
	@$(MAKE) -C src clean_all
	@rm -rf bin/
	@rm -rf build/
