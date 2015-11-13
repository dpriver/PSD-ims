# main Makefile


help:
	@echo "================================= RULES ================================================"
	@echo " - help			-> prints this help"
	@echo " - all			-> compile everything (client and server)"
	@echo "========================================================================================"


all: common client_ server_ tests


rpc:
	@$(MAKE) -C src rpc

common:
	@$(MAKE) -C src common

client: common
	@$(MAKE) -C src client

server: common
	@$(MAKE) -C src server

client_:
	@$(MAKE) -C src client

server_:
	@$(MAKE) -C src server

tests:
	@$(MAKE) -C src tests

clean:
	@$(MAKE) -C src clean_all
	@rm -rf bin/
	@rm -rf build/
