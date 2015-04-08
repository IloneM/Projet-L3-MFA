/* ----------------------------------------------------------------------
 * Where........: CMAP - Polytechnique 
 * File.........: mystdio.h
 * Author.......: Marc Schoenauer
 * Created......: Tue Mar 21 13:24:17 1995
 * Description..: I/O haut niveau
 * 
 * Ident........: $Id: mystdio.h,v 1.2 1998/03/09 14:38:21 marc Exp $
 * ----------------------------------------------------------------------
 */

#ifndef _MYSTDIO_H
#define _MYSTDIO_H


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//using namespace std;

/* quelques mqcros d,interet general */
#ifndef MIN_MAX
#define MIN_MAX
#define max(a,b) ((a)>=(b)?(a):(b))
#define min(a,b) ((a)<=(b)?(a):(b))
#endif

#define VERY_BIG 1e308
#define TRUE 1
#define FALSE 0

//#define MaxLong 40096
#define MaxLong 4000096

struct _FICH_IN 
{
   FILE *Fic ;          /* le fichier C */
   char Buffer[MaxLong];      /* le buffer */
   char *Pt ;           /* le pointeur courant */
   char Conv[128] ;     /* le tableau des delimiteurs :
			   Conv[c] contient 0 si delimiteur, c sinon */
   int Exit_On_EOF ;	/* faut-il sortir sur lecture apres eof ? */
   char Nom[1024];
   char Dernier;		/* pour Remet_Char */
} ;

typedef struct _FICH_IN FICH_IN;

#define M_Delim(F,Car) (F)->Conv[Car]='\0'
#define M_UnDelim(F,Car) (F)->Conv[Car]=(Car)

/* ouverture : open, maj de Conv et lecture 1ere ligne */
#ifdef __cplusplus
extern "C" {
#endif

FICH_IN *Ouvre_FICH_IN (char * );
FICH_IN *Transforme_en_FICH_IN ( FILE * );
FICH_IN *Ouvre_FICH_IN_Non_Fatale (char * );
void Fin_De_Fichier_Non_Fatale(FICH_IN *) ;
void Ferme_FICH_IN ( FICH_IN *);
void Rouvre_FICH_IN ( FICH_IN *);
char Lit_Char( FICH_IN *);
void Remet_Char(FICH_IN *);		/* A NE FAIRE QU'APRES UN Lit_Char */
char *Lit_Mot( FICH_IN *, char *);
char* Lit_Mot_Alloue(FICH_IN *);
char *Lit_Ligne( FICH_IN *, char *);
int Lit_Entier_Pas_EOF (FICH_IN *, long *);	/* rend 0 si EOF, 1 sinon, resultat ds le pteur */
long Lit_Entier ( FICH_IN *);			/* EOF indiscernable de 0 */
int Lit_Reel_Pas_EOF (FICH_IN *, double *);	/* rend 0 si EOF, 1 sinon, resultat ds le pteur */
double Lit_Reel ( FICH_IN *);			/* EOF indiscernable de 0 */


char *Ad_option(char, int, char**);
/* renvoie un pointeur sur Chaine de -MotCle=Chaine , NULL si pas d'otion */
char *Ad_option_mot(char* MotCle, int argc, char* argv[]);
int argument_suivant(int, char**);
int VerifieArguments( int argc, char **argv );


#ifdef __cplusplus
}
#endif

#endif   /* du if ndef _MYSTDIO_H */
