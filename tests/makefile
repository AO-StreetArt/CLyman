# Makefile is defined to call make within the app, api, and model directories

.PHONY: mkapp mkapi mkmodel

all: mkapp mkapi mkmodel

mkapp:
	@$(MAKE) -C app

mkapi:
	@$(MAKE) -C api

mkmodel:
	@$(MAKE) -C model
