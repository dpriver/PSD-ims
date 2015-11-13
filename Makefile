# main Makefile


help:
	@echo "================================= RULES ================================================"
	@echo " - help			-> prints this help"
	@echo " - all			-> compile everything (client and server)"
	@echo "========================================================================================"


all: common client server


rpc:
	@$(MAKE) -C src rpc

common:
	@$(MAKE) -C src common

client:
	@$(MAKE) -C src client

server:
	@$(MAKE) -C src server

clean:
	@$(MAKE) -C src clean_all
	@rm -rf bin/
	@rm -rf build/
