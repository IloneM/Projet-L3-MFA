# ----------------------------------------------------------------------
# Where........: CMAP - Polytechnique
# File.........: Makefile
# Author.......: Marc Schoenauer
# Created......: Tue Mar 21 12:52:19 1995
# Description..: Makefile pour le repertoire Util
#
# Ident........: $Id: Makefile,v 1.5 1998/03/20 06:51:36 marc Exp $
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
# DEBUG_OPT =
DEBUG_OPT = -g


#--- Target principale: la librairie ---
NAME    = util
LIBRARY = lib$(NAME).a

#--- Les sources C++ ---
SOURCES_C = lecture.C myrand.C parametre.C mystring.C pile.C

#--- Les sources C ---
SOURCES_c = mystdio.c pipecom.c

#--- Les entetes ---
HEADERS_C = erreur.h lecture.h mystring.h pile.h
HEADERS_c = erreur_msg.h mystdio.h pipecom.h myrand.h parametre.h


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
	$(TAR) czvf $(TARFILE) $(SOURCES) $(INCLUDE) Makefile

# list des dependances

lecture.o: mystdio.h lecture.h
myrand.o: myrand.h
parametre.o: parametre.h mystdio.h lecture.h mystring.h
mystdio.o: mystdio.h erreur.h erreur_msg.h
pipecom.o: pipecom.h
mystring.o: mystring.h
