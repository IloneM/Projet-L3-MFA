/* ----------------------------------------------------------------------
 * Where........: CMAP - Polytechnique 
 * File.........: mystdio.c
 * Author.......: Marc Schoenauer
 * Created......: Tue Mar 21 13:42:02 1995
 * Description..: Lectures de fichiers (en C)
 * 
 * Ident........: $Id: mystdio.c,v 1.5 1998/03/09 14:38:20 marc Exp $
 * ----------------------------------------------------------------------
 */

#include <ctype.h>
#include <string.h>
#include "mystdio.h"
#include "erreur.h"
#include "erreur_msg.h"	/* liste des textes d'erreur, APPLI-SPECIFIQUE*/




#define M_PLUS '+'
#define M_MOINS '-'
#define M_POINT '.'


/*--------------------------- Actualise (le buffer)---------------------------- */
/* Attention, miracle : si fin de fichier sans \n, fgets renvoie rien, non teste
   mais ca passe quand meme car Buffer est inchange ... AAARRRRG (cf aussi GrosEof) */

void grave_erreur(int NoErreur, char *serrloc);

int Actualise(Fich)
   FICH_IN *Fich ;
{
   char c ;
   
   if (!fgets(Fich->Buffer, MaxLong, Fich->Fic))    /* Fin de fichier depassee */
   {
      if (Fich->Exit_On_EOF)
	 grave_erreur(Err_Fin_Fic, Fich->Nom);
      return FALSE;
   }
   Fich->Pt = Fich->Buffer ;
   
   while( (c=*Fich->Pt ) )
      if (c=='#') 
      {
	 *Fich->Pt = '\0' ;
	 break ;
      }
      else Fich->Pt++ ;
   Fich->Pt = Fich->Buffer ;
   return TRUE;
}

/*-------------------------- Ouvre_FICH_IN----------------------------------*/
void init_FICH_IN(FLoc)
   FICH_IN *FLoc;
{
   int i;
   FLoc->Exit_On_EOF = 1;	/* par defaut, sortir sur EOF */
   for (i=0; i<31; i++)
      M_Delim(FLoc,i);
   for (i=32; i<128; i++)
      M_UnDelim(FLoc,i);
/* liste des delimiteurs : le \n y est deja (Ascii 10) */
   M_Delim(FLoc,' ');
   M_Delim(FLoc,',');
   M_Delim(FLoc,'"');
   M_Delim(FLoc,'\\');
/* dans mvsea, les multi-value seront separes par ';' */
   /*    Actualise(FLoc);  on ne le fait pas tout de suite, car bloquant 
         pour Transforme_en_FICH_IN */
   FLoc->Pt = FLoc->Buffer;
   FLoc->Buffer[0]='\0';	
           /* et le prochain appel a une lecture lancera actualise */
} 

FICH_IN *Ouvre_FICH_IN ( SFich )
char *SFich ;

{
   FICH_IN *FLoc;
   
   FLoc = (FICH_IN *) malloc(sizeof (FICH_IN)) ;
   if ( ( FLoc->Fic = fopen(SFich, "r") ) == NULL) 
      grave_erreur(Err_Ouv_Fich, SFich);
   strcpy(FLoc->Nom, SFich);	/* memoire du nom pour les erreurs */
   init_FICH_IN(FLoc);
   return FLoc;
} 

/* transforme un fich standard en FICH_IN
   utile pour lectures ds stdin! */
FICH_IN *Transforme_en_FICH_IN ( Fich )
FILE *Fich ;

{
   FICH_IN *FLoc;
   
   FLoc = (FICH_IN *) malloc(sizeof (FICH_IN)) ;
   FLoc->Fic = Fich;
   strcpy(FLoc->Nom, "interne" );	/* memoire du nom pour les erreurs */
   init_FICH_IN(FLoc);
   return FLoc;
} 

FICH_IN *Ouvre_FICH_IN_Non_Fatale ( SFich )
char *SFich ;

{
   FICH_IN *FLoc;
   
   FLoc = (FICH_IN *) malloc(sizeof (FICH_IN)) ;
   if ( ( FLoc->Fic = fopen(SFich, "r") ) == NULL) 
      return 0;
   strcpy(FLoc->Nom, SFich);	/* memoire du nom pour les erreurs */
   init_FICH_IN(FLoc);
   return FLoc;
} 

void Fin_De_Fichier_Non_Fatale(Fich)
FICH_IN *Fich ;
{
Fich->Exit_On_EOF = 0;
}

void Ferme_FICH_IN (Fich)
FICH_IN *Fich ;
{
fclose(Fich->Fic);
free(Fich);
}

void Rouvre_FICH_IN (Fich)
FICH_IN *Fich ;
{
fclose(Fich->Fic);
Fich->Fic = fopen(Fich->Nom, "r") ;
Actualise(Fich); 
}

/*-------------------------- Lectures ------------------------------*/

char Lit_Char(Fich)
   FICH_IN *Fich ;
{
   char c;
   while ( ! (c = *(Fich->Pt)) ) 
      if (!Actualise(Fich))
	 return EOF;
   
   while ( !(Fich->Conv[(int)c]) )    /* sauter les premiers delimiteurs */
   {
      (Fich->Pt)++;
      while (! (c = *(Fich->Pt)) )     
	 if (!Actualise(Fich))
	    return EOF;
   }
   (Fich->Pt)++ ;                 /* Pt pointe toujours sur le suivant a lire */
   Fich->Dernier = c ;		/* pour Remt_Char */
   return c;
}

void Remet_Char(FICH_IN *Fich)	/* A NE FAIRE QU'APRES UN Lit_Char */
{
   char c,d, *ptloc;
   
   if (Fich->Pt != Fich->Buffer)
      (Fich->Pt)--;
   else		/* on a fait un Actualise entre temps */
   {
      d = Fich->Dernier;
      ptloc = Fich->Pt ;
      while (*(ptloc+1))
      {
	 c=*ptloc;
	 *ptloc = d ;
	 d = c ;
	 ptloc++ ;
      }
      c=*ptloc;
      *ptloc++ = d ;
      *ptloc++ = c;
      *ptloc='\0';
   }   
}

/*------------------- Lit_Mot --- */

char *Lit_Mot(Fich, Mot)     /* remplit Mot, QUI DOIT ETRE ALLOUE */
   FICH_IN *Fich ;              /* renvoie Mot (pour faire comme tt le monde */
   char *Mot ;
{
   char *PtLoc;
   char c ;
   
   while (! (c = *(Fich->Pt)) )     
      if (!Actualise(Fich))
	 return 0;
   
   while ( !(Fich->Conv[(int)c]) )    /* sauter les premiers delimiteurs */
   {
      (Fich->Pt)++;
      while (! (c = *(Fich->Pt)) )     
	 if (!Actualise(Fich))
	    return 0;
      
   }
   PtLoc = Fich->Pt;             /* et stocke le debut du mot */
   
   while (Fich->Conv[(int)c])         /*  attendre le delim suivant */
   {
      (Fich->Pt)++;
      while (! (c = *(Fich->Pt)) )     
	 if (!Actualise(Fich))
	    return 0;
      
   }
   *(Fich->Pt) = '\0';          /* pour le strcpy */
   strcpy ( Mot, PtLoc );
   *(Fich->Pt) = c;             /* et on restaure l'ancien */
   return Mot ;
}

/*---------------- Lit_Mot_Alloue ----------------- */
/* alloue la place necessaire au mot lu */

char* Lit_Mot_Alloue(FICH_IN *fin)
{
   char motbidon[MaxLong];
   char* ptloc;
   Lit_Mot(fin, motbidon);
   ptloc = (char*)malloc(strlen(motbidon)+1);
   strcpy(ptloc, motbidon);
   return ptloc;
}

/*---------------- Lit_Ligne ----------------- */

char *Lit_Ligne(Fich, Ligne)     /* remplit Ligne, QUI DOIT ETRE ALLOUE */
   FICH_IN *Fich ;              /* renvoie Ligne (pour faire comme tt le monde */
   char *Ligne ;
{         
   char *PtLoc;
   char c ;
   
   while (! (c = *(Fich->Pt)) )     
      if (!Actualise(Fich))
	 return 0;
   
   while ( !(Fich->Conv[(int)c]) )    /* sauter les premiers delimiteurs */
   {
      (Fich->Pt)++;
      while (! (c = *(Fich->Pt)) )     
	 if (!Actualise(Fich))
	    return 0;
      
   }
   PtLoc = Fich->Pt;             /* et stocke le debut de la ligne */
   /* on sait qu'on est pas sur un delim */
   while (c && (c != '\n'))         /*  attendre la fin de ligne : \0 ou \n */
      c= *(Fich->Pt)++;
   *( (Fich->Pt)-1 ) = '\0';          /* pour virer le \n*/
   strcpy ( Ligne, PtLoc );
   return Ligne ;
}

/*------------------ Divers lecture ------------ */

/* ------------------lecture d'un entier avec indicateur de EOF */
int Lit_Entier_Pas_EOF (Fich, ptint)
/* ------------------ */
FICH_IN *Fich ; 
long *ptint;	/* la valeur lue */
{
char *PtLoc;
char c ;

while (! (c = *(Fich->Pt)) )     
      if (!Actualise(Fich)) 
	 return 0;		/* EOF  */
while ( !(Fich->Conv[(int)c]) )    /* sauter les premiers delimiteurs */
    {
    (Fich->Pt)++;
    while (! (c = *(Fich->Pt)) )     
      if (!Actualise(Fich))
	 return 0;		/* EOF  */
    }

PtLoc = Fich->Pt;             /* et stocke le debut du mot-nombre */
if ((c == M_PLUS)||(c == M_MOINS))
    (Fich->Pt)++;
while ( isdigit(c=*(Fich->Pt)) )         /*  tant que chiffre */
    (Fich->Pt)++;

*(Fich->Pt) = '\0';          /* pour le strcpy */
*ptint = atol(PtLoc);		/* valeur lue */
*(Fich->Pt) = c;             /* et on restaure l'ancien */
return 1;
}

/* ------------------lecture bete d'un entier */
long Lit_Entier (Fich)
/* ------------------ */
FICH_IN *Fich ; 
{
long res;
if (Lit_Entier_Pas_EOF(Fich, &res))
   return res;
else
   return 0;
}

/* ------------------lecture d'un reel avec indication de EOF */
int Lit_Reel_Pas_EOF (Fich, ptres)
/* ------------------ */
   FICH_IN *Fich ; 
   double *ptres;
{
   char *PtLoc;
   char c ;
   
   while (! (c = *(Fich->Pt)) )     
      if (!Actualise(Fich))
	 return 0;
   
   while ( !(Fich->Conv[(int)c]) )    /* sauter les premiers delimiteurs */
   {
      (Fich->Pt)++;
      while (! (c = *(Fich->Pt)) )     
	 if (!Actualise(Fich))
	    return 0;
      
   }
   
   PtLoc = Fich->Pt;             /* et stocke le debut du mot-nombre */
   
   if ((c == M_PLUS)||(c == M_MOINS))
      (Fich->Pt)++;
   if (c!=M_POINT)		/* lire la partie entiere */
      while ( isdigit(c=*(Fich->Pt)) )         /*  tant que chiffre */
	 (Fich->Pt)++;
   
   if (c==M_POINT)		/* lire la partie decimale */
      (Fich->Pt)++;
   while ( isdigit(c=*(Fich->Pt)) )         /*  tant que chiffre */
      (Fich->Pt)++;
   
   if ( (c=='e' || c=='E') )	/* lire l'exposant */
   {
      (Fich->Pt)++;
      c = *(Fich->Pt) ;
      if ( (c == M_PLUS)||(c == M_MOINS) )
	 (Fich->Pt)++;
      while ( isdigit(c=*(Fich->Pt)) )         /*  tant que chiffre */
	 (Fich->Pt)++;
   }     
   *(Fich->Pt) = '\0';          /* pour le strcpy */
   *ptres = atof(PtLoc);
   *(Fich->Pt) = c;             /* et on restaure l'ancien */
   /*tentative de deceler une erreur : TROP de fausses alarmes
   if ( (Fich->Pt - PtLoc) <= 1)
   {
      printf("Warninge, peut etre un pb de lecture de reel dans %s\n",Fich->Nom);
      printf("Ligne courante=%s\n",PtLoc);
   }
   */
      
   return 1;	/* pas EOF */
}              

/* ------------------lecture bete d'un reel */
double Lit_Reel (Fich)
/* ------------------ */
FICH_IN *Fich ; 
{
double res;
if (Lit_Reel_Pas_EOF(Fich, &res))
   return res;
else
   return 0.0;
}

void Saute_N_Mots(FicLoc,Nb_Mots)
FICH_IN *FicLoc;
int Nb_Mots;
    {
    int i;
	char CVal[1024];
    for (i=0;i<Nb_Mots;i++)
	    Lit_Mot(FicLoc, CVal);      
    }

/* ####################################################
      gestion des arguments/options
##################################################### */

/* --- Tableau de verif des args, a 1 si l'argument a ete utilise --- */
static int	*CheckArgs = NULL;
#define ArgCheck(i)	(CheckArgs[i] = 1)

/* 
 * Initialise le tableau de verif. des args
 */
void InitArgCheck( int argc )
/* ----------------------- */
{
    if( CheckArgs )
	return;
    CheckArgs = calloc( argc, sizeof(*CheckArgs) );
}

/* 
 * Verifie que tous les arguments de la forme -xxx ont ete 
 * vu par la fonction Ad_option...
 */
int VerifieArguments( int argc, char **argv )
/* --------------------------------------- */
{
    int		i;
    int		ok = 1;
    if( ! CheckArgs )
	return 1;			/* dommage! */
    for( i=0; i < argc; i++ ) {
	if( (*argv[i] == '-') && (CheckArgs[i] == 0) ) {
	    /* --- Pas Vu! --- */
	    ok = 0;
	    fprintf( stderr, "Waouw: Argument %s non reconnu\n", argv[i] );
	}
    }
    return ok;
}



/* renvoie l'indice de l'argument suivant, 0 si apu */

static int arg_suivant = 0 ;

int argument_suivant(argc, argv)
int argc;
char *argv[];
{
arg_suivant++;                /* il stocke le dernier rendu */
while (arg_suivant < argc)
   {
   if (*argv[arg_suivant] != '-')
      return arg_suivant ;
   arg_suivant++;
   }
return 0;
}

/* renvoie un pointeur sur la chaine de -cChaine , NULL si pas d'otion */

char *Ad_option(char c, int argc, char* argv[])
{
int i;
for (i=0; i<argc; i++)
    if ( (*argv[i] == '-') && (*(argv[i]+1) == c) )
        return argv[i]+2 ;
return NULL;             
}

/* renvoie un pointeur sur Chaine de -MotCle=Chaine , NULL si pas d'otion */
char *Ad_option_mot(char* MotCle, int argc, char* argv[])
{
int i;
char c, *ptloc;
InitArgCheck( argc );
for (i=0; i<argc; i++)
    if ( (*argv[i] == '-')  )	/* c'et une option */
    {
       ptloc = argv[i]+1;
       while ( (c=*ptloc) ) 
	  if (c=='=') 
	     *ptloc = '\0';
	  else ptloc++;
       /* ici, ptloc pointe sur 0 */
       if ( !strcasecmp(argv[i]+1, MotCle) )	/* egalite des chaines */
       {
	   ArgCheck( i );
	   return ++ptloc ;
       }
    }
return NULL;             
}

/*****************************************************************
   GESTION DES ERREURS
   ***************************************************************/

/* les Numeros d'erreur specifiques de l'appli sont dans erreur.h*/
/* les 10 premiers numeros sont reserves aux erreurs communes - cf + loin */

void grave_erreur(int NoErreur, char *serrloc)
{
   printf("Fatale Erreur %d : %s%s\n",NoErreur, ERREUR[NoErreur], serrloc);
   exit(NoErreur);
}
   
void legere_erreur(int NoErreur, char *serrloc)
{
   printf("Legere erreur %d : %s%s\n",NoErreur, ERREUR[NoErreur], serrloc);
}

void On_Sort(int NoErreur, char *serrloc, int NoComplet)
{
    printf("%s : %s %s %s \n", ERREUR[Err_Fatale],ERREUR[NoErreur],serrloc,ERREUR[NoComplet]);
    exit(NoErreur);
}

