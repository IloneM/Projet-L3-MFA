// ----------------------------------------------------------------------
// Where........: CMAP - Polytechnique 
// File.........: myrand.C
// Author.......: Marc Schoenauer
// Created......: Tue Mar 21 13:27:45 1995
// Description..: Implememtation des generateurs aleatoires
// 
// Ident........: $Id: myrand.C,v 1.7 1997/04/28 13:14:46 marc Exp $
// ----------------------------------------------------------------------

#include <math.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <sstream>
#include <iostream>
//#include <fstream>
#include <sys/time.h>
using namespace std;

#include "myrand.h"

#ifdef APOLLO
    long  random(); 
    const long BIG = (long)(1<<31)-1 ;        // borne max pour random(), 
    #define M_EQUI_01 (double)random()/(double)BIG
#endif

// HP700 : tout fait ... 
#ifdef HP700
#ifdef DEBUG_RAND48
    #define M_EQUI_01 mydrand48()
   long nbrand=0;
   double mydrand48()
   {
     nbrand++;
     cout << "DRAND: "<< nbrand << endl;
     return drand48();
   }
#else
    #define M_EQUI_01 drand48()
#endif
#endif

// quand ca marche !
#ifdef RAND
    const long BIG = 32767;
    #define M_EQUI_01 (double)rand()/(double)BIG
#endif    

#define TRUE 1
#define FALSE 0

extern "C"
//----------------------------------------------
double myrandom()      // renvoie un double equireparti entre 0 INCLUS et 1
//----------------------------------------------
{
return M_EQUI_01 ;
}


extern "C"
//----------------------------------------------
int bitrandom()             // renvoie 0 ou 1 equireparti
//----------------------------------------------
{
return (M_EQUI_01 < 0.5) ;
} 

extern "C"
//----------------------------------------------
int flip(double p)          // renvoie 0 ou 1, probabilite p (entre 0 et 1) de rendre 1
//----------------------------------------------
{
double r01;
    r01 = M_EQUI_01 ;
return (r01<p ? 1 : 0 );
}

// seulement de C++
//----------------------------------------------
int rnd(int n)     // renvoie un entier equidistribue entre 0 INCLUS et n EXCLUS
//----------------------------------------------
{
double r01;
    r01 = M_EQUI_01 ;
return (int) ( n*r01 ) ;
}
                                                                  
extern "C"
//----------------------------------------------
int rnd(int n1, int n2)     // renvoie un entier equidistribue entre n1 INCLUS et n2 EXCLUS
//----------------------------------------------
{
double r01;
    r01 = M_EQUI_01 ;
return n1 + (int) ( (n2-n1)*r01 ) ;
}
                                                                  
static double autre ;
static int boolautre = FALSE ;


extern "C"
//----------------------------------------------
double randgauss()      // renvoie var normale centree en 0
//----------------------------------------------
//     principe : chgt de variable polaire :
//		X,Y  var gaussiennes independantes SSI
//      Ro et Teta vars exponetielle et Equirepartie, resp.
//
//          pour alleger les calculs, on en calcule 2 a chaque fois
{            
if (boolautre)
    {
    boolautre = FALSE;
    return autre;
    }
boolautre = TRUE ;
double r1, r2;
r1 = M_EQUI_01 ;
r1 = ( (r1==0.0) ? 4.0E+10 : sqrt(-2*log(r1)) ) ;
// r1 = sqrt( -2*log(myrandom()) ) ;
r2 = M_2PI*M_EQUI_01 ;

autre = r1 * cos(r2);
return  r1 * sin(r2);
}


extern "C"
//----------------------------------------------
double add_gauss(double valini, double minloc, double maxloc, double sigma)
//----------------------------------------------
// renvoie valini + gaussienne(sigma), apres "rebonds" sur min et max

{
long iloc;
double dlargloc = 2 * (maxloc-minloc) ;
double newval = valini+randgauss()*sigma ;

if (fabs(newval) > 1.0E9)		// explosion de iloc
   newval = myrandom()*(maxloc-minloc) + minloc ;

if ( (newval > maxloc) )
    {
    iloc = (long) ( (newval-minloc) / dlargloc ) ;
    newval -= dlargloc * iloc ;
    if ( newval > maxloc )
        newval = 2*maxloc - newval ;
    }

if (newval < minloc) 
    {
    iloc = (long) ( (maxloc-newval) / dlargloc ) ;
    newval += dlargloc * iloc ;
    if (newval < minloc)
        newval = 2*minloc - newval ;
    }
if ( (newval<minloc) || (newval>maxloc) )
    {
    cout << "Grave erreur dans add_gauss\n";
    exit(1);
    }
return newval;
}

extern "C"
//----------------------------------------------
double rebond(double val, double min, double max)
//----------------------------------------------
   // rebond unique sur les bornes
{
   if (val<min)
      return 2*min-val;
   if (val>max)
      return 2*max-val;
   return val;
}
  


extern "C"
//----------------------------------------------
int selection(int indiv, double* proba, double somme)
//----------------------------------------------
{
    int	indloc;
double rloc=0., 
	myrand = myrandom() * somme;       // reel entre 0 et somme
for ( indloc=0; indloc<indiv && rloc < myrand ; indloc++)
    rloc += proba[indloc];
if (indloc==0)
    return rnd(0, indiv);
else
    return indloc-1;                // attention, renvoyer celui qu'on a DEJA depasse
}


// 
// Selection a argument variables
// utile pour faire une rapide selection entre qq elements :
// lequel = va_selection( 2, (double)p1, (double)p2 );
//	(les cast (double) sont FORTEMENT CONSEILLES)
//	
// 
extern "C"
int va_selection(int nb, ... )
// ---------------------------
{
   va_list  	args;
   double	*poids;
   double	somme = 0.0;
   double 	tirage;
   int		i = 0;

   va_start( args, nb );
   poids = (double *) malloc( sizeof(double) * nb );
   if( poids ) {
      for( i=0; i < nb; i++ ) {
	 poids[i] = va_arg( args, double );
	 somme += poids[i];
      }
      tirage = myrandom() * somme;
      somme = poids[0];
      for( i=0; i < nb; i++, somme += poids[i] )
	 if( tirage < somme )
	    break;

      free( poids );
   }
   va_end( args );
   
   return( i );
}

extern "C"
//----------------------------------------------
long init_random(long seed)	// init random, a partir de l'horloge si seed=0
//----------------------------------------------
{
   if (seed != 0)
   {
      srand48(seed );
      return seed;
   }		// sinon, se caler sur l'horloge
   
   struct timeval tval;
   struct timezone tzp;

   gettimeofday (&tval, &tzp);	// temps ecoule depuis midnight January 1, 1970.
   srand48(tval.tv_usec );	// les micro secondes	
   return tval.tv_usec;
}

//----------------------------------------------
void melange(void** tab, int nb)// ordre TOTALEMENT aleatoire
//----------------------------------------------
// on fait nb transpositions random ...
{
void* tmpt;
int perm;
for (int i=0; i<nb; i++)
  {
    perm = rnd(nb);
    tmpt = tab[perm];
    tab[perm]=tab[i];
    tab[i]=tmpt;
  }
}

//----------------------------------------------
void melange(int* tab, int nb)// ordre TOTALEMENT aleatoire
//----------------------------------------------
// on fait nb transpositions random ...
{
int itmp;
int perm;
for (int i=0; i<nb; i++)
  {
    perm = rnd(nb);
    itmp = tab[perm];
    tab[perm]=tab[i];
    tab[i]=itmp;
  }
}


extern "C"
//----------------------------------------------
void remplit_random(int* tab, int taille)
//----------------------------------------------
// rmeplit un tableau d'entiers distincts ds un ordre aleatoire
{
  int i;
  for (i=0; i<taille; i++)
    tab[i]=i;
// on fait nb transpositions random ...
int perm, tmp;
for (i=0; i<taille; i++)
  {
    perm = rnd(taille);
    tmp = tab[perm];
    tab[perm]=tab[i];
    tab[i]=tmp;
  }
}

//---------------------------------------------- 
int* permute_aleatoire(int* tableau,int taille)  // renvoie une permutation de
//----------------------------------------------    1..taille
{
  int i;
   if (!tableau)
     tableau = new int[taille];
  
   for (i=0; i<taille; ++i) 
     tableau[i]=-1;

   // creation aleatoire de l'index 
   for (i=taille; i>0; --i) {
     int j=rnd(i);
     int l=0;
     int k=-1;
     do {
       if (tableau[++k]==-1) ++l;
     } while (l<=j);
     tableau[k]=i-1;
   }
   return tableau;
}

//////////////////// Distribution de poisson:
//
//   P_l[X=k] = exp(-l) l^k/k!
//
// c'est la limite de la loi binnomiale (somme de N lois 0/1 de proba 0)
// lorsque N tend vers +infini avec Np tend vers l
//
// ce qui est le cas (a peu pres) pur les bitstring, N etant le nombre de bits
// et p laproba de muter 1 bit: le nombre entier de bits mutes suit alors la loi 
// de Poisson de parametre N*p
//
// La facon la plus simple de la calculer est d'attendre que le produit de k uniformes 
// depasse exp(-l):
//
// ATTENTION: peut parfaitement renvoyer 0 !!!

int poisson(double lambda)
{
  double borne=exp(-lambda);
  double var = 1.0;
  int res=0;
  while (var > borne) {
    var *= myrandom();
    res++;
  }
  return res-1;
}

// par compatibilite avec l'ancienne version:
int poisson(int N, double p)
{
  return poisson(N*p);
}

