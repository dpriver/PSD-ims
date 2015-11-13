# main Makefile


help:
	@echo "================================= RULES ================================================"
	@echo " - help			-> prints this help"
	@echo " - all			-> compile everything (client and server)"
	@echo "========================================================================================"


all: client server


rpc:
	@$(MAKE) -C src rpc

client:
	@$(MAKE) -C src client

server:
	@$(MAKE) -C src server

clean:
	@$(MAKE) -C src clean_all
	@rm -rf bin/
	@rm -rf build/
