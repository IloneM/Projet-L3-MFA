# Fichier  : Makefile
# Auteur   : Nilo Schwencke d'apres Florent Hivert
# A propos : L3 MFA S6-Projet (bipop cma-es)
#
# Date     : mar. mars 24 19:10:54 CET 2015
#######################################################
# Description des differents modules
#

SHELL = /bin/sh
CPPFLAGS = -std=gnu++11 -fopenmp -I/usr/include/eigen3/ -I/usr/local/include/libcmaes/ -L/usr/local/lib/
LDFLAGS = -lcmaes
CCCP = g++

#### Fichiers ####
CPP_FILES = $(wildcard *.cpp)
HPP_FILES = $(wildcard *.hpp)
O_FILES = $(SRCS:.cpp=.o)
SRCS = $(CPP_FILES) $(HPP_FILES)
MAIN_FILE = main
EXEC = rp_cma

#### Dépendances ####
all: $(MAIN_FILE)

%.o: %.cpp
	$(CCCP) $(CPPFLAGS) -o $@ -c $< $(LDFLAGS)

#rp_cma.o      : rp_cma.cpp
$(MAIN_FILE) : $(O_FILES)
	@$(CCCP) $(CPPFLAGS) $(O_FILES) $(MAIN_FILE).cpp -o $(EXEC) $(LDFLAGS) 

#### Cibles diverses ####
clean:
	@rm $(O_FILES)

mrproper: clean
	@rm $(EXEC)

