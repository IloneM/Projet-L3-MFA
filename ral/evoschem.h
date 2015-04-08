// ----------------------------------------------------------------------
// Where........: CMAP - Polytechnique 
// File.........: evoschem.h
// Author.......: Marc Schoenauer
// Created......: 20/4/96
// Description..: Implementation de la classe evoschem : schema d'evolution
// 
// Ident........: $Id: evoschem.h,v 1.1 1996/05/30 17:32:42 marc Exp $
// '----------------------------------------------------------------------
#ifndef _H_EVOSCHEM
#define _H_EVOSCHEM

class popu;
class chromosome;
class selecteur;
class remplaceur;

/////////////////////////////////////////////////////////////////
// La classe de base evoschem
/////////////////////////////////////////////////////////////////
class evoschem 
{
protected:
    unsigned int taille_pop;
public:
evoschem(unsigned int t){taille_pop=t;}
virtual ~evoschem();
virtual void boucle(popu*, int)=0;
virtual int elitisme(){return 0;}			/* faut-il tester et eventuellement re-injecter l'ancien meilleur ? */
virtual int max_offspring()=0;		/* la taille qu'il faut prevoir pour les_chromos (parents + offspring) */
chromosome** get_les_chromos(popu* pop);	/* pour manipualtion locale */
};

evoschem* ChoixEvolution(const char * EvoName, const int taille);

/////////////////////////////////////////////////////////////////

class gag: public evoschem
{
protected:
    selecteur *selector;
    selecteur *selector_mate;

int param_elitisme;
public:
gag(unsigned int);
~gag();
virtual void boucle(popu*, int);
virtual int max_offspring(){return 2*taille_pop;}
virtual int elitisme() {return (int)param_elitisme;}
};

/////////////////////////////////////////////////////////////////

class ssga: public evoschem
{
protected:
    selecteur *selector;	/* les 3 tournois */
    selecteur *selector_mate;
    remplaceur *replacor;		// le tournoi de Remplacement
public:
ssga(unsigned int);
~ssga();
virtual void boucle(popu*, int);
virtual int max_offspring(){return 2*taille_pop;}
};

/////////////////////////////////////////////////////////////////

class es: public evoschem	// classe generique Evolution Strategie
{
protected:
    int lambda;			/* nbre d'offspring */
    int *tab_es;
    int taille_tab_es;
  selecteur *selector;	// force a equireparti
  selecteur *selector_mate;	// force a sel_uniforme
public:
es(unsigned int);
virtual void boucle(popu*, int)=0;
virtual int max_offspring();
};

/////////////////////////////////////////////////////////////////

class esplus: public es
{
protected:
public:
esplus(unsigned int t):es(t){}
virtual void boucle(popu*, int);
};

/////////////////////////////////////////////////////////////////

class escomma: public es
{
protected:
public:
escomma(unsigned int t);		// un controle de coherence en +
virtual void boucle(popu*, int);
};

/////////////////////////////////////////////////////////////////

class ep: public evoschem
{
protected:
    selecteur *selecteur_mate;	// force a sel_uniforme, pour le cass ou
    remplaceur *replacor;	// force a tournoi_ep

public:
ep(unsigned int);	
~ep(){}
virtual void boucle(popu*, int);
virtual int max_offspring(){return 2 * taille_pop;}
};


/////////////////////////////////////////////////////////////////

class totale: public evoschem
{
protected:
  unsigned int immobilisme;		// parents non detruits
  unsigned int fertile;			// nbre de parents ayant des enfants 
				// (les autres disparaissent corps et biens) 
  unsigned int taux;			// nbre d'enfants par parent
  int param_elitisme;		// ...
  selecteur *selector;		// selecteur initial
  selecteur *selector_mate;	// selecteur pour croisement

  selecteur *selector_local;	// selection entre les enfants d'un meme parent
  int conflit_local;		// avec ou sans le parent en question

  remplaceur *replacor;		// remplacement final
  int conflit_generations;	// entre parents-enfants pour remplacement

  // methode privee: selection locale
void selection_locale(popu*, int, int);
public:
totale(unsigned int);	
~totale();
virtual void boucle(popu*, int);
virtual int max_offspring();
virtual int elitisme() 
  {
    if (immobilisme>0) return 0; /* inutile si immobilisme */
    else return param_elitisme;
  }
};




#endif
