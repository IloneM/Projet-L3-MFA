// ----------------------------------------------------------------------
// Where........: CMAP - Polytechnique 
// File.........: replace.C
// Author.......: Marc Schoenauer
// Created......: 6 mai 95
// Description..: Implementation de la classe remplaceur
// 
// Ident........: $Id: replace.C,v 1.3 1998/03/10 13:13:42 marc Exp $
// ----------------------------------------------------------------------
#include <string.h>
#include <sstream>
using namespace std;
#include "myrand.h"
#include "appli.h"
#include "popu.h"
#include "parametre.h"
#include "replace.h"

remplaceur* choix_remp(char* pts, int taille)
{
   // csel contient le nom du remplaceur :

   remplaceur* psel;
   if (!strcasecmp(pts,"TOURNOI"))
      psel =  (remplaceur *)new tournoiR(taille);
   else if (!strcasecmp(pts,"EP_TOURNOI"))
     psel = (remplaceur *)new ep_tournoi(taille);
    else if (!strcasecmp(pts,"DETERMINISTE"))
       psel =  (remplaceur *)new rep_determin(taille);
   else
   {
     cout << "Remplaceur inconnu : " << pts << endl;
      cout << "Choisir parmi DETERMINISTE, EP_TOURNOI ou TOURNOI\n";
      exit(1);
   }
   return psel;
}


//////////////////////////////////////////
//        class remplaceur  
//////////////////////////////////////////
remplaceur::remplaceur(int t):taille_pop(t), taille_res(0),resultats (0)
{}
remplaceur::~remplaceur()
{ if (resultats) delete [] resultats;}

void remplaceur::verif(int debut, int nb, int nb_a_trouver, int val)
{
  int i;
  if (nb_a_trouver > nb)
    {
      cout << "Grave erreur ds verif: Nb a selectionner " << nb_a_trouver << "  > Nb totoal " << nb << endl;
      // il faudrait ici un longjump vers la procedure d'arret
      exit(1);
    }
  if (taille_res < debut+nb)
    {
    if (resultats) delete [] resultats;
    resultats = new int[debut+nb];
    taille_res = debut+nb;
    }
  for (i=0; i<debut; i++)
    resultats[i]=1;		// les debut premiers sont preserves !
  for (i=debut; i<debut+nb; i++) // les nb suivants participent au combat
    resultats[i]=val;
}

//////////////////////////////////////////
//        class tournoi R: tournoi de remplacement
//////////////////////////////////////////
   
//-------------------------------------------
tournoiR::tournoiR(int t):remplaceur(t), taille_restants(0), restants(0)
//-------------------------------------------
{
  taille_tournoi = (int)* new IntParametre("TAILLE_TOURNOI", "Taille du tournoi", 2);
}

//-------------------------------------------
tournoiR::tournoiR(int t, int tt):remplaceur(t),taille_restants(0), restants(0),taille_tournoi(tt)
//-------------------------------------------
{}

//-------------------------------------------
int* tournoiR::trouve_les_gagnants(popu* pop, int debut, int nb, int nb_gagnants)
//-------------------------------------------
{
  int i, nbopp, res, le_gagnant, indice_gagnant, challenger, indice_challenger;
  verif(debut, nb, nb_gagnants, 0);
  // il faut faire nb_gagnants tournois successifs, sans retirer les memes !
  if (taille_restants < nb)
    {
      if (restants) delete [] restants;
      restants = new int[nb];
      taille_restants = nb;
    }
  for (i=0; i<nb; i++)
    restants[i]=i+debut;	// indice ds la population --> debut + ...
  for (i=0; i<nb_gagnants; i++)
    {
      nbopp = 0;		// nbre d'opposants
      indice_gagnant = rnd(nb-i);	// un au hasard
      le_gagnant = restants[indice_gagnant];	// ... parmi ceux non-encore selectionnes
      while(nbopp++ < taille_tournoi)		// taille du tournoiR
	{
	  indice_challenger = rnd(nb-i);
	  challenger = restants[indice_challenger];
	  if ( (res = pop->comp_chromos(challenger, le_gagnant) ) > 0 )
	    {
	      indice_gagnant = indice_challenger;
	      le_gagnant = challenger;
	    }
	  else
	    if (res == 0)
	      if (flip(0.5))	// au hasard l'un des 2
		{
		  indice_gagnant = indice_challenger;
		  le_gagnant = challenger;
		}
	}
      // mise a jour des resultats et de restants
      resultats[le_gagnant]=1;
      restants[indice_gagnant] = restants[nb-i-1];
    }
  return resultats;
}

//-------------------------------------------
int* tournoiR::trouve_les_perdants(popu* pop, int debut, int nb, int nb_perdants)
//-------------------------------------------
{

  int i, nbopp, res, indice_perdant, le_perdant, challenger, indice_challenger;
  verif(debut, nb, nb_perdants, 1);
  // il faut faire nb_perdants tournois successifs, sans retirer les memes !
  if (taille_restants < nb)
    {
      if (restants) delete [] restants;
      restants = new int[nb];
      taille_restants = nb;
    }
  for (i=0; i<nb; i++)
    restants[i]=i+debut;	// indice ds la population --> debut + ...
  for (i=0; i<nb_perdants; i++)
    {
      nbopp = 0;		// nbre d'opposants
      indice_perdant = rnd(nb-i);	// un au hasard
      le_perdant = restants[indice_perdant];	// ... parmi ceux non-encore selectionnes
      while(nbopp++ < taille_tournoi)		// taille du tournoiR
	{
	  indice_challenger = rnd(nb-i);
	  challenger = restants[indice_challenger];
	  if ( (res = pop->comp_chromos(challenger, le_perdant) ) > 0 )
	    {
	      indice_perdant = indice_challenger;
	      le_perdant = challenger;
	    }
	  else
	    if (res == 0)
	      if (flip(0.5))	// au hasard l'un des 2
		{
		  indice_perdant = indice_challenger;
		  le_perdant = challenger;
		}
	}
      // mise a jour des resultats et de restants
      resultats[le_perdant]=0;
      restants[indice_perdant] = restants[nb-i-1];
    }
  return resultats;
}
//-------------------------------------------
int tournoiR::le_perdant(popu* pop, int debut, int nb) // renvoie le No du chrom. a virer
//-------------------------------------------
{
   int res, challenger, 
      le_gagnant = debut+rnd(nb);

   while(nb++ < taille_tournoi)		// taille du tournoiR
   {
      challenger = debut+rnd(nb);
      if ( (res = pop->comp_chromos(challenger, le_gagnant) ) < 0 )
	 le_gagnant = challenger;
      else
	 if (res == 0)
	    if (flip(0.5))	// au hasard l'un des 2
		   le_gagnant = challenger;
   }
   return le_gagnant;
}

   
//////////////////////////////////////////
//        class rep_determin
//////////////////////////////////////////
   
//-------------------------------------------
int* rep_determin::trouve_les_gagnants(popu* pop, int debut, int nb, int nb_gagnants)
//-------------------------------------------
{
  verif(debut, nb, nb_gagnants, 0);		// met des 0 partout (et verifie la taille
  pop->trie_les_chromos(debut, nb); 
  for (int i=debut; i<debut+nb_gagnants; i++) // les nb_gagnants premiers -> 1
    resultats[i]=1;
  return resultats;
}

//-------------------------------------------
int* rep_determin::trouve_les_perdants(popu* pop, int debut, int nb, int nb_perdants)
//-------------------------------------------
{
  verif(debut, nb, nb_perdants, 1);		// met des 1 partout (et verifie la taille
  pop->trie_les_chromos(debut, nb); 
  for (int i=nb-1; i>nb-1-nb_perdants; i--)    // les nb_perdants derniers --> 0
    resultats[i]=0;
  return resultats;
}

//-------------------------------------------
int rep_determin::le_perdant(popu* pop, int debut, int nb) // renvoie le No du chrom. a virer
//-------------------------------------------
{
  int res, iworse=debut;
  if (nb > 1)
    for (int i=debut+1; i<debut+nb; i++)
      if ( (res = pop->comp_chromos(iworse, i) ) > 0 ) // iworse better than i
	iworse = i;
      else
	if (res == 0)
	  if (flip(0.5))	// au hasard l'un des 2
	    iworse = i;
  return iworse;
}

//////////////////////////////////////////
//        class ep_tournoi
//////////////////////////////////////////
ep_tournoi::ep_tournoi(int t):remplaceur(t), le_rang(0), la_perf_ep(0)
{
  taille_tournoi = (int) * new IntParametre("TAILLE_EP","Taille du tournoi stochstique EP_like", 6);
}

ep_tournoi::ep_tournoi(int t, int tt):remplaceur(t), taille_tournoi(tt), le_rang(0), la_perf_ep(0)
{}


//-------------------------------------------
void ep_tournoi::verif(int debut, int nb, int nb_a_trouver, int val)
//-------------------------------------------
{
  if (taille_res < debut+nb)
    {
    if (la_perf_ep) delete [] la_perf_ep;
    if (le_rang) delete [] le_rang;
    la_perf_ep = new int[debut+nb];
    le_rang = new int[debut+nb];
    }
  remplaceur::verif(debut, nb, nb_a_trouver, val); // met a jour taille_res
}

// pour trier le tableau le_rang a partir des perf tournoi
// contenues ds la_perf_ep
//-------------------------------------------
int *le_rang_glob;
int *la_perf_ep_glob;
int comp_perf_ep(const void* v1, const void* v2)
//-------------------------------------------
{
   int i1 = *(int *)v1;
   int i2 = *(int *)v2;
   if ( la_perf_ep_glob[i1] > la_perf_ep_glob[i2] )
      return -1;
   else
      if (la_perf_ep_glob[i1] < la_perf_ep_glob[i2])
	 return +1;
      else
	 return 0;
}


//-------------------------------------------
void ep_tournoi::trie_ep(popu* pop, int debut, int nb_chromos)
//-------------------------------------------
{
  int res, ichrom, iopp, opp, score;
  for (ichrom=debut; ichrom<debut+nb_chromos; ichrom++)
    le_rang[ichrom] = ichrom;
  for (ichrom=debut; ichrom<debut+nb_chromos; ichrom++)
    {
      score=0;
      for (iopp=0; iopp<taille_tournoi; iopp++)
	{
	  opp = debut+rnd(nb_chromos);
	  if ( (res=pop->comp_chromos(ichrom, opp)) > 0)
	    score += 2;
	  else if (res==0)
	    score++;
	}
      la_perf_ep[ichrom] = score;
    }
  // a cause du passage en argument, utilisation des globales...
  le_rang_glob = le_rang;
  la_perf_ep_glob = la_perf_ep;
   qsort(le_rang+debut, nb_chromos, sizeof(int), comp_perf_ep);
}

//-------------------------------------------
int* ep_tournoi::trouve_les_gagnants(popu* pop, int debut, int nb, int nb_gagnants)
//-------------------------------------------
{
  verif(debut, nb, nb_gagnants, 0);		// met des 0 partout (et verifie la taille
  trie_ep(pop, debut, nb); 
  for (int i=debut; i<debut+nb_gagnants; i++) // les nb_gagnants premiers -> 1
    resultats[le_rang[i]]=1;
  return resultats;
}

//-------------------------------------------
int* ep_tournoi::trouve_les_perdants(popu* pop, int debut, int nb, int nb_perdants)
//-------------------------------------------
{
  verif(debut, nb, nb_perdants, 1);		// met des 1 partout (et verifie la taille
  trie_ep(pop,debut, nb); 
  for (int i=nb-1; i>nb-1-nb_perdants; i--)    // les nb_perdants derniers --> 0
    resultats[le_rang[i]]=0;
  return resultats;
}

//-------------------------------------------
int ep_tournoi::le_perdant(popu* pop, int debut, int nb) // renvoie le No du chrom. a virer
//-------------------------------------------
{
  int res, ichrom, score,
    iworse=debut, 
    worse_score=2*nb;	// aucun score nepeut depasser 2*nb
  int iopp, opp;
  if (nb > 1)
  // on fait comme ds trie_ep, masi sans trier
  for (ichrom=debut; ichrom<debut+nb; ichrom++)
    {
      score=0;
      for (iopp=0; iopp<taille_tournoi; iopp++)
	{
	  opp = debut+rnd(nb);
	  if ( (res=pop->comp_chromos(ichrom, opp)) > 0)
	    score += 2;
	  else if (res==0)
	    score++;
	}
      if (score < worse_score)
	{
	  worse_score=score;
	  iworse = ichrom;
	}
      else
	if (score == worse_score)
	  if (flip(0.5))
	    {
	      worse_score=score;
	      iworse = ichrom;
	    }
    }
  return iworse;
}
