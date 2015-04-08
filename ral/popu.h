// -*-C++-*--------------------------------------------------------------
// Where........: CMAP - Polytechnique 
// File.........: popu.h
// Author.......: Marc Schoenauer
// Created......: Tue Mar 21 13:11:05 1995
// Description..: Declaration de la classe population
// 
// Ident........: $Id: popu.h,v 1.15 1998/03/20 06:13:56 marc Exp $
// ----------------------------------------------------------------------

#ifndef POPU_H
#define POPU_H  


///////////////////////////////////////////////////////////////////////
// classe chromosome : pure virtuelle
///////////////////////////////////////////////////////////////////////

#include <time.h>

#include "mystring.h"
#include "pipecom.h"

#include "chromosome.h"
#include "parametre.h"

class selecteur;
class evoschem;

///////////////////////////////////////////////////////////////////////
// classe popu
///////////////////////////////////////////////////////////////////////
class popu {
protected:
  ParamList                parList;        // La liste des param

  TitleParametre        titPopu;        // Le titre des param de popu
  UintParametre        recense;        // taille de la population

  // --- Criteres d'arrets: ---
  // ---         - pas avant premiere_ligne_droite ---
  // ---         - si pas mieux en derniere_ligne_droite iterations ---
  TitleParametre        titArret;        // Titre des criteres
  IntParametre        MaxEval;        // Nombre maximum d'evaluation
  IntParametre        MaxGen;                // Nombre maximum de generation
  int             MaxAge;                // L'un ou l'autre suivant lequel est non nul
  IntParametre        premiere_ligne_droite;
  IntParametre        derniere_ligne_droite;
  WordParametre        PerfCible;        // Arret sur perf atteinte [neant]

  TitleParametre        titAffiche;        // Titre pour les affichages
  IntParametre        User ;                // affichage
  IntParametre        Courbe;                // Frequence d'aff de la courbe

  TitleParametre        titSauve;        // Titre pour les sauvegardes
  IntParametre        frequence_sauvegarde; // si si
  WordParametre        nomsauvegarde;        // et oui
  WordParametre       nomres;	           // nom du fichier gnuplot
  String              NomRes;	// double emploi avec le precedent !!!
  IntParametre        recalcul_perf; // faut-il recalculer apres readin ?

  TitleParametre        titEvo;            // Titre pour le schema d'evolution
  IntParametre                OnMaximise; // Maximisation ou minimisation ?
  WordParametre                NomEvo; // la (nom du) schema d'evolution

  unsigned int        recense_max;        // nbre max  (taille les_chromos)
  chromosome** les_chromos ;      // taille pour parents + enfants

  evoschem* evolution ;        // le schema d'evolution

  int (*comp_perf)(const void*, const void*);
  int Arret_sur_cible;        // si on doit tester best_perf / perf_cible
  double perf_cible;        // la valeur de la perf declenchant l'arret
  double worse_perf, best_perf, somme ;       // Attention, tiens compte de maximise

  chromosome* meilleur ;        // garde la trace du meilleur 

  int         nb_gen;                        //  No generation courante (old stuff)
  time_t        Chrono;                        // chronometre pour le temps/gen

  // --- Les detecteurs d'arret ---
  int                dernAmelior;                // Repere la derniere generation qui a
  // ameliore la meilleure_perf
  int                amelior;	         // dit si la derniere generation a ameliore
  int                sprintFinal;                // Repere si la 1ere ligne droite est passee
  int         Age_sauvegarde;                // Date de la derniere sauvegarde

  // --- Affichage de la perf de la population ---
  PCom        *gpCom;                        // Communication avec gnuplot
  int                initGnuplot;                // Flag d'initialisation de gnuplot
  FILE        *resFile;                // Fichier des performances
  // --- Affichage des histogrammes de la perf de la population ---
  PCom        *hpCom;                        // Communication avec gnuplot
  int                initHnuplot;                // Flag d'initialisation de gnuplot
  void genere_histo(double, double); // Affichage de l'histogramme


  String        Name;                        // Le nom de la popu

  int historiquec ;                // nbre de fichiers d'init
  char** historique;                // lsite des noms de fichiers initialisant
  int* nb_historique;                // et liste des nbres d'individus provenant de chaque

  // ---------- Fonction internes ----------
  int statistiques();
  void dump_parents(ostream& fich);

   
  int test_arret(int);

  void status(ostream&);
  void init();                                // initialisation des parents : lecture eventuelle
  // des fichiers historique (init_param)
  // puis aleatoires (clone_random)
  int readin(char*, int);        // lecture de parents[ind_courant ..] ds fichier ASCII

public:
  //=====
  virtual void sauvegarde();        // sur fichier, en ASCII
  void help( int argc, char **argv );
   
  popu( chromosome* c, char *name=NULL ) ;
  virtual ~popu(); 
   
  void init_param(int argc, char *argv[]);
  int generation() ;
  chromosome* rend_meilleur(){return meilleur;}

  // --- Accesseurs ---
  int GetSize() { return (int)recense; }
  double perf_opt() {return best_perf;}
  double perf_somme() {return somme;}
  int Age();			// l'age en fn du compteur (Gen ou Eval)
  int Age_Max(){return MaxAge;}
  int Gen() {return nb_gen;}	// age en generations 
			 // appeler meilleur->NbEval() pour les evaluations
  chromosome * GetChromo( int i ) { return les_chromos[i]; }

  // utilitaire de tri (en fn de maximise, odre ad hoc
  int maximise() {return (int)OnMaximise;}
  void trie_les_chromos(int debut, int nbre); // ds le tableau les_chromos
  int comp_chromos(int i, int j) // ATTENTION a l'ordre: qsort != comparaison std
  {return (*comp_perf)(les_chromos+j, les_chromos+i);}

  // Friend, pour deplacer et detruire ds la boucle d'evolution
  friend class evoschem;
  // chromosome** evoschem::get_les_chromos(popu*);// les schemas manipulent directement les_chromos

        
  // RAJOUTE POUR LES BESOINS DE IMIT
        
  int GetSizeMax() { return (int)recense_max; }
  int GetImmobile() { return nb_gen - dernAmelior;}
  int amelioration() { return amelior; }
  // Affreux bricolage, visant à permettre que la perf puisse changer en cours de route, typiquement, decroitre quand on optimise;
  // Ceci est incomplet, il faudrait faire ca pour tous les chromos, pas seulement le premier.
  void Amnesie();
};


class copopu : public popu {        // basique pour co-evolution de 2 populations
protected:
  popu* la_copopu;                
public:                                // les accesseurs
  virtual ~copopu(); 
  copopu(chromosome* ch, char* str):popu(ch, str){}
  popu* GetCopopu(){return la_copopu;}
  void MajCopopu(popu* pop){la_copopu=pop;}
  void sauvegarde();        // sur fichier, en ASCII
  
};

#endif

