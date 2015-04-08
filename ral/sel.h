// -*-C++-*--------------------------------------------------------------
// Where........: CMAP - Polytechnique 
// File.........: sel.h
// Author.......: Marc Schoenauer
// Created......: Tue Mar 21 14:07:26 1995
// Description..: Definition des selecteurs:
//		  Selectionnent des individus de la population selon
//		  differents schemas
// 
// Ident........: $Id: sel.h,v 1.4 1996/05/30 17:28:00 marc Exp $
// ----------------------------------------------------------------------

#include "chromosome.h"
//class ostream;
class popu;

class selecteur 
{
protected :
   int taille_pop ;
   int taille_locale;	// le nbre d'offspring poru la generation courante
   int debut_local;	// debut ds les_chromos pour la selection courante
public :
selecteur(int t);		// avec lecture (eventuelle) des parametres
selecteur(selecteur* s);	// SANS ...
virtual ~selecteur();	// pour heritage !
virtual void prepare_select(popu*, int, int=0)=0;   
virtual int select()=0;	// renvoie le No du chrom. selecteurne
virtual selecteur* clone()=0;
};

selecteur *choix_sel( char* NomSel, int taille_pop );

//////////////////////////////////////////
// le trivial, au cas ou
//////////////////////////////////////////

class aleatoire : public selecteur 
// renvoie un chromosome au hasard (tirages independants)
{
public :
aleatoire(int taille):selecteur(taille){};
aleatoire(aleatoire *s):selecteur(s){};
virtual void prepare_select(popu*, int, int=0);
virtual int select();	// renvoie le No du chrom. selecteurne
virtual selecteur* clone();
};

//////////////////////////////////////////
// le selecteur des strategies d'evolution et evolutionary programming
//////////////////////////////////////////
// idem aleatoire, mais si on en tire exactement taille_pop, 
// on les a TOUS UNE FOIS exactement (ordre aleatoire).
class equireparti : public selecteur 
// renvoie un chromosome au hasard (tirages independants)
{
protected:
int index_local;
int *le_rang;
public :
equireparti(int taille):selecteur(taille){};
equireparti(equireparti* s):selecteur(s){};
~equireparti(){if (le_rang) delete [] le_rang;}
virtual void prepare_select(popu*, int, int=0);
virtual int select();	// renvoie le No du chrom. selecteurne
virtual selecteur* clone();
};


//////////////////////////////////////////
// Ceux qui gerent un tableau de psudo-fitness heritent de celui-la,
// qui factorise les diverses procedures de scaling
// Apres maj du tableau des fitness (specifique), 
// tirage ds ce tableau par 1 random + progression le long de
// la roulette  par intervalles reguliers (la_somme/nbre_a_selectionner)
// 
//////////////////////////////////////////
class proportionnel : public selecteur 
{
protected:
  double* fitness_tmp;	    // tableau des performances
  int taille_fitness_tmp;	// taille du precedent
  double pression;		// la pression selective
  int typ_scaling;		// type de mise a l'echalle
  double facteur_de_partage;	// pour sharing

  double la_somme;		// somme des fitness de fitness_tmp
  int *tab_select;			// tableau des selcetes (melange)
  int iselect;			// celui a rendre ds tab_select
public:
proportionnel(int taille);
proportionnel(proportionnel* s);
virtual ~proportionnel()
  {if (fitness_tmp) delete[] fitness_tmp;
  if (tab_select) delete[] tab_select;}
   
  //lese mise a l'echelle
void scale_lineaire();
void scale_exp();
void scale_dyn();
  // le sharing
void maj_simil(popu*, int, int);
void prepare_sharing(popu*, int, int);

virtual void prepare_select(popu*, int, int=0);
virtual void remplit_fitness_tmp(popu*, int, int=0)=0; // LE point specifique
void verif(int);
virtual int select();	       
};

// les definitions des types de mise a l'echalle
#define LINEAIRE 0
#define EXPONENTIEL 1
#define DYNAMIC 2

/////////////////////////////////////////
class rang : public proportionnel
{
protected :

public :
rang(int t):proportionnel(t){};
rang(rang* s):proportionnel(s){};
virtual void remplit_fitness_tmp(popu*, int, int=0); // LE point specifique
selecteur* clone();
};

/*

//////////////////////////////////////////
class roulette : proportionnel 
{
protected :

public :
   roulette(ParamList *plist):proportionnel(plist){};
   virtual ~roulette(){}
   
   virtual void prepare_select(popu*, int, int=0);
   
   virtual int select();	// renvoie le No du chrom. selecteurne

   
};
  

*/

//////////////////////////////////////////
class tournoi : public selecteur {
private :
  popu* pop_locale;
   int taille_tournoi ;	// la taille du tournoi
public :
   tournoi(int t);
  tournoi(tournoi* t):selecteur(t),taille_tournoi(t->taille_tournoi){}
  tournoi(int tpop, int t):selecteur(tpop),taille_tournoi(t){}
   virtual void prepare_select(popu*, int, int=0);
   virtual int select();	// renvoie le No du chrom. selecteurne
   virtual selecteur* clone();
};


/*
//////////////////////////////////////////
class sel_boltzmann : public selecteur {
private :
   int taille ;	// la taille du tournoi
public :
   sel_boltzmann(int t, char* s):selecteur(t,s){}
   
   virtual void prepare_select(double*, int, int=0);
   virtual int* select_sans_remise(int, double*, int, int);
   
   virtual int select();	// renvoie le No du chrom. selecteurne

   virtual void init_param();
   virtual void status(ostream&);
};
*/
