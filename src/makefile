# Makefile is defined to call make within the app, api, and model directories

.PHONY: mkapp mkapi mkmodel

all: mkmodel mkapi mkapp

mkapp:
	@$(MAKE) -C app

mkapi:
	@$(MAKE) -C api

mkmodel:
	@$(MAKE) -C model
