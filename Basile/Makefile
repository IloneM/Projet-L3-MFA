
include ../Make.in

# Pour enlever le debug
DEBUG_OPT = -g

#DEBUG_OPT = -g

#--- Target principale: le programme ---
NAME    = Roger 

PROGRAM = $(NAME)

#--- Les sources C++ ---
SOURCES_C = main.C reel.C perf.C
#--- Les sources C ---
SOURCES_c =

#--- Les headers ---
HEADERS_C = reel.h
HEADERS_c = 

ADD_LIBS = $(RAL_LIB) $(UTIL_LIB) 
INC_MACRO = -I$(RAL_DIR) -I$(UTIL_DIR) 

#--- Macros generiques ---
SOURCES = $(SOURCES_C) $(SOURCES_c)
OBJETS = $(SOURCES_C:.C=.o) $(SOURCES_c:.c=.o)
OBJETS_LOG = $(SOURCES_LOG:.C=.o)
OBJETS_SIHC = $(SOURCES_SIHC:.C=.o)
OBJETS_TEST = $(SOURCES_TEST:.C=.o)
INCLUDE = $(HEADERS_C) $(HEADERS_c)

#--- Regles de construction ---
# cible principale: le programme
tout : $(PROGRAM)

$(PROGRAM) : $(ADD_LIBS) $(OBJETS)
	$(CXX) $(DEBUG_OPT) $(OBJETS) $(ADD_LIBS) -o $@ -lm

force:
########### depouille

clean :
	/bin/rm -f $(OBJETS) $(PROGRAM)

TARFILE = Roger.tgz

tar :  $(TARFILE)

$(TARFILE) : $(SOURCES) $(INCLUDE) Makefile *.pl sc*	;
	$(TAR) czvf $(TARFILE) *.h *.C *.param *.pl sc* Makefile


#--- Cible nulle, pour forcer les entrees ---
force:

###
reel.o: reel.h 
reel.o: $(RAL_DIR)/popu.h $(RAL_DIR)/sel.h 
reel.o: $(RAL_DIR)/chromosome.h  $(RAL_DIR)/genutil.h
reel.o: $(UTIL_DIR)/myrand.h $(UTIL_DIR)/parametre.h 
bit_std.o: reel.h 
bit_std.o: $(RAL_DIR)/popu.h $(RAL_DIR)/sel.h 
bit_std.o: $(RAL_DIR)/chromosome.h  $(RAL_DIR)/genutil.h
bit_std.o: $(UTIL_DIR)/myrand.h $(UTIL_DIR)/parametre.h 
main.o: reel.h 
main.o: $(RAL_DIR)/popu.h $(RAL_DIR)/sel.h 
main.o: $(RAL_DIR)/chromosome.h  $(RAL_DIR)/genutil.h
main.o: $(UTIL_DIR)/myrand.h $(UTIL_DIR)/parametre.h 
perf.o: reel.h 
perf.o: $(RAL_DIR)/popu.h $(RAL_DIR)/sel.h 
perf.o: $(RAL_DIR)/chromosome.h  $(RAL_DIR)/genutil.h
perf.o: $(UTIL_DIR)/myrand.h $(UTIL_DIR)/parametre.h 

