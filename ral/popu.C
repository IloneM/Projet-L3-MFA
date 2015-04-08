// ----------------------------------------------------------------------
// Where........: CMAP - Polytechnique 
// File.........: popu.C
// Author.......: Marc Schoenauer
// Created......: Tue Mar 21 13:13:47 1995
// Description..: Implementation de la classe population.
// 
// Ident........: $Id: popu.C,v 1.20 1998/03/10 13:10:14 marc Exp $
// ----------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////
// popu.C : travaille sur des chromosomes, via les methodes virtuelles
////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <sstream>
#include <fstream>
using namespace std;

#include "myrand.h"
#include "lecture.h"

#include "appli.h"
#include "popu.h"
#include "evoschem.h"
#include "sel.h"




///////////////////////////////////////////////////////////
//       classe popu
/////////////////////////////////////////////////////

// 
// Createur de la classe:
// 	le chromosome passe est l'initiateur de toute la population
//	il est stockee dans le meilleur
// Les parametres de la populaion sont initialises ici 
//	(a la creation de l'objet)
// 
popu::popu( chromosome *c, char *name ) :
  // --------------------------
  parList( name, 0 ),
  titPopu( "La population" ),
  recense( "POPULATION", "taille population", 100 ),

  titArret( "Criteres d'arret" ),
  MaxEval( "MAX_EVAL", "Nbre max total d'evaluations de fitness", 0 ),
  MaxGen( "MAX_GEN", "Nbre max de generations (si pas nbre d'eval)", 100 ),
  premiere_ligne_droite( "DEBUT", "Nbre minimal d'evaluations (ou de generations)", 50 ),
  derniere_ligne_droite( "FIN", "Nbre max d'evaluations (ou de generation) sans progres", 10 ),
  PerfCible( "PERF_CIBLE", "Valeur cible pour la perf ", "NEANT" ),

  titSauve( "Les sauvegardes/relectures" ),
  frequence_sauvegarde( "FREQ_SAV", "Frequence des sauvegardes (0 = en fin de pgm only)", 0 ),
  nomsauvegarde( "NOM_SAV", "Prefixe des fichiers de sauvegarde (+ No gen)", "sauvegarde" ),
  nomres( "NOM_RES", "Nom du fichier de courbe (+.xg)", "popu" ),
  recalcul_perf("RECALCUL_PERF", "Faut-il recalculer les perfs des chromos relus (0/1)",0),

  titAffiche( "Affichage" ),
  User( "AFF_POP", "Niveau d'affichage population", 1 ),
  Courbe( "COURBE", "Frequence d'affichage courbe", 0 ),
  titEvo( "Schema d'evolution" ),
  OnMaximise( "MAXIMISATION", "Maximisation (1) ou minimisation(0)", 1),
  NomEvo("EVOLUTION", "Schema d'evolution (GAG, 'ES,', ES+, EP)", "GAG"),
  Name(name)
{
  meilleur = c;
  gpCom = NULL;
  hpCom = NULL;
  initGnuplot = initHnuplot = 0;
  resFile = NULL;
  dernAmelior = 0;
  sprintFinal = 0;
  les_chromos = 0;
  historiquec = 0;
  if( Name.Length() == 0 )
    Name = "popu";
  NomRes = (String)nomres + ".xg";
  // le choix du schema d'evolution
  evolution = ChoixEvolution(NomEvo, (int)recense);

  // l'arret sur perf_cible atteinte: si PERF_CIBLE ne vaut ni NEANT 
  if ( Arret_sur_cible = (strcasecmp(PerfCible, "NEANT")&&strcasecmp(PerfCible, "RIEN")) )
    perf_cible = Lit_Reel((char *)PerfCible);

  if (maximise())
    comp_perf = &comp_perf_maximise;
  else
    comp_perf = &comp_perf_minimise;
}



// 
// Le destructeur, charge de fermer proprement les choses.
// 
popu::~popu()
// ----------
{
  int		i;

  if( gpCom ) {
    PipeComSend( gpCom, "quit\n" );	// Ferme gnuplot
    PipeComClose( gpCom );
    gpCom =NULL;
  }
  if( hpCom ) {
    PipeComSend( hpCom, "quit\n" );	// Ferme gnuplot
    PipeComClose( hpCom );
    hpCom =NULL;
  }
  sauvegarde();

  if( resFile )			// Le fichier des performances
    fclose( resFile );
  if( les_chromos ) {			// Les chromosomes
    les_chromos[0]->FermeTout(); // Actions specifiques du chromosome en fin
    for( i=0; i < recense; i++ )
      delete les_chromos[i];
    delete [] les_chromos;
  }
  // --- Autres tableaux alloues ---
  if( historiquec > 0 )
    {
      delete [] historique;
      delete [] nb_historique;
    }
}

copopu::~copopu()
// ----------
{
  if (la_copopu)
    sauvegarde();
}

//----------------------------------------------
void popu::init()
//----------------------------------------------
{
  // les tableaux de popu
  recense_max = evolution->max_offspring();// c'ets le schema d'evolution qui sait ca
  les_chromos = new PtChromo[recense_max];

  if (!les_chromos) 
    {
      cout << Name << " : Problemes d'allocation memoire\n" ;
      exit(1);
    }

  nb_gen = 0;
   
  // maintenant, la creation des chromos
  int i, index_loc, index_initialise = 0;
  if (historiquec)			// il y a des fichiers a lire
    {					// les noms sont deja charges 
      for (i=0; i<historiquec; i++)
	{
	  index_loc = readin( historique[i], index_initialise) ;
	  nb_historique[i] = index_loc ;	// nombre lu ds ce fichier
	  index_initialise += index_loc;
	}
    }

  if (index_initialise < recense)	// il en manque encore -> aleatoires
    for (i=index_initialise; i<recense; i++)
      {
	do {
	  les_chromos[i] = meilleur->_clone_random();
	  if (! les_chromos[i]->viable() ) {
	    delete les_chromos[i];
	    les_chromos[i] = 0;
	  }
	} while( (les_chromos[i] == 0) && (test_arret(-1) != 0) );
	if( test_arret(-1) == 0 ) {
	  // --- Horreur: On n'a pas cree tous les indiv. ---
	  // --- (on a surement depasse MaxEval) ---
	  recense = (unsigned int) i;
	  cout << Name << " : Initialisation ratee, on sauve ce qu'on peut: "
	       << recense << endl;
	  	     sauvegarde();
	  exit(-1);
	}
      }
}

//----------------------------------------------
// lecture de parents[ind_courant ..] ds fichier ASCII
int popu::readin(char* nomfic, int ind_courant)	
//----------------------------------------------
{
  FICH_IN *fin ;
  int i, nbloc;
  char motloc[1024];
   
  fin = Ouvre_FICH_IN(nomfic);  // test dans Ouvre_FICH_IN 
  M_Delim(fin,':');		// : et , st des delimiteurs
  M_Delim(fin,';');

  // recherche du mot INDIVIDUS
  while (strcmp(Lit_Mot(fin, motloc), "INDIVIDUS"))
    ;
  nbloc = (int)Lit_Entier(fin);
   
  if (nbloc+ind_courant > recense)
    {
      cout << Name << " Attention: trop d'individus dans " << nomfic << endl;
      cout << nbloc << " dans le fichier" << endl;
      cout << ind_courant << " deja lus" << endl;
      cout << recense << " necessaires au total" << endl;
    }

  for (i=ind_courant; (i < recense) && (i<nbloc+ind_courant); i++)
    les_chromos[i] = meilleur->_readin(fin); // lit la perf, puis passe la main au readin specifique
  if (recalcul_perf) 
    for (i=ind_courant; (i < recense) && (i<nbloc+ind_courant); i++)
      les_chromos[i]->raz_perf();
  // et le prochain appel a perf() recalculera tout
  return nbloc ;
   
}

// en parametre ?
#define MAX_HISTO 100

//----------------------------------------------
void popu::genere_histo(double pmax, double pmin)
//----------------------------------------------
{
  int ipas, ichrom;
  FILE *fhisto=fopen( Name + ".histo", "w" );
  if (!fhisto)
    {
      cout << "Probleme pour ouvrir " << Name << ".histo" << endl;
      return;
    }
  double pas=(pmax-pmin)/MAX_HISTO;
  int pnb[MAX_HISTO];
  for (ipas=0; ipas<MAX_HISTO; ipas++)
    pnb[ipas]=0;
  for (ichrom=0; ichrom<recense; ichrom++)
    {
      if (les_chromos[ichrom]->perf() == pmax)
	pnb[MAX_HISTO-1]++;
      else
	pnb[(int) ((les_chromos[ichrom]->perf()-pmin)/pas)]++;
    }
  // Ecriture des histos: ne pas oublier le 1er et le dernier
  fprintf(fhisto, "%g 0\n", pmin);
  for (ipas=0; ipas<MAX_HISTO; ipas++)
    {
      fprintf(fhisto, "%g %d\n", pmin+ipas*pas, pnb[ipas]);
      fprintf(fhisto, "%g %d\n", pmin+(ipas+1)*pas, pnb[ipas]);
    }
  fprintf(fhisto, "%g 0\n", pmax);
  fclose(fhisto);
  // --- Et on appelle gnuplot sur ce fichier ---
  if( ! initHnuplot ) {
    // --- C'est la 1ere fois: initialise gnuplot ---
    char	*args[6];
    args[0] = strdup( "gnuplot" );
    args[1] = strdup( "-geometry" );
    args[2] = strdup( "300x200-0+240");
    args[3] = strdup( "-title" );
    args[4] = strdup( (char *)Name );
    args[5] = 0;
    hpCom = PipeComOpenArgv( "gnuplot", args );
    if( ! hpCom )
      cerr << "Impossible de communiquer avec gnuplot" << endl;
    else {
      PipeComSend( hpCom, "set grid\n" );
      PipeComSend( hpCom, "set data style lines\n" );
      PipeComSend( hpCom, "plot '" + Name + ".histo'\n" );
    }
    initHnuplot = 1;
  }
  else {
    if( hpCom )
      {
	char sgen[64];
	sprintf(sgen, "title 'Generation %d'\n", nb_gen);
	PipeComSend( hpCom, "plot '" + Name + ".histo' " + sgen );
      }
  }
  
}
  // Affreux bricolage, visant ?? permettre que la perf puisse changer en cours de route, typiquement, decroitre quand on optimise;
  // Ceci est incomplet, il faudrait faire ca pour tous les chromos, pas seulement le premier.
  
void popu::Amnesie() 
{ 
  worse_perf = best_perf = 0; 
  for (int i=0; i < recense; i++)
    {
      les_chromos[i]->raz_perf();
      les_chromos[i]->perf();
    }
  trie_les_chromos(0, recense);
  delete meilleur;
  meilleur = les_chromos[0]->_clone();
};

//----------------------------------------------
int popu::statistiques()
//----------------------------------------------
{
  double	old_perf;	// la generation precedente
  int		replot_courbe = 0;
  int ibest, iworse;

  // min, max et moyenne
  double perf_min, perf_max;
  int imax=0, imin=0;
  somme = perf_max = perf_min = les_chromos[0]->perf();
  if (recense>1)			// tout prevoir !
    for (int i=1; i<recense; i++)
      {
	double p = les_chromos[i]->perf();
	somme += p;
	if (p > perf_max)
	  {
	    imax = i;
	    perf_max=p;
	  }
	if (p < perf_min)
	  {
	    imin = i;
	    perf_min=p;
	  }
      }
  int deterior;	//  int amelior, deterior;   amelior est ds popu (pour appels)
  if (maximise())
    {
      best_perf = perf_max;
      worse_perf = perf_min;
      ibest = imax;
      iworse = imin;
      amelior = (best_perf > meilleur->perf()) ; // amelioration
      deterior = (best_perf < meilleur->perf()); // perdu le meilleur
    }
  else
    {
      best_perf = perf_min;
      ibest = imin;
      worse_perf = perf_max;
      iworse = imax;
      amelior = (best_perf < meilleur->perf()) ; // amelioration
      deterior = (best_perf > meilleur->perf()); // perdu le meilleur
    }

  if ( (perf_max==perf_min) && (recense > 1) ) {
    cout << Name << " : Difficile de demarrer : max = min.\n";
    //    exit(1);
  }
  // le meilleur global
  if (amelior)	// garder le meilleur
    {
      old_perf = meilleur->perf(); // garder pour affichage
      delete meilleur;
      meilleur = les_chromos[ibest]->_clone();
      replot_courbe = 1;
    }
      
  if( User >= 1 ) 
    {
      if( User > 2 )
	cout << Name
	     << " Gen " << nb_gen << " - " << meilleur->NbEval() << "  Max = "
	     << best_perf << "  Min = " << worse_perf << "  Moy = " << 
		      somme/(int)recense << endl;
      if (nb_gen == 1)
	cout << Name << " ++ " << nb_gen << " " << meilleur->NbEval() << "  xx -> " <<  best_perf  << endl;
      else
	if( amelior ) // amelioration
	  {
	    cout << Name << " ++ " << nb_gen << " " << meilleur->NbEval() << " " << old_perf << " -> " <<  best_perf  << endl;
	    if (User >= 2)
	      {
		meilleur->printout(cout);
		cout << "\n";
	      }
	  }
      cout.flush();
    }
  // --- Ecriture du fichier pour courbes de perf ---
  if( ! resFile ) {
    resFile = fopen( NomRes, "w" );
  }
  // --- Ecriture et flush du fichier de perf ---
  fprintf( resFile, "%d\t%g\t%g\n", meilleur->NbEval(), 
	   best_perf, somme / (int)recense );
  fflush( resFile );
  
  if( Courbe && ( ((nb_gen % Courbe)==0) || replot_courbe) ){
    // --- Affiche joliement le meilleur ---
    meilleur->Affiche();

    // --- Et on appelle gnuplot sur ce fichier ---
    if( ! initGnuplot ) {
      // --- C'est la 1ere fois: initialise gnuplot ---
      char	*args[6];
      args[0] = strdup( "gnuplot" );
      args[1] = strdup( "-geometry" );
      args[2] = strdup( "300x200-0+0" );
      args[3] = strdup( "-title" );
      args[4] = strdup( (char *)Name );
      args[5] = 0;
      gpCom = PipeComOpenArgv( "gnuplot", args );
      if( ! gpCom )
	cerr << "Impossible de communiquer avec gnuplot" << endl;
      else {
	PipeComSend( gpCom, "set grid\n" );
	PipeComSend( gpCom, "set data style lines\n" );
	PipeComSend( gpCom, "plot '" + NomRes + "' using 1:2 title 'Best'," );
	PipeComSend( gpCom, "   '" + NomRes + "' using 1:3 title 'Average'\n" );
      }
      initGnuplot = 1;
    }
    else {
      if( gpCom )
	PipeComSend( gpCom, "replot\n" );
    }
    // --- Affichage des courbes histogrammes ---
    genere_histo(perf_max, perf_min);
  }

  // --- Declenche le chrono a la premiere passe ---
  if( nb_gen == 1 )
    time( & Chrono );

  if( frequence_sauvegarde && !(nb_gen % frequence_sauvegarde) ) {
    sauvegarde();
    // --- Affiche le temps ecoule (stable a partir de nb_gen=2) ---
    if( nb_gen > 1 ) {
      double	tps_moyen = ( difftime( time(NULL), Chrono ) 
			      / (nb_gen-1) );
      cout << Name << " : Temps / generation (" << nb_gen << "): " << tps_moyen << " sec." << endl;
    }
  }

  if (deterior) // meilleur a ete perdu
    if (evolution->elitisme())	// le remettre a laplace du pire
      {				// APRES tous les affichages
	delete les_chromos[iworse];
	les_chromos[iworse] = meilleur->_clone();
	best_perf = meilleur->perf(); // pour les selections !!!
	somme = somme + best_perf - worse_perf; // ATTENTION, worse_perf n'est plus a jour !!!
      }
 
  return test_arret(amelior); 
}



//----------------------------------------------
void popu::dump_parents(ostream& fich)
//----------------------------------------------
{
  int i;
   
  if (User<=2) return;
   
  cout << Name << " Gen " << nb_gen << " - " << meilleur->NbEval() ;
  if (User > 3)               // si User = 2 : une seule ligne
    {
      for (i=0; i< GetSize(); i++)
	{
	  fich << i << " - ";
	  fich << les_chromos[i]->perf() << " ";
	  les_chromos[i]->printout(fich) ;
	  fich << endl;
	}
    }
  // et dans tous les cas  pour les parents
  fich << " Max = " << best_perf << " min = " << worse_perf << " average = " <<  somme / (int)recense << endl ;
  return ;
}

//-------------------------------------------
int popu::Age()
//----------------------------------------------
{
  if((int) MaxEval )
    return meilleur->NbEval();
  else
    return nb_gen;
}
  
//-------------------------------------------
int popu::test_arret(int amelioration)
//----------------------------------------------
{
  if( AppliArretDemande() ) {	// gere les Ctrl C
    return 0;
  }
  int ageCourant=Age();
  int nbre_max=Age_Max();

  if (ageCourant >= nbre_max) {
    cout << Name << " Arret: Age maximum atteint" << endl; 
    return 0;
  }
  if (amelioration == -1)	// cas particluier de l'initialisation
    return 1;			// on ne teste que le nbre max d'evaluations

  if (Arret_sur_cible)
    if ( (maximise() && (best_perf >= perf_cible)) ||
	 (!maximise() && (best_perf <= perf_cible))
	 ) {
      cout << Name << " Arret: Perf cible atteinte" << endl; 
      nb_gen = MaxGen; // XXXXX ceci pour le depouillement. MS.
      return 0;
    }
		
         
  if (amelioration)
    dernAmelior = ageCourant;
  else
    if (sprintFinal)		// deja ds derniere ligne droite
      {
	if (ageCourant-dernAmelior >= derniere_ligne_droite)
	  // tps depuis la derniere amelioration
	  {
	    cout << Name << " Arret: Pas d'amelioration depuis "
		 << dernAmelior << endl; 
	    return 0;	// stoppez tout
	  }
      }
    else				// encore ds premiere ligne droite
      if (ageCourant >= premiere_ligne_droite)	// GOTO derniere ligne droite
	{
	  sprintFinal = 1;
	  dernAmelior = ageCourant;
	  if (User)
	    cout << Name << " Derniere ligne droite\n";
	}      
   
  return 1;
}



//----------------------------
void popu::trie_les_chromos(int debut, int nb)
//----------------------------
{
  qsort(les_chromos+debut, nb, sizeof(chromosome *), comp_perf);
}

//----------------------------
void popu::sauvegarde()
//----------------------------
{
  int i;
  char nomfich[256], sloc[1024];
   
  if (Age_sauvegarde == nb_gen)
    return;
  sprintf(nomfich, "%s%d", (char*)nomsauvegarde, nb_gen);
  filebuf buffloc; 
  if (buffloc.open(nomfich,ios_base::out) == 0)
    cout << Name
	 << " ERREUR ds l'ouverture de " << nomfich << "\n";
  else
    cout << Name << " Ouverture du fichier de sauvegarde " << nomfich << endl;
  ostream fout(&buffloc);
   
  // on s'annonce
  fout << "#######     "<<nomfich<<"\n#######################\n";
   
  // on genere l'equivaelent du .status 
  // et non recopie du vieux, si on a lance plusieurs fois ds le meme dir!!

    Parametre::AllStatus( fout );

  // l'historique des fichiers d'init
  fout << "##############################\n";
  fout << "FICHIERS " << historiquec << endl;
  if (historiquec > 0)
    for (i=0; i<historiquec; i++)
      fout << " " <<  historique[i] << " " << nb_historique[i] << endl;

  // le nombre d'evaluations (ds ce run) pour en arriver la:
  fout << "##############################\n";
  fout << "EVALUATIONS " <<  meilleur->NbEval() << endl;
   
  // le nbre d'individus a ecrire
  fout << "##############################\n";
  fout << "INDIVIDUS  " << (int)recense << endl;
   
  // tri avant sauvegarde (sinon fichier illisible)
  trie_les_chromos(0, recense);
  // les individus eux-memes, avec niveau d'affichage -1
  int user_sav = chromosome::User;
  chromosome::User = -1;
  for (i=0; i<recense; i++)
    {
      fout << les_chromos[i]->perf() << "\t";
      les_chromos[i]->printout(fout) ;
      fout << endl;
    }
  chromosome::User = user_sav;
  Age_sauvegarde = nb_gen;	// pour ne pas la refaire au meme age
  return;
}

//----------------------------
void copopu::sauvegarde()
//----------------------------
{
   
  la_copopu->popu::sauvegarde();
  popu::sauvegarde();
   
}



//---------------------------
int popu::generation()
//----------------------------
{
  nb_gen++;
  if (!statistiques())		// renvoie test_arret(amelioration)
    return 0;

  dump_parents(cout);
  meilleur->strategie_avant(this); // et des operateurs genetiques
  evolution->boucle(this, recense); // recree recense offspring
  meilleur->strategie_apres(this);
  return 1;
}
