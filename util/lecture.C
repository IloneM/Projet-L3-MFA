// ----------------------------------------------------------------------
// Where........: CMAP - Polytechnique 
// File.........: lecture.C
// Author.......: Bertrand Lamy (Equipe genetique)
// Created......: Tue Mar 21 13:30:44 1995
// Description..: Module de lecture haut niveau.
//		  Ne contient que les fcts de lecture C++
//		  (les autres sont dans mystdio)
// 
// Ident........: $Id: lecture.C,v 1.5 1996/04/30 22:21:46 lamy Exp $
// ----------------------------------------------------------------------

#include <string.h>
#include <ctype.h>
#include "mystdio.h"
#include "lecture.h"

///////////////////////////////////////////////////
char *Trouve_Mot(FICH_IN *fin, char* MotCle)
///////////////////////////////////////////////////
{
   char *pt, sloc[MaxLong];
   if (!fin)
      return 0;
   Rouvre_FICH_IN(fin);	// on repart au debut
   while ( ( pt=Lit_Mot(fin, sloc) ) )	// tant que pas EOF
      if ( !strcasecmp(pt, MotCle) )	// trouve
	 return pt;
return 0;
}

///////////////////////////////////////////////////
char *Trouve_Mot_again(FICH_IN *fin, char* MotCle)
///////////////////////////////////////////////////
{
   char *pt, sloc[MaxLong];
   if (!fin)
      return 0;
   while ( (pt=Lit_Mot(fin, sloc)) )	// tant que pas EOF
      if ( !strcasecmp(pt, MotCle) )	// trouve
	 return pt;
return 0;
}

///////////////////////////////////////////////////
long Lecture_Entier(FICH_IN *fin, int argc, char *argv[], char* MotCle, long Defaut)
///////////////////////////////////////////////////
{
   char *pt;
   long iloc;
   
   // Priorite : ligne de commande
   if ( (pt=Ad_option_mot(MotCle,argc, argv)) )	// trouve sur la ligne de commande
   {
      iloc = atol(pt);
      return iloc;
   }
   // le fichier de parametres
   if (Trouve_Mot(fin, MotCle))	// OK, trouve
   {
      iloc = Lit_Entier(fin);
      return iloc;
   }

   // l'environnement
   if ( (pt = getenv(MotCle)) )
   {
      iloc = atol(pt);
      return iloc;
   }
      
   // si pas trouve : le defaut
   return Defaut;
}

///////////////////////////////////////////////////
double Lecture_Reel(FICH_IN *fin, int argc, char *argv[], char* MotCle, double Defaut)
///////////////////////////////////////////////////
{
   char *pt;
   double rloc;
   
   // Priorite : ligne de commande
   if ( (pt=Ad_option_mot(MotCle,argc, argv)) )	// trouve sur la ligne de commande
   {
      rloc = atof(pt);
      return rloc;
   }
   // le fichier de parametres
   if (Trouve_Mot(fin, MotCle))	// OK, trouve
   {
      rloc = Lit_Reel(fin);
      return rloc;
   }

   // l'environnement
   if ( (pt = getenv(MotCle)) )
   {
      rloc = atof(pt);
      return rloc;
   }
      
   // si pas trouve : le defaut
   return Defaut;
}

char sloc[MaxLong];
///////////////////////////////////////////////////
char* Lecture_Ligne(FICH_IN *fin, int argc, char *argv[], char* MotCle, char* Defaut)
///////////////////////////////////////////////////
{
   char *pt;
   
   // Priorite : ligne de commande
   if ( (pt=Ad_option_mot(MotCle,argc, argv)) )	// trouve sur la ligne de commande
      return pt;

   // le fichier de parametres
   if (Trouve_Mot(fin, MotCle))	// OK, trouve
   {
      pt = Lit_Ligne(fin, sloc);
      return pt;
   }

   // l'environnement
   if ( (pt = getenv(MotCle)) )
      return pt;
      
   // si pas trouve : le defaut
   return Defaut;
}


///////////////////////////////////////////////////
char* Lecture_Mot(FICH_IN *fin, int argc, char *argv[], char* MotCle, char* Defaut)
///////////////////////////////////////////////////
{
   char *pt;
   
   // Priorite : ligne de commande
   if ( (pt=Ad_option_mot(MotCle,argc, argv)) )	// trouve sur la ligne de commande
      return pt;

   // le fichier de parametres
   if (Trouve_Mot(fin, MotCle))	// OK, trouve
   {
      pt = Lit_Mot(fin, sloc);
      return pt;
   }

   // l'environnement
   if ( (pt = getenv(MotCle)) )
      return pt;
      
   // si pas trouve : le defaut
   return Defaut;
}

///////////////////////////////////////////////////
char* Lecture_Mot_Alloue(FICH_IN *fin, int argc, char *argv[], char* MotCle, char* Defaut)
///////////////////////////////////////////////////
{
   char *ptt, *pta;
   
   // Priorite : ligne de commande
   ptt=Ad_option_mot(MotCle,argc, argv);	// trouve sur la ligne de commande
   
   if (!ptt)	// pas trouve : on essaie le fichier de parametres
      if (Trouve_Mot(fin, MotCle))	// OK, trouve
	 ptt = Lit_Mot(fin, sloc);
      else
      {
	 ptt = getenv(MotCle);	// l'environnement
	 if (!ptt)
	    ptt = Defaut;   // si pas trouve : le defaut
      }
   // allocation et recopie
   pta = new char[strlen(ptt+1)];
   strcpy(pta, ptt);
   return pta;
}

///////////////////////////////////////////////////
///////////////////////////////////////////////////
///////////////////////////////////////////////////
///////////////////////////////////////////////////
// les decorticages de strings
//

static char ConvString[256];
static char String_lecture[MaxLong], *pt_string_lecture;

void init_lecture_string(char* str, char *del)
{
   int i;
   char c, *ptloc;
   for (i=0; i<31; i++)
      ConvString[i]=0;
   for (i=32; i<128; i++)
      ConvString[i] = i ;
   /* liste des delimiteurs : le \n y est deja (Ascii 10) */
   if (del) 	// passes par l'appelant
   {
      ptloc = del;
      while (c=*ptloc++)
	 ConvString[c] = 0;
   }
   else
   {
      ConvString[' '] = 0;
      ConvString[','] = 0;
      ConvString['"'] = 0;
      ConvString['\\'] = 0;
   }
   strncpy(String_lecture, str, MaxLong);
   pt_string_lecture = String_lecture;
}

/* ------------------lecture d'un entier avec indicateur de EOF */
long Lit_Entier(char *str)
/* ------------------ */
{
char c, *PtDeb;

if (str)
   init_lecture_string(str);
if (!(c=*pt_string_lecture))
   return 0;	// fin de chaine
while ( !(ConvString[c]) )    /* sauter les premiers delimiteurs */
    {
       pt_string_lecture++;
    if (! (c = *pt_string_lecture) )     
	 break;		/* EOString  */
    }

PtDeb = pt_string_lecture ;             /* et stocke le debut du mot-nombre */
if ((c == '+')||(c == '-'))
   pt_string_lecture++;
while ( isdigit(c=*pt_string_lecture) )         /*  tant que chiffre */
   pt_string_lecture++;

*pt_string_lecture = '\0';          /* pour le strcpy */
long ilong = atol(PtDeb);		/* valeur lue */
*pt_string_lecture = c;             /* et on restaure l'ancien */
return ilong;
}


/* ------------------lecture d'un mot ds une string */
char *Lit_Mot(char* str, char* Mot)     /* remplit Mot, QUI DOIT ETRE ALLOUE */
/* ------------------ */
{
   char c, *PtDeb;
   
   if (str)
      init_lecture_string(str);
   if (! (c = *pt_string_lecture) )     
      return 0;
   
   while ( !ConvString[c] )    /* sauter les premiers delimiteurs */
   {
      pt_string_lecture++;
      if (! (c = *pt_string_lecture) )     
	 return 0;
   }
   PtDeb = pt_string_lecture;             /* et stocke le debut du mot */
   
   while (ConvString[c])         /*  attendre le delim suivant */
   {
      pt_string_lecture++;
      if (! (c = *pt_string_lecture) )     
	 break;	// sortie de boucle car EOString
      
   }
   *pt_string_lecture = '\0';          /* pour le strcpy */
   strcpy ( Mot, PtDeb );
   *pt_string_lecture = c;             /* et on restaure l'ancien */
   return Mot ;	// rend Mot, comme tous ces appels en gal
}

/* ------------------lecture d'un mot ds une string */
char Lit_Char(char* str)     /* rend le premier char non delim */
/* ------------------ */
{
   char c;
   
   if (str)
      init_lecture_string(str);
   if (! (c = *pt_string_lecture) )     
      return 0;
   
   while ( !ConvString[c] )    /* sauter les premiers delimiteurs */
   {
      pt_string_lecture++;
      if (! (c = *pt_string_lecture) )     
	 return 0;
   }
   c = *pt_string_lecture++;
   return c;
}

/* ------------------lecture d'un mot ds une string */
void Remet_Char(char c)     /* A appeler seulement apres un Lit_Char */
/* ------------------ */
{
    if (pt_string_lecture > String_lecture)
    {
       pt_string_lecture--;	// juste apres un Lit_Char
       if (c)			// sinon, retrouve le meme
	  *pt_string_lecture=c;
    }
    else
       printf("Warninge ds Remet_Char ...\n");
}

   
/* ------------------lecture et allocation d'un mot ds une string */
char *Lit_Mot_Alloue(char* str)
/* ------------------ */
{
   char c, *PtDeb;
   
   if (str)
      init_lecture_string(str);
   if (! (c = *pt_string_lecture) )     
      return 0;
   
   while ( !ConvString[c] )    /* sauter les premiers delimiteurs */
   {
      pt_string_lecture++;
      if (! (c = *pt_string_lecture) )     
	 return 0;
   }
   PtDeb = pt_string_lecture;             /* et stocke le debut du mot */
   
   while (ConvString[c])         /*  attendre le delim suivant */
   {
      pt_string_lecture++;
      if (! (c = *pt_string_lecture) )     
	 break;	// sortie de boucle car EOString
      
   }
   *pt_string_lecture = '\0';          /* pour le strcpy */
   char* ptloc = new char[pt_string_lecture-PtDeb+1];
   strcpy ( ptloc, PtDeb );
   *pt_string_lecture = c;             /* et on restaure l'ancien */
   return ptloc ;	// rend le mot alloue
}



/* ------------------lecture d'un reel ds une string */
double Lit_Reel (char *str)
/* ------------------ */
{
   char c, *PtDeb;
   
   if (str)
      init_lecture_string(str);
   if (! (c = *pt_string_lecture) )     
	 return 0;
   
   while ( !ConvString[c] )    /* sauter les premiers delimiteurs */
   {
      pt_string_lecture++;
      if (! (c = *pt_string_lecture) )     
	 break;
   }
   
   PtDeb = pt_string_lecture ;             /* et stocke le debut du mot-nombre */
   
   if ((c == '+')||(c == '-'))
      pt_string_lecture++;
   if (c!='.')		/* lire la partie entiere */
      while ( isdigit(c=*pt_string_lecture) )         /*  tant que chiffre */
	 pt_string_lecture++;
   
   if (c=='.')		/* lire la partie decimale */
      pt_string_lecture++;
   while ( isdigit(c=*pt_string_lecture) )         /*  tant que chiffre */
      pt_string_lecture++;
   
   if ( (c=='e' || c=='E') )	/* lire l'exposant */
   {
      pt_string_lecture++;
      c = *pt_string_lecture ;
      if ( (c == '+')||(c == '-') )
	 pt_string_lecture++;
      while ( isdigit(c=*pt_string_lecture) )         /*  tant que chiffre */
	 pt_string_lecture++;
   }     
   *pt_string_lecture = '\0';          /* pour le strcpy */
   double res = atof(PtDeb);
   *pt_string_lecture = c;             /* et on restaure l'ancien */
      
   return res;	/* pas EOF */
}              
