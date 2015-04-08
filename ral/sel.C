// ----------------------------------------------------------------------
// Where........: CMAP - Polytechnique 
// File.........: sel.C
// Author.......: Marc Schoenauer
// Created......: Tue Mar 21 14:35:07 1995
// Description..: Implementation de la classe selecteur
// 
// Ident........: $Id: sel.C,v 1.10 1998/03/10 13:13:04 marc Exp $
// ----------------------------------------------------------------------
#include <string.h>
#include <sstream>
using namespace std;
#include "myrand.h"
#include "appli.h"
#include "popu.h"
#include "sel.h"

selecteur* choix_sel(char* pts, int taille)
{
   // csel contient le nom du selecteur :

   selecteur* psel;
   if (!strcasecmp(pts,"ALEATOIRE"))
      psel = (selecteur *)new aleatoire(taille);
   if (!strcasecmp(pts,"EQUIREPARTI"))
      psel = (selecteur *)new equireparti(taille);
   //    else if (!strcasecmp(pts,"ROULETTE"))
   //    psel =  (selecteur *)new roulette(taille);
    else if (!strcasecmp(pts,"RANG"))
       psel =  (selecteur *)new rang(taille);
   else if (!strcasecmp(pts,"TOURNOI"))
      psel =  (selecteur *)new tournoi(taille);
    else if (!strcasecmp(pts,"RIEN"))
       psel =  (selecteur *)0;
   else
   {
     cout << "Selecteur inconnu : "<<  pts << endl;
      cout << "Choisir parmi ALEATOIRE, EQUIREPARTI, RANG, TOURNOI ou RIEN\n";
      exit(1);
   }
   return psel;
}


//////////////////////////////////////////
//        class selecteur  de base
//////////////////////////////////////////
//  2 constructeurs: avec la taille, lit les parametres
//                   a partir d'un autre selecteur, sans lecture parametres

selecteur::selecteur(int t):taille_pop(t),
  taille_locale(0),debut_local(0){}
selecteur::selecteur(selecteur* s):taille_pop(s->taille_pop),
  taille_locale(0),debut_local(0){}
selecteur::~selecteur(){}
	 
//////////////////////////////////////////
//        class aleatoire 
//////////////////////////////////////////
//-------------------------------------------
selecteur* aleatoire::clone()   
//-------------------------------------------
{
    aleatoire* ptloc = new aleatoire(taille_pop);
    return (selecteur*)ptloc;
}

//-------------------------------------------
void aleatoire::prepare_select(popu*, int tloc, int debut)   
//-------------------------------------------
{
   taille_locale = tloc;
   debut_local = debut;
}

//-------------------------------------------
int aleatoire::select()
//-------------------------------------------
// random entre debut et debut+taille_locale
{
   return debut_local + rnd(taille_locale);	
}


//////////////////////////////////////////
//        class equireparti 
//////////////////////////////////////////
//-------------------------------------------
selecteur* equireparti::clone()   
//-------------------------------------------
{
    equireparti* ptloc = new equireparti(taille_pop);
    return (selecteur*)ptloc;
}

  // la meilleure facon de tirer au hasard uniformement, 
  // sachant qu'ilpeut y avoir plusieurs enfants par parent:
  // les melanger, puis le sprendre ds l'ordre du melange

//-------------------------------------------
void equireparti::prepare_select(popu*, int tloc, int debut)   
//-------------------------------------------
{
   if (taille_locale < tloc)
     {
       if (le_rang) delete [] le_rang;
       le_rang = new int[tloc];
     }
   taille_locale = tloc;
   remplit_random(le_rang, tloc);
   index_local=0;		// pointe toujours sur le suivant a renvoyer
   debut_local = debut;
}

//-------------------------------------------
int equireparti::select()	// renvoie le No du chrom. selecteurne
//-------------------------------------------
{
  int ret=le_rang[index_local];		// pointe toujours sur le suivant a renvoyer
  index_local++;
  if (index_local >= taille_locale)
    index_local=0;
   return debut_local+ret;
}

//////////////////////////////////////////
//        class proportionnel
//////////////////////////////////////////
// attention a mettre a 0 la taille_fitness_tmp, sinon delete plantera ds verif
//-------------------------------------------
proportionnel::proportionnel(int taille):selecteur(taille), taille_fitness_tmp(0)
//-------------------------------------------
{
  pression = (double) * new DoubleParametre("PRESSION", "Pression selective", 2.0);
  facteur_de_partage = (double) * new DoubleParametre ( "PARTAGE", "Facteur de partage", 0.0 );
  char * Wtyp_scaling = (char*) * new WordParametre("TYP_SCALING", "Type de scaling (Lineaire, exponentielle, dynamique (Michalewicz))", "lineaire");
  if (!strcasecmp(Wtyp_scaling, "lineaire"))
    typ_scaling = LINEAIRE;
  else if (!strcasecmp(Wtyp_scaling, "exponentiel"))
    typ_scaling = EXPONENTIEL;
  else if (!strcasecmp(Wtyp_scaling, "dynamique"))
    typ_scaling = DYNAMIC;
  else 
   {
     cout << "Type de mise a l'echelle inconnu : " << (char*)Wtyp_scaling << endl;
      AppliHelp(0,0);		// et exit
   }

}

//-------------------------------------------
proportionnel::proportionnel(proportionnel* ps):selecteur(ps)
//-------------------------------------------
{
  pression = ps->pression;
  facteur_de_partage = ps->facteur_de_partage;
  typ_scaling = ps->typ_scaling;
  taille_fitness_tmp = ps->taille_fitness_tmp;
}

//-------------------------------------------
void proportionnel::verif(int nbre_a_selectionner)
//-------------------------------------------
// verification des allocations memoires
// a faire avant remplissage fitness_tmp !
{
  if (taille_fitness_tmp < nbre_a_selectionner) {
    if (taille_fitness_tmp) {	// c'etait deja alloue
      delete[] fitness_tmp;
      delete [] tab_select;
    }
    fitness_tmp = new double[nbre_a_selectionner];
    tab_select = new int[nbre_a_selectionner];
  }
}

double *les_simil=0;
int taille_simil=0;
//----------------------------------------------
void proportionnel::maj_simil(popu* pop, int tloc, int debut)
//----------------------------------------------
{
   int i,j;
   double dloc ;
   int fin=tloc+debut;
   if (taille_simil < fin*fin)
     {
       if (les_simil) delete [] les_simil;
       taille_simil = fin*fin;
       les_simil = new double[taille_simil];
     }
   les_simil[fin*fin-1]=1;
   for(i=debut; i<fin-1; i++)
   {
      les_simil[i*fin+i] = 1;            // share(soi-meme) = 1
      for (j=i+1; j<fin; j++)
      {
	 dloc = pop->GetChromo(i)->dist(pop->GetChromo(j)); 
	 les_simil[i*fin+j] = les_simil[j*fin+i] = 
	    (dloc<facteur_de_partage ? 1-dloc/facteur_de_partage : 0);
      }
   }
   return ;
}

//----------------------------------------------
void proportionnel::prepare_sharing(popu* pop, int tloc, int debut)
//----------------------------------------------
{
   int i,j;
   double dloc;
  if (facteur_de_partage==0)
    return;

   int fin=tloc+debut;
   maj_simil(pop, tloc, debut);	// met le tableau les_simil a jour
   la_somme=0;
   for(i=debut; i<fin; i++)
     {
       dloc = 0. ;
       for (j=debut; j<fin; j++)
	 dloc += les_simil[i*fin+j];
       la_somme += ( fitness_tmp[i] = pop->GetChromo(i)->perf() / dloc ) ;
     }
   return;
}


//-------------------------------------------
void proportionnel::prepare_select(popu* pop, int tloc, int debut)
//-------------------------------------------
{
  debut_local = debut;		// tout est decale de debut
  taille_locale=tloc;

  // remplissage de fitness_tmp: specifique
  remplit_fitness_tmp(pop, tloc, debut);

  // sharing eventuellement
  prepare_sharing(pop, tloc, debut);

  // mise a l'echelle
  //  mae();

  // tirage effectif:
  // une roulette avec P marques (reste stochastique).
  // on suppose ici que le tableau fitness_tmp et la_somme sont corrects
  // et on remplit tab_select (pour pouvoir le melanger ensuite)

  double pas = la_somme/taille_locale; // distance entre 2 marques sur la roulette
  double rloc=0.0;
  // on met ds fitness_tmp les sommes cumulees
  for (int i=0; i<taille_locale; i++)
    {
      rloc += fitness_tmp[i];
      fitness_tmp[i]=rloc;
    }
  double target = la_somme * myrandom();
  int icourant = 0;
  for (int index=0; index<taille_locale; index++)
    {
      while (fitness_tmp[icourant] < target)
	{
	  icourant++;
	  if (icourant == taille_locale) // il faut boucler
	    {
	      target -= la_somme;
	      icourant=0;
	    }
	}
      tab_select[index]=icourant;
      target += pas;
	  
    }
	
  melange(tab_select, taille_locale);
  iselect=0;
}
   
//-------------------------------------------
int proportionnel::select()
//-------------------------------------------
{
  if (iselect >= taille_locale) // on a besoin de plus de nbre_a_selectionner
    iselect=0;
  return debut_local+tab_select[iselect++];
}

////////////////////////////////////////////////////
// class rang: proportionnel basee sur le rang
///////////////////////////////////////////////////
//-------------------------------------------
selecteur* rang::clone()
//-------------------------------------------
{
  rang *ptloc = new rang(this);
  return (selecteur*)ptloc;
}

//-------------------------------------------
void rang::remplit_fitness_tmp(popu* pop, int tloc, int debut)
//-------------------------------------------
{
  // remettre les chromos ds le bon ordre:
  pop->trie_les_chromos(debut, tloc);
  // remplir le tableau tmp_fitness
  verif(tloc);			// verif des allocations memoire
  for (int i=0; i<tloc; i++)
    fitness_tmp[i] = (double)tloc-i;
  la_somme = (double)tloc*(tloc+1)/2.0;
}



/*
//-------------------------------------------
void roulette::prepare_select(popu* pop, int nb, int debut)
//---------------------------------------
{
   debut_local = debut;
  // premiere etape: remise dans l'ordre (maximiser ou minimiser)
  // et positif. 
  // melange virtuel des chromosomes
  remplit_random(tab_melange, taille_locale);
  // remplissage de fitness_tmp
  double rmin = pop->min_perf(), 
    rmax = pop->max_perf();
  la_somme=0.0;			// il faut la calculer sur les nb VIABLES
  if (pop->maximise())
    {
      for (int i=0; i<nb; i++)
	la_somme += ( fitness_tmp[i] = pop[tab_melange[i]]->perf()-rmin );
      rmax -= rmin;
      rmin = 0.0;
    }
  else
    {
      for (int i=0; i<nb; i++)
	la_somme += ( fitness_tmp[i] = rmax - pop[i]->perf() );
      rmax = rmax-rmin;
      rmin = 0.0;
    }
  // ici, le min de tmp est 0, et il contient des fitness
  // a maximiser

  switch(typ_scaling)
    {
    case LINEAIRE: 
      scale_lineaire();
      break;
    case EXPONENTIEL:
      scale_exp();
      break;
    case DYNAMIC:
      scale_dyn();
      break;
    }
}

//-------------------------------------------
int roulette::select()	// renvoie le No du chrom. selecteurne
//-------------------------------------------
   // le tableau les_perf est suppose OK
{
   double rloc=0.0;
   double rand = myrandom() * la_somme;       // reel entre 0 et somme
   for (int indloc=0; indloc<taille_pop && rloc < rand ; indloc++)
      rloc += fitness_tmp[indloc];
   if (indloc==0)		// cas somme=0, p. ex.
      return rnd(0, taille_pop);
   else
      return indloc-1;                // attention, renvoyer celui qu'on a DEJA depasse
}

*/

//////////////////////////////////////////
//        class tournoi 
//////////////////////////////////////////
   
//-------------------------------------------
tournoi::tournoi(int t):selecteur(t)
//-------------------------------------------
{
   taille_tournoi = (int)* new IntParametre("TAILLE_TOURNOI", "Taille du tournoi", 2);
}

//-------------------------------------------
selecteur* tournoi::clone()   
//-------------------------------------------
{
    tournoi* ptloc = new tournoi(taille_pop, taille_tournoi);
    ptloc->taille_locale = taille_locale;
    ptloc->pop_locale = pop_locale;
    return (selecteur*)ptloc;
}

//-------------------------------------------
void tournoi::prepare_select(popu* pop, int tloc, int debut)
//-------------------------------------------
{
  pop_locale = pop;
   taille_locale = tloc;
   debut_local = debut;
}

//-------------------------------------------
int tournoi::select()	// renvoie le No du chrom. selecteurne
//-------------------------------------------
{
   int challenger, 
      le_gagnant = debut_local+rnd(taille_locale);
   int res, nb=1;

   while(nb++ < taille_tournoi)		// taille du tournoi
   {
      challenger = debut_local+rnd(taille_locale);
      if ( (res = pop_locale->comp_chromos(challenger, le_gagnant) ) > 0 )
	 le_gagnant = challenger;
      else
	 if (res == 0)
	    if (flip(0.5))	// au hasard l'un des 2
		   le_gagnant = challenger;
   }
   return le_gagnant;
}

