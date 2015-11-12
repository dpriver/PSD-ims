# main Makefile


help:
	@echo -e "========= RULES ========================================================================"
	@echo -e "- help			-> prints this help"
	@echo -e "- all			-> compile everything (client and server)"



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
