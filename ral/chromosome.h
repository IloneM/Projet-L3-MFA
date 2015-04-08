// -*-C++-*--------------------------------------------------------------
// Where........: CMAP - Polytechnique 
// File.........: chromosome.h
// Author.......: Marc Schoenauer
// Created......: Tue Mar 21 12:46:29 1995
// Description..: Class chromosome: defini les proprietes
//		  de base necessaires aux individus des populations
//		  genetiques
// 
// Ident........: $Id: chromosome.h,v 1.11 1998/03/09 14:40:08 marc Exp $
// ----------------------------------------------------------------------

#ifndef CHROMOSOME_H
#define CHROMOSOME_H 

#include <iostream>
using namespace std;

#include "parametre.h"
#include "mystdio.h"

//class _ostream;
class selecteur;
class popu;

///////////////////////////////////////////////////////////////////////
// classe chromosome : pure virtuelle
///////////////////////////////////////////////////////////////////////

class chromosome {
private:
  double performance ;	     // passer par perf() et maj_perf() pour y acceder
  int perf_ok;			// faut-il recalculer la perf ?
  unsigned long age ;		// le compteur nb_eval lors de sa creation.
  static int nb_eval;

  // --- private, pour etre sur de mettre a jour la perf via _clonexxx ---
  virtual chromosome* clone() = 0;	// alloue memoire, copie et renvoie pointeur
  virtual chromosome* clone_random() = 0; // alloue memoire, init aleatoire et renvoie pointeur
  virtual double calcule_tout() = 0; // ne doit JAMAIS etre appele directement

protected:
public:				// ouverts a tous
  static IntParametre	User;		// niveau d'affichage
  chromosome():perf_ok(1){}
  chromosome(chromosome* a):
    performance(a->performance),
    perf_ok(a->perf_ok),
    age(a->age)
  {}
  virtual ~chromosome();

  // --- les points d'entree de clonages et lecture, traitant la perf et l'age
  chromosome* _clone()
  {
    chromosome* pt = clone();
    pt->performance = perf();
    pt->perf_ok = 1;
    pt->age = age;		// strictement identiques ...
    return pt;
  }
  chromosome* _clone_random()	// pour ne plus oublier de faire raz_perf
  {
    chromosome* pt=clone_random();
    pt->raz_perf();
    pt->age = nb_eval;
    return pt;
  }
  // _readin lit la perf, et passe la main a la lecture specifique
  chromosome* _readin(FICH_IN*);
  virtual chromosome* readin(FICH_IN*) = 0 ; // ne traite plus que le chromo

  virtual double dist(chromosome* autre) = 0 ;  

  // --- Appel global ---
  virtual void operateurs(popu*, int, selecteur*) = 0;
  // --- Cross Over ---
  virtual void crossover(chromosome* ) = 0  ; // modifie this en fn de l'autre
  // --- Mutation ---
  virtual int mutation(double =0.0, double =0.0) = 0 ;

  // --- Affichage ---
  virtual void printout(ostream&) = 0 ;
  virtual ostream& operator<< (ostream& fich)
  { printout(fich); return fich;}
  virtual void Affiche() {}
      
  // appellees au cours d'une generation, avant et apres reproduction
  virtual int strategie_avant(popu*){return 1;};
  virtual int strategie_pendant(popu*){return 1;};
  virtual int strategie_apres(popu*){return 1;};
   
  virtual chromosome* init_param(popu*);	// lecture des parametres specifiques
  virtual void FermeTout(void){}      // actions specifiques en fin d'evloution
  // et eventuellement mise a jour de meilleur (APRES lecture des parametres)
  virtual double perf() 
  {
    if (!perf_ok) 
      {
	performance = calcule_tout();
	nb_eval++;
	perf_ok=1;
      }
    return performance;
  }
  void raz_perf() { perf_ok = 0; }
  
  virtual int viable(){return 1;}	// si non viable, on recommence (init_random ou operateur genetique)
  virtual int NbEval() { return nb_eval;} // Donne le nb Eval de la classe eventuellement DERIVEE si surcharge
  virtual unsigned long Age() {return age;}
  virtual int comp_inf(chromosome*);// appele par les globalesci-dessus,
  // destinee a etre surchargee eventuellement
  virtual void Cree_Somme(){};	// desaffecte a partir de MIC_Relax.
  virtual void Cree_Distrib(){}; // desaffecte a partir de MIC_Relax.
};

typedef chromosome *PtChromo ;
typedef chromosome *Pchromosome ;

// utilistaires de tri
////////////////////////
// utilitaire pour tris ds tableaux de chromo
int comp_perf_maximise(const void* v1, const void* v2);
int comp_perf_minimise(const void* v1, const void* v2);


#endif
