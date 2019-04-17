# Makefile is defined to call make within the app, api, and model directories

.PHONY: mkmodel mkproc

all: mkmodel mkdb

mkmodel:
	@$(MAKE) -C model

mkdb:
	@$(MAKE) -C db
