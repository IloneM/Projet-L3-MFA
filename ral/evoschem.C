// ----------------------------------------------------------------------
// Where........: CMAP - Polytechnique 
// File.........: evoschem.C
// Author.......: Marc Schoenauer
// Created......: 20/4/96
// Description..: Implementation de la classe evoschem : schema d'evolution
// 
// Ident........: $Id: evoschem.C,v 1.2 1997/04/28 12:37:41 marc Exp $
// ----------------------------------------------------------------------
#include <string.h>
#include <sstream>
#include "popu.h"		// contient chromosome.h, mystring.h et parametre.h
#include "sel.h"
#include "replace.h"
#include "appli.h"
#include "myrand.h"
#include "evoschem.h"
#include "parametre.h"

evoschem* ChoixEvolution(const char * EvoName, const int taille)
{
  evoschem* ptloc;
  if (!strcasecmp(EvoName, "GAG"))
    ptloc = (evoschem*) new gag(taille);
  else if (!strcasecmp(EvoName, "SSGA"))
    ptloc = (evoschem*) new ssga(taille);
  else if (!strcasecmp(EvoName, "ES+"))
    ptloc = (evoschem*) new esplus(taille);
  else if (!strcasecmp(EvoName, "ES,"))
    ptloc = (evoschem*) new escomma(taille);
  else if (!strcasecmp(EvoName, "EP"))
    ptloc = (evoschem*) new ep(taille);
  else if (!strcasecmp(EvoName, "TOTAL"))
    ptloc = (evoschem*) new totale(taille);
  else
    {
      cout << "Schema d'evolution inconnu : " <<  EvoName << endl;
      AppliHelp(0,0);		// et exit
      exit(1);
    }
  return ptloc;
}

////////////////////////////////////////////////////////////////////
// class evoschem generique
////////////////////////////////////////////////////////////////////
evoschem::~evoschem()
{}


chromosome** evoschem::get_les_chromos(popu* pop)
// pour manipualtion locale 
{ 
  return pop->les_chromos;
}

void maj_chromos(int* resultats, chromosome** les_chromos, int indmax)
// resultats contient 0 ou 1 suivant que le chromo correspondant est a detruire
// oou a conserver. On fait le boulot.
{
  int ichrom, deleted=0, verif=0;
  // d'abord on detruit tout pour pouvoir recopier ss vergogne ensuite
  for (ichrom=0; ichrom<indmax; ichrom++)
    if (!resultats[ichrom])
      {
	delete les_chromos[ichrom];
	deleted++;
      }
  // puis on recopie -si besoin est - en partant du haut
  int index_del=-1;		// index_del est sur le dernier trou bouche:
  ichrom = indmax;		// et ichrom sur le dernier qui a ete recopie
  while (index_del < ichrom)
    {
      while(resultats[++index_del]) // positionner index_del sur le trou suivant
	;
      while (!resultats[--ichrom]) // recherche du suivant a recopier
	;
      les_chromos[index_del]=les_chromos[ichrom]; // recopioe ds le trou
    }
}

////////////////////////////////////////////////////////////////////
// class gag : generationnel algorithme genetique
////////////////////////////////////////////////////////////////////
// sancta simplicitas

//-----------------------------------------
gag::gag(unsigned int taille):evoschem(taille)
//-----------------------------------------
{
  WordParametre SelName= * new WordParametre("SELECTEUR", "Type de selection (roulette, rang ou tournoi", "TOURNOI");
  param_elitisme = (int) * new IntParametre("ELITISME", "Remise du meilleur si disparu (0/1)", 1);
  char *sn = strdup((char *)SelName);
  selector = choix_sel(sn, taille);
  selector_mate = selector->clone(); // Pour avoir des tirages independants
  taille_pop=taille;
}

//-----------------------------------------
gag::~gag()
//-----------------------------------------
{
  delete selector; 
  delete selector_mate;
}

//-----------------------------------------
void gag::boucle(popu* pop, int nb_chromos)
//-----------------------------------------
// nb_chromos est le nombre viables (<= taille_pop)
{
  int ichrom, elu, offspring;
  chromosome ** les_chromos = get_les_chromos(pop);
  selector->prepare_select(pop, nb_chromos);
  selector_mate->prepare_select(pop, nb_chromos);
  for (ichrom=0; ichrom<nb_chromos; ichrom++)
    {
      elu = selector->select();
      offspring = ichrom + nb_chromos;
      les_chromos[offspring] = les_chromos[elu]->_clone();
      les_chromos[offspring]->operateurs(pop, nb_chromos, selector_mate);
    }
  // remplacement boeuf
  pop->rend_meilleur()->strategie_pendant(pop);
  for (ichrom=0; ichrom<nb_chromos; ichrom++)
    {
      delete les_chromos[ichrom];
      les_chromos[ichrom] = les_chromos[ichrom+nb_chromos];
    }
}



////////////////////////////////////////////////////////////////////
// class ssga : steady state algorithme genetique
////////////////////////////////////////////////////////////////////
// boucle = 1 choix + 1 remplacement
//-----------------------------------------
ssga::ssga(unsigned int taille):evoschem(taille)
//-----------------------------------------
{
  int tt = (int) * new IntParametre("TAILLE_SELECT", "Taille du tournoi pour la selection", 2);
  selector = new tournoi(taille, tt);
  tt = (int) * new IntParametre("TAILLE_MATE", "Taille du tournoi de choix de partenaire", 2);
  selector_mate = new tournoi(taille, tt);
  tt = (int) * new IntParametre("TAILLE_REPLACE", "Taille du tournoi pour le remplacement", 2);
  replacor = new tournoiR(taille, tt);

  taille_pop=taille;
}


//-----------------------------------------
ssga::~ssga()
//-----------------------------------------
{
  delete selector; 
  delete selector_mate;
  delete replacor;
}

//-----------------------------------------
void ssga::boucle(popu* pop, int nb_chromos)
//------------------------------------------------------
{
  int elu, offspring;
  chromosome ** les_chromos = get_les_chromos(pop);
  selector->prepare_select(pop, nb_chromos);
  selector_mate->prepare_select(pop, nb_chromos);
  offspring = nb_chromos;
  elu = selector->select();
  les_chromos[offspring] = les_chromos[elu]->_clone();
  les_chromos[offspring]->operateurs(pop, nb_chromos, selector_mate);
  pop->rend_meilleur()->strategie_pendant(pop);
  int malheureux_elu = replacor->le_perdant(pop, 0, nb_chromos);
  delete les_chromos[malheureux_elu];
  les_chromos[malheureux_elu] = les_chromos[offspring];
}

////////////////////////////////////////////////////////////////////
// class es : Evolution Strategy, type generique
////////////////////////////////////////////////////////////////////
int es::max_offspring()
{return lambda + taille_pop;}

//-----------------------------------------
es::es(unsigned int taille):evoschem(taille)
//-----------------------------------------
{
  lambda = (int) * new IntParametre("LAMBDA", "Nombre d'enfants", 15);
  taille_pop=taille;
  selector = new equireparti(taille);
  selector_mate = new aleatoire(taille);
}

////////////////////////////////////////////////////////////////////
// class es+ : Evolution Strategy, type (mu+lambda)
////////////////////////////////////////////////////////////////////
void esplus::boucle(popu* pop, int nb_chromos)
{
  int ichrom, index;
  // la meilleure facon de tirer au hasard uniformement, 
  // sachant qu'ilpeut y avoir plusieurs enfants par parent:
  // les melanger, puis le sprendre ds l'ordre du melange
  chromosome ** les_chromos = get_les_chromos(pop);
  //  selector->prepare_select(pop, nb_chromos);
  selector_mate->prepare_select(pop, nb_chromos);
  index=0;
  for (ichrom=0; ichrom<lambda; ichrom++)
    {
      //      index=selector->select();
      les_chromos[nb_chromos+ichrom] = les_chromos[index]->_clone();
      // attention aux operateurs globaux
      les_chromos[ichrom+nb_chromos]->operateurs(pop, nb_chromos, selector_mate);
      
      //      printf("Les performances de %d --> %g\n",ichrom + nb_chromos, les_chromos[ichrom+nb_chromos]->perf());
      index++;
      if (index >= nb_chromos)
	index=0;
    }
  // pour virer les effets de bord de qsort sur les ex-aequo
  melange((void **)les_chromos,nb_chromos+lambda); 
  pop->trie_les_chromos(0, nb_chromos+lambda);
  pop->rend_meilleur()->strategie_pendant(pop);
  for (index=nb_chromos; index<lambda+nb_chromos; index++)
    delete les_chromos[index];
}

////////////////////////////////////////////////////////////////////
// class escomma : Evolution Strategy, type (mu,lambda)
////////////////////////////////////////////////////////////////////
escomma::escomma(unsigned int t):es(t)
{
  if (lambda < taille_pop)
    {
      cerr << "Erreur, Lambda < Mu ds une ES comma. On ajuste Lambda=Mu\n";
      lambda = taille_pop;
    }
}

void escomma::boucle(popu* pop, int nb_chromos)
{
  int ichrom, index;
  chromosome** les_chromos = get_les_chromos(pop);
  //  selector->prepare_select(pop, nb_chromos);
  selector_mate->prepare_select(pop, nb_chromos);
  index = 0;
  for (ichrom=0; ichrom<lambda; ichrom++)
    {
      //      index=selector->select();
      les_chromos[nb_chromos+ichrom] = les_chromos[index]->_clone();
      // attention aux operateurs globaux
      les_chromos[ichrom+nb_chromos]->operateurs(pop, nb_chromos, selector_mate);
      index++;
      if (index >= nb_chromos)
	index=0;
    }
  for (index=0; index<nb_chromos; index++)
    delete les_chromos[index];
  for (index=0; index<lambda; index++)
    les_chromos[index] = les_chromos[index+nb_chromos];
  melange((void **)les_chromos,lambda); 
  pop->trie_les_chromos(0, lambda);
  pop->rend_meilleur()->strategie_pendant(pop);
  if (lambda>nb_chromos)	// il en reste a detruire
    for (index=nb_chromos; index<lambda; index++)
      delete les_chromos[index];
}


////////////////////////////////////////////////////////////////////
// class ep : Evolutionary Programming de basee
////////////////////////////////////////////////////////////////////

//-----------------------------------------
ep::ep(unsigned int taille):evoschem(taille)
//-----------------------------------------
{
  taille_pop=taille;
  selecteur_mate = new aleatoire(taille);
  replacor = new ep_tournoi(taille); // lit son propre parametre
}


//-------------------------------------------
void ep::boucle(popu* pop, int nb_chromos)
//-------------------------------------------
{
  int ichrom;
  chromosome** les_chromos= get_les_chromos(pop);
  for (ichrom=0; ichrom<nb_chromos; ichrom++)
    {
      les_chromos[ichrom+nb_chromos] = les_chromos[ichrom]->_clone();
      les_chromos[ichrom+nb_chromos]->operateurs(pop, nb_chromos, selecteur_mate);
    }
  pop->rend_meilleur()->strategie_pendant(pop);
  int* resultats=replacor->trouve_les_perdants(pop, 0, 2*nb_chromos, nb_chromos);
  maj_chromos(resultats, les_chromos, 2*nb_chromos);
}



////////////////////////////////////////////////////////////////////
// class totale: tout est possible
////////////////////////////////////////////////////////////////////

totale::~totale()
{
  delete selector;
  delete selector_mate;
  delete selector_local;
  delete replacor;
}


totale::totale(unsigned int taille):evoschem(taille)
{
  taille_pop=taille;
  ParamList * PSav = Parametre::getCurrentList();
  ParamList * PLoc = new ParamList(0, 0, "Parametres d'evolution", PSav);
  taux = (unsigned int) * new UintParametre( 
					    "TAUX", "nombre d'enfants par parent", 1 );
  fertile = (unsigned int) * new UintParametre( 
					       "FERTILE", 
					       "nombre ou pourcentage autorise a se reproduire", 
					       "100 %", 
					       &taille_pop );
  immobilisme = (unsigned int) * new UintParametre( 
						   "IMMOBILISME", 
						   "nombre ou pourcentage de parents survivants ss modif", 
						   "0 %", 
						   &taille_pop );
  param_elitisme = (int) * new IntParametre( "ELITISME", 
					     "Remise du meilleur si pas ameliore", 1 );

  // les selecteurs
  // il faut une ParamList dediee a chaque selecteur (parametres avec meme nom)
  PLoc = new ParamList(0, 0, "Selecteur initial", PSav);
  WordParametre SelIni = * new WordParametre(
					     "SEL_INI", 
					     "Selecteur initial (uniforme, roulette, rang ou tournoi",
					     "RANG");
  PLoc = new ParamList("sel_ini", 0, 0, PLoc);
  selector = choix_sel(SelIni, taille);

  // selecteur croisement
  PLoc = new ParamList(0, 1, "Selecteur de croisement", PSav);
  WordParametre SelCrois = * new WordParametre( 
					       "SEL_CROIS", 
					       "Selecteur de croisement (RIEN=le meme que initial", 
					       "RIEN" );
  PLoc = new ParamList("sel_crois", 1, 0, PLoc);
  if (strcasecmp(SelCrois, "RIEN")) // pas rien -> il y a un selecteur croisement
    selector_mate = choix_sel(SelCrois, taille);
  else
    selector_mate = selector->clone();

  // Selectio locale
  PLoc = new ParamList(0, 2, "Selecteur parent-enfants", PSav);
  WordParametre SelLocal = * new WordParametre( 
					       "SEL_LOCAL", 
					       "Selecteur parent-enfants (RIEN = pas de selection", 
					       "RIEN" );
  PLoc = new ParamList("sel_local", 2, 0, PLoc);
  if (strcasecmp(SelLocal, "RIEN")) // il y a un selecteur local
    {
      conflit_local = (int) * new IntParametre( 
					       "CONFLIT_LOCAL", 
					       "Les parents participent-ils au conflit local ?", 1 );
      selector_local = choix_sel(SelLocal, taille);
    }
  else
    selector_local = 0;

  // Selectio finale (remplacement)
  PLoc = new ParamList(0, 3, "Selecteur final", PSav);
  WordParametre SelFinal = * new WordParametre( 
					       "REMPLACEMENT", 
					       "selecteur final (tournoi, EP_tournoi, deterministe)", 
					       "DETERMINISTE" );
  PLoc = new ParamList("sel_final", 3, 0, PLoc);
  replacor = choix_remp(SelFinal, taille);
  conflit_generations = (int) * new IntParametre( 
						 "CONFLIT_FINAL", 
						 "Les parents participent-ils au conflit final ?", 0 );
  Parametre::setCurrentList(PSav); // remettre tout en place
}



int totale::max_offspring()
{
  return taille_pop*(taux+1);
}

//-----------------------------------------
void totale::selection_locale(popu* pop, int parent, int position)
//-----------------------------------------
// les fils de parent st ds les_chromos a partir de position
// on fait la selection, et met le gagnant em premier, en
// detruisant les autres
{
  int nb_tournoi=taux;
  chromosome** les_chromos = get_les_chromos(pop);
  if (conflit_local)		// le pere participe au massacre
    {
      les_chromos[position+taux]=les_chromos[parent]->_clone();
      nb_tournoi = taux+1;
    }
  selector_local->prepare_select(pop, nb_tournoi, position);
  int rescape = selector_local->select();
  for (int ichrom=position; ichrom<position+nb_tournoi; ichrom++)
    if (ichrom != rescape)
      delete les_chromos[ichrom];
  les_chromos[position]=les_chromos[rescape];
}

//-----------------------------------------
void totale::boucle(popu* pop, int nb_chromos)
//-----------------------------------------
{
  int ichrom, index, offspring, elu, ioff, nb_offspring, nb_parents;
  int *res;

  pop->trie_les_chromos(0, nb_chromos);	// a cause de fertile et immobilisme
  chromosome ** les_chromos = get_les_chromos(pop);
  
  // seuls les fertiles premiers participent
  selector->prepare_select(pop, fertile); 
  selector_mate->prepare_select(pop, fertile);

  offspring=nb_chromos;	// rangement des offspring ds les_chromos
  for (ichrom=0; ichrom<fertile; ichrom++)
    {
      index = offspring;	// sauve a cause de la selection locale
      elu=selector->select();
      for (ioff=0; ioff<taux; ioff++)
	{
	  les_chromos[offspring] = les_chromos[elu]->_clone();
	  // attention aux operateurs globaux
	  les_chromos[offspring]->operateurs(pop, fertile, selector_mate);
	  offspring++;
	}
      if (selector_local)
	{
	  selection_locale(pop, elu, index);
	  offspring=index+1;	// il n'en reste qu'un !
	}
    }
  nb_offspring = offspring-nb_chromos;
  nb_parents = nb_chromos-immobilisme;

  // 3 possibilites:
  //    nb_parents > nb_enfants: 
  //         - on garde tous les enfants (et vire donc les + mauvais parents)
  //         - on fait qd meme un tournoi parents+enfants
  //    nb_parents < nb_enfants
  //         - on vire les parents et on fait un tournoi enfants
  //         - on fait un tournoi parents+enfants
  //    (le conflit parents+enfants est le meme)

  if (conflit_generations)	// parents_enfants se battent
    {
      res = replacor->trouve_les_gagnants(pop, immobilisme, 
					  nb_parents+nb_offspring, nb_chromos-immobilisme);
      maj_chromos(res, les_chromos, offspring);
      return;
    }
  pop->rend_meilleur()->strategie_pendant(pop);

  // il n'y plus conflit parents-enfants: tout depend des nombres respectifs
  if (nb_parents > nb_offspring)// virer les mauvais parents
    {
      res = replacor->trouve_les_perdants(pop, immobilisme, 
					  nb_parents, nb_offspring);
      index = nb_chromos;		// debut des offspring
      for (ichrom=immobilisme; ichrom<nb_chromos; ichrom++)
	if (!res[ichrom])	// a virer
	  {
	    delete les_chromos[ichrom];
	    les_chromos[ichrom] = les_chromos[index++];
	  }
    }
  else   	// ici, nb_parents <= nb_offspring: selection parmi les enfants
    {
      // on detruit les parents
      for (ichrom=immobilisme; ichrom<nb_chromos; ichrom++)
	delete les_chromos[ichrom];
      // on recopie les enfants aux places libres
      for (ichrom=0; ichrom<nb_offspring; ichrom++)
	les_chromos[ichrom+immobilisme] = les_chromos[nb_chromos+ichrom];
      res = replacor->trouve_les_gagnants(pop, immobilisme, 
					  nb_offspring, nb_chromos-immobilisme);
      maj_chromos(res, les_chromos, nb_offspring+immobilisme);
    }
}

