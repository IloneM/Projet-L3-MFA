# ----------------------------------------------------------------------
# Where........: CMAP - Polytechnique
# File.........: Makefile
# Author.......: Marc Schoenauer
# Created......: Tue Mar 21 12:52:19 1995
# Description..: Makefile pour le repertoire RAL
#
# Ident........: $Id: Makefile,v 1.6 1998/03/20 06:49:36 marc Exp $
# ----------------------------------------------------------------------

include ../Make.in
#
# make.in definit, par machine,
#         CXX_DIR
#         les FLAGS
#         LM (-lm ou rien, pour l'edition de liens)
#
#
# ATTENTION, BUG si on compile avec le chemin total !!!!!
#########################################################
# Pour enlever le debug
DEBUG_OPT = -g


#--- Target principale: la librairie ---
NAME    = ral
LIBRARY = lib$(NAME).a

#--- Les sources C++ ---
SOURCES_C = popu.C chromosome.C genutil.C init_param.C sel.C replace.C evoschem.C appli.C 
#--- Les sources C ---
SOURCES_c =

#--- Les entetes ---
HEADERS_C = chromosome.h sel.h genutil.h popu.h evoschem.h appli.h replace.h
HEADERS_c =


#--- Macros generiques ---
SOURCES = $(SOURCES_C) $(SOURCES_c)
OBJETS = $(SOURCES_C:.C=.o) $(SOURCES_c:.c=.o)
INCLUDE = $(HEADERS_C) $(HEADERS_c)

#--- Regles de construction ---
# cible principale: la librairie
tout : $(LIBRARY)

$(LIBRARY) : $(OBJETS)
	ar ru $@ $(OBJETS)

clean :
	/bin/rm -f $(OBJETS) $(LIBRARY)

TARFILE = $(NAME).tgz

tar : $(TARFILE)

$(TARFILE): $(SOURCES) $(INCLUDE) Makefile
	$(TAR) czvf $(TARFILE) $(SOURCES) $(INCLUDE) Makefile samplemain.C

# list des dependances

popu.o: chromosome.h popu.h $(UTIL_DIR)/mystring.h $(UTIL_DIR)/pipecom.h 
popu.o : $(UTIL_DIR)/parametre.h $(UTIL_DIR)/myrand.h appli.h evoschem.h
chromosome.o: chromosome.h 
genutil.o: $(UTIL_DIR)/myrand.h genutil.h
init_param.o: chromosome.h popu.h $(UTIL_DIR)/mystring.h $(UTIL_DIR)/pipecom.h 
init_param.o : $(UTIL_DIR)/parametre.h sel.h $(UTIL_DIR)/myrand.h
sel.o: sel.h chromosome.h popu.h $(UTIL_DIR)/mystring.h $(UTIL_DIR)/pipecom.h 
sel.o : appli.h $(UTIL_DIR)/parametre.h $(UTIL_DIR)/myrand.h 
appli.o: appli.h $(UTIL_DIR)/parametre.h $(UTIL_DIR)/mystring.h
appli.o: $(UTIL_DIR)/myrand.h
evoschem.o : popu.h chromosome.h evoschem.h sel.h replace.h appli.h
evoschem.o : $(UTIL_DIR)/myrand.h $(UTIL_DIR)/parametre.h
replace.o : replace.h chromosome.h appli.h $(UTIL_DIR)/myrand.h $(UTIL_DIR)/parametre.h
