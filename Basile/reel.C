#include <ctype.h>
#include <strings.h>
#include <iostream>
using namespace std;
#include "myrand.h"
#include "genutil.h"
#include "parametre.h"
#include "sel.h"
#include "popu.h"
#include "reel.h"

// Les static de la classe reel. SInon, ils ne sont pas alloue par g++

int reel::nval;		// le nbre de bits par reel
int reel::yadebornes;
double reel::pcross;
double reel::pmut;	// proba de mutation d'un bit
int reel::type_crois;      // type de croisement
char* reel::FnPerf;         // quelle fn perf  
char* reel::FicData;         // ou sont les donnees
char* reel::mask_croisement;	
double reel::sigma_global;	// le sigma global
int reel::strategie;		// 1/5, UN_ADAPT ou ADAPT
double reel::force_mut, reel::taux_fixe, reel::seuil;
int reel::nb_mut_reussies, reel::nb_mut_total;
double reel::tau, reel::tau_loc;
double reel::SIGMA_INI;
int reel::Modele_Log;
int reel::AvecNeg;
int reel::Rock;
reel::reel(reel* b)
{
  les_vals=new double[nval];
  sigma_loc = new double[nval];
  for (int i=0; i<nval; i++) {
    les_vals[i] = b->les_vals[i];
    sigma_loc[i] = b->sigma_loc[i];
  }
}
  
///////////////////////////////////////////////////
chromosome* reel::clone() 
///////////////////////////////////////////////////
{
   reel* ptloc = new reel(this);
   return (chromosome*)ptloc;
}

///////////////////////////////////////////////////
chromosome* reel::clone_random()
///////////////////////////////////////////////////
{
   reel* ptloc = new reel;
   for (int i=0; i<nval; i++) 
     {
       if (myrandom() < .1) 
	 ptloc->les_vals[i] = myrandom() * .1; // normalise entre 0 et 1
       else 
	 ptloc->les_vals[i] = myrandom();
       ptloc->sigma_loc[i] = SIGMA_INI;
     }
   return (chromosome*)ptloc;
}
 
///////////////////////////////////////////////////
double reel::dist(chromosome* autre)
///////////////////////////////////////////////////
{
   reel* lotre = (reel*)autre;
   double r,sum=0;
   for (int i=0; i<nval; i++) {
      r = (les_vals[i] - lotre->les_vals[i]);
      sum += r*r;
   }
   return sqrt(sum);
}

// Appel general des operateurs
void reel::operateurs(popu* pop, int , selecteur* sel_mate)
{
  if (flip(pcross)) {
    int elu = sel_mate->select(); // tout est cense etre pert
    chromosome* lelu = pop->GetChromo(elu);
    crossover(lelu);
    raz_perf();
  }
  if (flip(pmut)) // raz_perf is called as well in mutation.
    mutation();
}

// --- Cross Over ---

///////////////////////////////////////////////////
void reel::crossover(chromosome* heureux_elu) // arithmetique sans finesse
///////////////////////////////////////////////////
{
   reel* lotre = (reel*)heureux_elu;
   //modif MS, 8/1/04
   for (int i=0; i<nval; i++) {
     double alpha = myrandom(); // beware of contraction
     les_vals[i] = alpha * les_vals[i] + (1 - alpha) * lotre->les_vals[i] ;
   }
}

///////////////////////////////////////////////////
int reel::mutation(double, double)
   ///////////////////////////////////////////////////
{
    switch(strategie) {
    case FIXE: return mutation_fixe(); 
    case UN_ADAPT:  return mutation_un_adapt(); 
    case ADAPT: return mutation_adapt(); 
    case UN_CINQ: return mutation_un_cinq(); 
    default: cout << " strategie " << strategie << endl; exit(1);
    }
}

#define Poids(i) ((i)%2)
///////////////////////////////////////////////////
int reel::mutation_fixe(double, double)
   ///////////////////////////////////////////////////
{
  int i; // Rechenberg's rule
  if (yadebornes)		// rester ds [0,1]
    for (i=0; i<nval; i++) 
      les_vals[i] = add_gauss(les_vals[i], 0, 1, taux_fixe );
  else
    for (i=0; i<nval; i++) 
      les_vals[i] += randgauss() * taux_fixe;
  raz_perf();
  return 1;
}

///////////////////////////////////////////////////
int reel::mutation_un_cinq(double, double)
   ///////////////////////////////////////////////////
{
  int i; // Rechenberg's rule
  double old_perf = perf();
  if (yadebornes)		// rester ds [0,1]
    for (i=0; i<nval; i++) 
      les_vals[i] = add_gauss(les_vals[i], 0, 1, sigma_global );
  else
    for (i=0; i<nval; i++) 
      les_vals[i] += randgauss() * sigma_global;
  raz_perf();
  if (perf() > old_perf)
    nb_mut_reussies++;
  nb_mut_total++;
  return 1;
}
///////////////////////////////////////////////////
int reel::mutation_un_adapt(double, double)
   ///////////////////////////////////////////////////
{
	// sigma[0] is mutated, all coordinates mutated along N(0,sigma[0])
  int i;
  if (yadebornes)		// rester ds [0,1]
    for (i=0; i<nval; i++) 
      les_vals[i] = add_gauss(les_vals[i], 0, 1, sigma_global );
  else
    for (i=0; i<nval; i++) 
      les_vals[i] += randgauss() * sigma_global;
  raz_perf();
  return 1;
}

///////////////////////////////////////////////////
int reel::mutation_adapt(double, double)
   ///////////////////////////////////////////////////
{
  int i;
  double s = tau * randgauss();	// global mutation factor
  if (yadebornes) 		// rester ds [0,1]
    for (i=0; i<nval; i++) 
      {
	sigma_loc[i] *= exp(s + tau_loc * randgauss());
	les_vals[i] = add_gauss(les_vals[i], 0, 1, sigma_loc[i] );
      }
// 	else // il s'agit du numero d'un SV
// 	  if (myrandom() < sigma_loc[i]) // on le mute
// 	    {
// 	      les_vals[i] = get_exemple_tourmente();
// 	      sigma_loc[i] *= exp(s + tau_loc * randgauss());
// 	    }
//       }
  else
    for (i=0; i<nval; i++) 
      {
	sigma_loc[i] *= exp(s + tau_loc * randgauss());
	les_vals[i] += randgauss() * sigma_loc[i];
      }
//       else // il s'agit du numero d'un SV
// 	if (myrandom() < sigma_loc[i]) // on le mute
// 	  {
// 	    les_vals[i] = get_exemple_tourmente();
// 	    sigma_loc[i] *= exp(s + tau_loc * randgauss());
// 	  }
  raz_perf();
  return 1;
}

//--------------------------------


    // --- Affichage ---
///////////////////////////////////////////////////



///////////////////////////////////////////////////
chromosome* reel::readin(FICH_IN* fin)
///////////////////////////////////////////////////
{
  // quelques mvts memoire en trop, mais est du bon "type" ss surcharge 
   reel* ptloc = (reel *)clone();

   for (int i=0; i<nval; i++)
      ptloc->les_vals[i] = Lit_Reel(fin);
   for (int i=0; i<nval; i++)
      ptloc->sigma_loc[i] = Lit_Reel(fin);
   return (chromosome*)ptloc;
}


///////////////////////////////////////////////////
chromosome* reel::init_param(popu*)  // lecture des parametres specifiques
   ///////////////////////////////////////////////////
{
  new TitleParametre( "Les croisements");
  reel::pcross = (double) * new DoubleParametre( "PCROIS", 
         "Probabilite de croisement", 0.6 );

  new TitleParametre( "Les mutations");
  reel::pmut = (double) * new DoubleParametre("PMUT", "Proba de mutation par individu [0.1]", 0.1);
  WordParametre *strategie1 = new WordParametre("STRATEGIE", "Strategie de mutation : Fixe, un_cinq (regle des 1/5), un_adapt ou adapt", "ADAPT");
  if (!strcasecmp((char *) *strategie1, "fixe")) {
    strategie=FIXE;
    taux_fixe = (double) * new DoubleParametre("TAUX_FIXE", "parametre de mutation fixe", .1);
  }
  else if (!strcasecmp((char *) *strategie1, "un_cinq")) {
    strategie = UN_CINQ;
    force_mut = (double) * new DoubleParametre("FACTEUR5",
                             "Facteur multiplicatif pourla regle des 1/5", 1.1);
    seuil = (double) * new DoubleParametre("SEUIL5",
                             "Seuil pour la regle des '1/5'", 0.2);
    if ( (force_mut<1) || (seuil<0) || (seuil>1) ) {
      cout << "Incompatibilites dans coeffs de la regle des 1/5\n";
      exit(1);
    }
  }
  else if (!strcasecmp((char *) *strategie1, "un_adapt")) {
    strategie = UN_ADAPT;
    tau = (double) * new DoubleParametre("TAU", 
                         "Adaptation du sigma global (* Schwefel's scale)",1);
   }
  else if (!strcasecmp((char *) *strategie1, "adapt")) {
    strategie = ADAPT;
    tau = (double) * new DoubleParametre("TAU", 
                         "Adaptation globale du sigma local (* Schwefel's scale)",1);
    tau_loc = (double) * new DoubleParametre("TAU_LOC", 
                             "Adaptation locale du sigma local (* Schwefel's scale)",1);
  }
  else {
    cerr << "Erreur fatale. Strategie de mutation inconnu: " << (char *)strategie1 << endl;
    exit(1);
  }
    SIGMA_INI = (double) * new DoubleParametre("SIGMA", 
                             "Valeur initiale du Sigma",0.3);
  
  
  new TitleParametre( "Le probleme");
  FnPerf = (char*) * new  WordParametre("LA_PERF", 
			  "Nom de la fonction perf (ROC)", "ROC");
  
  sigma_global = SIGMA_INI;

  maj_probleme((char*)FnPerf,User, nval);

  // ATTENTION, on n'avait pas encore nval !
  tau /= sqrt((double)2*nval);
  tau_loc /= sqrt((double)2*sqrt((double)nval));
  if (User>1) 
    cout << "Tau = " << tau << " et Tau_loc = " << tau_loc << endl;

  // init des divers statiques de la classe
  mask_croisement = new char[nval];
  //  val_min = new double[nval];	// a remplir ...
  //  val_max = new double[nval];	// id

  // pour le moment, meilleur est OK
  return this;
}
