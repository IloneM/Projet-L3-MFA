# Fichier  : Makefile
# Auteur   : Nilo Schwencke d'apres Florent Hivert
# A propos : L3 MFA S6-Projet (bipop cma-es)
#
# Date     : mar. mars 24 19:10:54 CET 2015
#######################################################
# Description des differents modules
#

#/*
# * replace -o by -gl for debug
# */

SHELL = /bin/sh
CPPFLAGS = -std=gnu++11 -fopenmp -I/usr/include/eigen3/ -I/usr/local/include/libcmaes/ -L/usr/local/lib/
LDFLAGS = -lcmaes
CCCP = g++

#### Fichiers ####
CPP_FILES = $(wildcard *.cpp)
HPP_FILES = $(wildcard *.hpp *.h)
O_FILES = $(CPP_FILES:.cpp=.o)
SRCS = $(CPP_FILES) $(HPP_FILES)
EXEC = rpcma

#### Dépendances ####
all: $(EXEC)

main.o: $(HPP_FILES)
%.o: %.cpp
	@$(CCCP) $(CPPFLAGS) -o $@ -c $< $(LDFLAGS)

$(EXEC) : $(O_FILES)
	@$(CCCP) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

#### Cibles diverses ####
clean:
	@rm -f $(O_FILES)

mrproper: clean
	@rm -f $(EXEC)

