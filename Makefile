# main Makefile


all: clean rpc_regenerate rpc common client server

help:
	@echo "================================= RULES ================================================"
	@echo " - help			-> prints this help"
	@echo " - all			-> compile everything (client and server)"
	@echo "========================================================================================"


force_all: force_debug

force_debug: clean rpc_regenerate all_debug tests

force_trace: clean rpc_regenerate common_debug rpc client_debug server_debug_trace tests

all_debug: common_debug rpc client_debug server_debug

all_debug_trace: common_debug_trace rpc client_debug_trace server_debug_trace

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

common_debug_trace:
	@$(MAKE) -C src common_debug_trace

client_debug_trace:
	@$(MAKE) -C src client_debug_trace

server_debug_trace:
	@$(MAKE) -C src server_debug_trace

client_:
	@$(MAKE) -C src client

server_:
	@$(MAKE) -C src server

clean:
	@$(MAKE) -C src clean_all
	@rm -rf bin/
	@rm -rf build/
