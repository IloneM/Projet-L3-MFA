# Fichier  : Makefile
# Auteur   : Nilo Schwencke d'apres Florent Hivert
# A propos : L3 MFA S6-Projet (rp cma-es)
#
# Date     : mar. mars 24 19:10:54 CET 2015
#######################################################
# Description des differents modules
#

### DEFAULT ###

#### Extensions ####
CPP_EXTENSIONS = cc cp cxx cpp CPP c++ C
HPP_EXTENSIONS = hh H hp hxx hpp HPP h++ tcc
#### Debug ####
DEBUG = -1
#### Flags ####
CPPFLAGS =
LDFLAGS =
#### Executable ####
EXEC = exec

#project specific settings must be defined in settings.mk
-include settings.mk

#### Binaries ####
SHELL = /bin/sh
CCCP = g++
RM = rm -f

#### Files ####
CPP_FILES = $(foreach ext,$(CPP_EXTENSIONS),$(wildcard *.$(ext)))
HPP_FILES = $(foreach ext,$(HPP_EXTENSIONS),$(wildcard *.$(ext)))
O_FILES = $(filter %.o,$(foreach ext,$(CPP_EXTENSIONS),$(CPP_FILES:.$(ext)=.$(ext).o)))
SRCS = $(strip $(CPP_FILES) $(HPP_FILES))
DEPS = $(O_FILES:.o=.d)

#### Settings computation ####
ifeq "$(DEBUG)" "2"
CPPFLAGS += -g3 -O0
else ifeq "$(DEBUG)" "1"
CPPFLAGS += -pg #profile
else ifeq "$(DEBUG)" "0"
CPPFLAGS += -O2
endif

#### Dependencies ####
all: $(EXEC)

#function to create o targets
define ofile
$(1): $(patsubst %.o,%,$(1))
	@$(CCCP) $(CPPFLAGS) -o $(1) -c -MP -MMD $(patsubst %.o,%,$(1)) $(LDFLAGS)
endef

#creates the o targets from the O_FILES variable
$(foreach file,$(O_FILES),$(eval $(call ofile,$(file))))

$(EXEC): $(O_FILES) 
	@$(CCCP) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

#### Miscellaneous targets ####
clean:
	@$(RM) $(O_FILES) $(DEPS)

mrproper: clean
	@$(RM) $(EXEC)

rebuild: mrproper all

.PHONY: clean

-include $(DEPS)

