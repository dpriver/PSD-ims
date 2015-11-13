# main Makefile


help:
	@echo "================================= RULES ================================================"
	@echo " - help			-> prints this help"
	@echo " - all			-> compile everything (client and server)"
	@echo "========================================================================================"

force_all: clean rpc_regenerate all_debug tests

all: common rpc client server

all_debug: common_debug rpc client_debug server_debug

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

common_debug:
	@$(MAKE) -C src common_debug

client_debug:
	@$(MAKE) -C src client_debug

server_debug:
	@$(MAKE) -C src server_debug

client_:
	@$(MAKE) -C src client

server_:
	@$(MAKE) -C src server

clean:
	@$(MAKE) -C src clean_all
	@rm -rf bin/
	@rm -rf build/
