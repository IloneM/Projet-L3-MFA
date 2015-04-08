/* ----------------------------------------------------------------------
 * Where........: CMAP - Polytechnique 
 * File.........: myrand.h
 * Author.......: Marc Schoenauer
 * Created......: Tue Mar 21 13:25:04 1995
 * Description..: Module de generation aleatoire
 * 
 * Ident........: $Id: myrand.h,v 1.7 1997/04/28 13:14:47 marc Exp $
 * ----------------------------------------------------------------------
 */

#ifndef MYRAND_H
#define MYRAND_H

#ifdef __cplusplus
#include <stdio.h>
//#include <sstream>
#include <iostream>

extern "C" {
#endif

#ifndef MIN_MAX
#define MIN_MAX
#define max(a,b) ((a)>=(b)?(a):(b))
#define min(a,b) ((a)<=(b)?(a):(b))
#endif

#define	M_PI		3.14159265358979323846	/* pi */
#define	M_2PI		6.28318530717958647692  /* 2 * PI */


/* ----------------------------------------------------------------------
 *		les fonctions de util.C
 * ----------------------------------------------------------------------
*/

double myrandom();			/* renvoie un double equireparti entre 0 inclus et 1 exclus */
int bitrandom();             /* renvoie 0 ou 1 equireparti */
int flip(double );          /* renvoie 0 ou 1, probabilite p (entre 0 et 1) de rendre 1 */
int rnd(int, int);     /* renvoie un entier equidistribue entre n1 INCLUS et n2 EXCLUS */
double randgauss();     /* renvoie une gaussienne normale centree en 0 */
double add_gauss(double, double, double, double);
int selection(int, double *, double) ;  /* renvoie le resultat de la roulette ponderee */
int va_selection( int, ... );		/* Idem mais arguments variables */
					/* (les args DOIVENT etre castes en double */
long init_random(long seed) ;		/* init random, a partir de l'horloge si seed=0 */
void remplit_random(int * tab, int taille);
int* permute_aleatoire(int*,int);  /* renvoie une permutation de 1..taille dans tableau */
int poisson(double lambda);  // distribution de Poisson de parametre lambda

#ifdef __cplusplus
}
int rnd(int);     /* renvoie un entier equidistribue entre 0 INCLUS et n EXCLUS */
//istream& ouvrefichin(filebuf& ffi, char* s, char* ext);
void melange(void ** tab, int nb);
void melange(int* tab, int nb);
int poisson(int N, double pmut);  // renvoie le nombre de bits a muter entre 0 et N (Poisson)
#endif		/* de __cplusplus */

#endif		/* de MYRAND_H */
