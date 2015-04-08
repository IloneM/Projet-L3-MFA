// -*-C++-*--------------------------------------------------------------
// Where........: CMAP - Polytechnique 
// File.........: reel.h
// Author.......: Marc Schoenauer
// Created......: December 31, 1996
// Description..: Class reel
// 
// Ident........: reel.h,v 1.3 1998/03/20 06:23:35 marc Exp
// ----------------------------------------------------------------------

#ifndef REEL_H
#define REEL_H 
// Classe reel presque "universelle" (sans recombinaison globale)
//
// Il y a un sigma_global a la classe (eventuellement mis a jour ds strategie_avant --> 1/5).
// Chaque individu a un sigma_loc par coordonnees, eventuellement seul le premier sert
// Leur mise a jour se fait dans mutation
//
// parametre essentiel:
//    MUTATION decrit le type de strategie de mutation:
//                 UN_CINQ : regle des 1/5
//                 UN_ADAPT : un sigma adaptatif
//                 ADAPT : tousles sigma adaptatifs
#define FIXE 0
#define UN_CINQ 1
#define UN_ADAPT 2
#define ADAPT 3
#define Max_Strategie 4
//

#include "chromosome.h"
#include "mystdio.h"
#include "parametre.h"

//class ostream;
typedef int (*fmutation)(double, double);

///////////////////////////////////////////////////////////////////////
// classe reel
///////////////////////////////////////////////////////////////////////

class reel : public chromosome {
  protected :
  static int nval;		// le nbre de coordonnees par reel
  static int yadebornes;	// si 0, pas de bornes. Si 1, normalise entre 0 et 1
  //  static double val_min=0;
  //  static double val_max=1;	// les bornes pour les coordonnees
  static double pcross;		// si on faisait du croisement
  static int type_crois;	// type de croisement
  static char* mask_croisement;	
  static char* FnPerf;		// quelle fn perf  
  static char* FicData;		// ou sont les donnees 

  static double pmut;		// proba de muter
  static double sigma_global;	// le sigma global
  static int strategie;		// 1/5, UN_ADAPT ou ADAPT
  // pour les mutations de la mutation (locale ou globale)
  static double force_mut, seuil, taux_fixe; // en cas de strategie 1/5
  static int nb_mut_reussies, nb_mut_total; 
  static double tau, tau_loc;	// en cas d'adapt (ou un_adapt)
  static double SIGMA_INI;	// valeur initiale du sigma

  //  static int No_Codage_Trick; // (SV index, alpha)
  static int Modele_Log; // somme lineaire ou somme lineaire des logs.
  static int AvecNeg; // poids ds [0,1] ou [-.5,.5]
  static int Rock; // critere aire roc ou sum i * predit[i]
  static double SRM; // treshold rewarding small coefficients
  static int Linear; // function assumed linear or centered
  static int Multi; // number of functions aggregated
  // les data perso
  double* les_vals;
  double* sigma_loc;



  public :
  // les methodes
  reel(int)
  {les_vals=new double[1];sigma_loc=new double[1];} ;       // appele 1 fois par main
  reel()
  {les_vals=new double[nval]; sigma_loc=new double[nval];};	// appele par clone_random
  reel(reel*);	                // appele par (sous-)clone
  virtual ~reel() {delete[] les_vals; delete[] sigma_loc;};
  virtual chromosome* clone() ;
  virtual chromosome* clone_random(); 
  virtual double calcule_tout(); 
  virtual double dist(chromosome* autre) ;  
   
  // --- Appel global ---
  virtual void operateurs(popu*, int, selecteur*);
  // --- Cross Over ---
  virtual void crossover(chromosome* ); // modifie this en fn de l'autre
  // --- Mutation ---
  virtual int mutation(double=0.0, double =0.0) ;
  int mutation_un_cinq(double=0.0, double =0.0) ;
  int mutation_un_adapt(double=0.0, double =0.0) ;
  int mutation_adapt(double=0.0, double =0.0) ;
  int mutation_fixe(double=0.0, double =0.0) ;
  
  // --- Affichage ---
  virtual void printout(ostream&);
  virtual ostream& operator<< (ostream& fich)
  { printout(fich); return fich;};
  virtual void Affiche();
      
  virtual chromosome* readin(FICH_IN*);
  virtual chromosome* init_param(popu*);  // parametres specifiques, et choix type
  virtual int strategie_avant(popu*);
  //  virtual int strategie_pendant(popu*);
  //  virtual int strategie_apres(popu*);
  void maj_probleme(char*,int, int);    // in perf.C, maj de la fn a optimiser
      
  double get_exemple_tourmente();
  void affiche_memoire();
  void affiche_pile();
  void //reel::
  affiche_SV();
  void raz_memoire();
  void Effectue_Test();
};

#endif
