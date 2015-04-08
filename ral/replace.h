// -*-C++-*--------------------------------------------------------------
// Where........: CMAP - Polytechnique 
// File.........: replace.h
// Author.......: Marc Schoenauer
// Created......: 6 mai 95
// Description..: Definition des remplaceurs
// 
// Ident........: $Id: replace.h,v 1.1 1996/05/30 17:29:38 marc Exp $
// ----------------------------------------------------------------------

#include "chromosome.h"
class ostream;
class popu;

class remplaceur 
{
protected :
  int taille_pop ;
  int *resultats;
  int taille_res;
  // methodes privees
void verif(int, int, int, int);// verif de taille et remplissage de resultats avec val
public :
remplaceur(int t);
virtual ~remplaceur();	// pour heritage !
virtual int* trouve_les_gagnants(popu*, int, int, int)=0;
virtual int* trouve_les_perdants(popu*, int, int, int)=0;
virtual int le_perdant(popu*, int, int)=0; // pour ssga essentiellement
};

remplaceur *choix_remp( char* NomRemp, int taille_pop );


//////////////////////////////////////////
class tournoiR : public remplaceur {
private :
int taille_tournoi ;	// la taille du tournoi
int* restants;			// meme taille que resultats (taille_res)
int taille_restants;
public :
tournoiR(int t);
tournoiR(int, int);
virtual int* trouve_les_gagnants(popu*, int, int, int);
virtual int* trouve_les_perdants(popu*, int, int, int);
virtual int le_perdant(popu*, int, int); // pour ssga essentiellement
};

/////////////////////////////////////////
class rep_determin : public remplaceur {
public :
rep_determin(int t):remplaceur(t){}
virtual int* trouve_les_gagnants(popu*, int, int, int);
virtual int* trouve_les_perdants(popu*, int, int, int);
virtual int le_perdant(popu*, int, int); // pour ssga essentiellement
};

/////////////////////////////////////////
class ep_tournoi : public remplaceur {
protected:
int taille_tournoi;
int * le_rang;
int* la_perf_ep;
int (*comp_score)(const void*, const void*);

void verif(int, int, int, int);// surcharge pour tester les tailles des protected
void trie_ep(popu* pop, int debut, int nb_chromos);

public :
ep_tournoi(int t);
ep_tournoi(int t, int tt);
int* trouve_les_gagnants(popu*, int, int, int);
virtual int* trouve_les_perdants(popu*, int, int, int);
virtual int le_perdant(popu*, int, int); // pour ssga essentiellement
};
