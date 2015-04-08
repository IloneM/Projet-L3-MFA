/* ----------------------------------------------------------------------
 * Where........: CMAP - Polytechnique 
 * File.........: lecture.h
 * Author.......: Marc Schoenauer
 * Created......: Tue Mar 21 13:29:49 1995
 * Description..: Lecture d'objets (chaine, entiers, reels) dans les fichiers
 * 
 * Ident........: $Id: lecture.h,v 1.2 1995/05/27 05:42:55 marc Exp $
 * ----------------------------------------------------------------------
 */

#ifndef LECTURE_H
#define LECTURE_H

// ATTENTION, les lectures de string qui allouent LE DISENT
// donc, ds le doute, elles n'allouent pas la place du mot

#include "mystdio.h"

char *Trouve_Mot(FICH_IN *fin, char* MotCle);
char *Trouve_Mot_again(FICH_IN *fin, char* MotCle);
long Lecture_Entier(FICH_IN *fin, int argc, char *argv[], char* MotCle, long Defaut);
double Lecture_Reel(FICH_IN *fin, int argc, char *argv[], char* MotCle, double Defaut);
char* Lecture_Ligne(FICH_IN *fin, int argc, char *argv[], char* MotCle, char* Defaut);
char* Lecture_Mot(FICH_IN *fin, int argc, char *argv[], char* MotCle, char* Defaut);
char* Lecture_Mot_Alloue(FICH_IN *fin, int argc, char *argv[], char* MotCle, char* Defaut);

void init_lecture_string(char* str, char *del = 0); // del = liste des delim. 
long Lit_Entier(char *str=0);		// str=0 : la meme qu'a l'appel precedent
char *Lit_Mot(char* str, char* Mot);	// str=0 : la meme qu'a l'appel precedent
char* Lit_Mot_Alloue(char *str=0);	// ... 
double Lit_Reel (char *str=0);		// str=0 : la meme qu'a l'appel precedent
char Lit_Char(char* str=0);     /* rend le premier char non delim */
void Remet_Char(char c=0);     /* A appeler seulement apres un Lit_Char */
#endif
