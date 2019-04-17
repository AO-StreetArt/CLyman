# Makefile is defined to call make within the model subdirectories

.PHONY: mkfactory mklist mkobject mkproperty

all: mkproperty mkobject mklist mkfactory

mkfactory:
	@$(MAKE) -C factory

mklist:
	@$(MAKE) -C list

mkobject:
	@$(MAKE) -C object

mkproperty:
	@$(MAKE) -C property
