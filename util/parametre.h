// --*- C++ -*-----------------------------------------------------------
// Where........: CMAP - Polytechnique 
// File.........: parametre.h
// Author.......: Bertrand Lamy (Equipe genetique)
// Created......: Fri Feb  3 16:04:06 1995
// Description..: Lecture automatique de parametres sur la ligne
//		  de commande ou dans un fichier
//
// Ident........: $Id: parametre.h,v 1.9 1996/05/30 16:55:08 marc Exp $
// ----------------------------------------------------------------------


#ifndef PARAMETRE_H
#define PARAMETRE_H


//////////////////////////////////////////////////////////////////////
//
// Definit differents types de parametres,
// Les parametres sont definis par 
//	un mot cle
//	un commentaire
//	une valeur
//	et une valeur par defaut
//
// En fonction du type de valeur voulue, on uilisera:
//	DoubleParametre		double
//	IntParametre		int
//	UintParametre		uint avec possibilite de def. en %-tage
//	LongParametre		long
//	WordParametre		char *
//
// mais aussi:
//	TitleParametre	Pour ajouter un titre dans la listes des param
//	ArrayParametre	Pour definir une structre complexe de parametres
//
// La methode de classe:
// 	Parametre::SetSources
// permet de definir ou chercher les parametres pour les Read qui suivront.
//
// Chacun definit les methodes:
//	Read		lit le parametre
//	Help		Affiche le mot cle et le commentaire
//	Status		Affiche le mot cle, l'etat et le commentaire
//
// On definit aussi une classe "ParamList", qui est une liste de 
// parametres, sur lequel on peut appliquer les methodes precedentes
// (elle sont alors appliquees en sequences a tous les elements).
// la methode 	
//	Add	permet d'ajouter un element a l'ensemble
//
// Les parametres sont ajoutes a une liste de parametres 
// automatiquement, on utilise alors les methodes de classe:
//	Param::AllRead();
//	Param::AllHelp();
//	Param::AllStatus();
// pour declencher les ordres correspondants sur tous les parametres.
//
//
// *) Si un parametre est cree apres le SetSources, alors il est 
// automatiquement lu.
// *) Pour ordonner les parametres, on peut creer une ParamList avec un 
// prefix.
// *) Les ParamList permettent de structurer les parametres: une ParamList
// peut contenir elle meme des ParamList.
// *) Les parametres sont ajoutes 
//		- dans la DERNIERE ParamList cree, 
//		- ou dans la derniere liste definie par 
//			Parametre::setCurrentList
//
//////////////////////////////////////////////////////////////////////
// ATTENTION: Ne jamais faire
//     IntParametre toto("TOTO", parametre bidon",0);
// si toto est une variable LOCALE a une subroutine.
// Car un pointeur sur toto est alors stocke ds une Paramist
// et au premier status ou help, BOOM
//
// il faut faire
// int toto = (int) * new IntParametre("TOTO", parametre bidon",0);
//    et en plus on n'a plus comme ca de problem de cast d'IntParametre en int

#include "mystdio.h"
#include "mystring.h"
//#include <stdio.h>
#include <iostream>
using namespace std;

class ParamList;			// definit plus loin
class WordParametre;

// --- Les parametres types sont tous derives de cette classe ---
class Parametre {
protected:
    // --- Variables de classe ---
    static ParamList	mainList;	// La liste principale (et par default)
    static ParamList*	curList;	// La liste courante (=mainList au debut)

    static FICH_IN	*Fin;		// Le fichier d'entree
    static int		Argc;		// Ligne de commande: nb de mots
    static char		**Argv;		// la ligne de commande
    static int		sourcesAreOk;	// Flag pour coherence Fin et Argv

    // --- Attribut des parametres ---
    String	Comment;		// Le commentaire explicatif
    String	KeyWord;		// Le mot-cle du param
    int		read;			// flag pour savoir s'il est lu


    // --- Quelques methodes protected ---
    // --- Reserves a l'usage des sous-classes ---
    Parametre( const char *key, const char *comment,
	       ParamList *pList=0 );
    ~Parametre();

    static void CheckSources();

public:
    // --- Methodes de classe ---
    static void SetSources( WordParametre & FichParam, int argc, char **argv );

    static FICH_IN *GetFile();		// Renvoie le fichier de param
    static void Ferme();		// Ferme le fichier de param

    // --- Gestion des listes de parametres ---
    static void setCurrentList( ParamList *l );
    static ParamList* getCurrentList( );
    static ParamList* getMainList( );

    virtual void Read() = 0;
    virtual void Help( );
    virtual void Status( ostream& out );
    int isRead() { return read; }

    virtual ostream& PrintValue( ostream & ) = 0;
    virtual ostream& PrintDefault( ostream & ) = 0;
    virtual char * SPrintValue( char * ) = 0;

    // --- Enchainement des actions sur tous les parametres ---
    static void AllRead();
    static void AllHelp();
    static void AllStatus( ostream& out );
};



// --- Parametre Double ---
// ------------------------
class DoubleParametre : public Parametre {
    double	value;
    double	defValue;
public:
    DoubleParametre( const char *key, const char *comment, 
		     const double dflt, ParamList *pList=0 );

    DoubleParametre &operator=( const double );

    void Read();

    operator	double() { return value; }

    virtual ostream& PrintValue( ostream & );
    virtual ostream& PrintDefault( ostream & );
    virtual char *SPrintValue( char * );
};


// --- Parametre int ---
// ---------------------
class IntParametre : public Parametre {
    int		value;
    int		defValue;
public:
    IntParametre( const char *key, const char *comment, 
		  const int dflt, ParamList *pList=0 );

    IntParametre &operator=( const int );

    void Read();

    operator 	int() { return value; }

    virtual ostream& PrintValue( ostream & );
    virtual ostream& PrintDefault( ostream & );
    virtual char *SPrintValue( char * );
};


// --- Parametre unsigned int ---
// ------------------------------
class UintParametre : public Parametre {
    unsigned int	value;
    unsigned int	defValue;
    String		defString;
    UintParametre 	*From;
    unsigned int        *from;	// 2 manieres d'avoir un pourcentage
public:
    // --- Constructeur classique ---
    UintParametre( const char *key, const char *comment, 
		   const unsigned int dflt, ParamList *pList=0 );
    // --- Constructeur avec valeur de reference UintParametre, ---
    // permet de specifier des valeurs en % 
    // (par ex. defValueStr="80 %")
    UintParametre( const char *key, const char *comment, 
		  const char *defValueStr, UintParametre *ref,
		   ParamList *pList=0 );
    // --- Constructeur avec valeur de reference unsigned int, ---
    // permet de specifier des valeurs en % 
    // (par ex. defValueStr="80 %")
    UintParametre( const char *key, const char *comment, 
		  const char *defValueStr, unsigned int *ref,
		   ParamList *pList=0 );

    UintParametre &operator=( const unsigned int );

    void Read();

    operator	unsigned int() { return value; }

    virtual ostream& PrintValue( ostream & );
    virtual ostream& PrintDefault( ostream & );
    virtual char *SPrintValue( char * );
};


// --- Parametre long ---
// ----------------------
class LongParametre : public Parametre {
    long	value;
    long 	defValue;
public:
    LongParametre( const char *key, const char *comment, 
		   const long dflt, ParamList *pList=0 );

    LongParametre &operator=( const long );

    void Read();
    
    operator	long() { return value; }
    
    virtual ostream& PrintValue( ostream & );
    virtual ostream& PrintDefault( ostream & );
    virtual char *SPrintValue( char * );
};



// --- Parametre Mot ---
// ----------------------
class WordParametre : public Parametre {
protected:
    String	value;
    String 	defValue;
public:
    WordParametre( const char *key, const char *comment, 
		   const char *dflt, ParamList *pList=0 );

    WordParametre &operator=( char* );

    void Read();
    
    operator	char*() { return value; }
    
    virtual ostream& PrintValue( ostream & );
    virtual ostream& PrintDefault( ostream & );
    virtual char *SPrintValue( char * );
  friend void Parametre::SetSources(WordParametre &, int, char**);
};




// --- Permet d'ajouter des commentaires dans les parametres ---
//
// -------------------------------------------------------------
class TitleParametre : public Parametre {
public:
    TitleParametre( const char *comment, ParamList *pList=0 );
    
    virtual void Read();
    virtual void Help( );
    virtual void Status( ostream& out );

    virtual ostream& PrintValue( ostream &s );
    virtual ostream& PrintDefault( ostream &s );
    virtual char *SPrintValue( char * );
};


class ArgTemplate {
public:
    String	keyword;
    String	comment;
    String	defValue;

    ArgTemplate( const char *key, const char *comm, const char *dflt )
	: keyword(key), comment(comm), defValue(dflt) {};
};


// --- Tableau de parametres de chaines. ---
// C'est en fait une matrice de [nbLines x nbCols] WordParametre
// alloues dynamiquement a la lecture des parametres
// Ex: pour gerer un ensemble de parametres de cette tete:
//	VARIABLES	X0 X1 X2	# les variables
//	X0.RANGE	3.2
//	X1.RANGE	2.4
//
// on declarera:
// ArrayParametre LesVar( "VARIABLES", "les Variables", "X0", 
//			  1, "RANGE", "maximum de var.", "1.0" );
// ou "1.0" est la valeur par defaut du range des var. (char *)
// pour recuperer: 		on utilisera:
// --------------		-------------
//	le nb de var		LesVar.NbLines();
//	la ieme var		LesVar.Word(i);
//	l'indice d'une var	LesVar["X0"];
//	le range de X0		LesVar.Value( "X0", "RANGE" );
// -----------------------------------------
class ArrayParametre : public WordParametre {
    int			nbCols;
    int			nbLines;
    String		*Words;
    ArgTemplate		**argTmpl;
    WordParametre	**argMatrix;
    ParamList		*theList;

public:
    // --- le constructeur: un peu delicat ---
    // nbElem est le nombre de triplets de chaines de
    // caract. qui doivent suivre.
    ArrayParametre( const char *key, const char *comment, 
		    const char *dflt,
		    int	nbElem, ... );


    virtual void Read();
    virtual void Help( );

    // --- Quelques accesseurs ---
    char *Word( int nb );		// renvoit le mot nb
    int operator[]( const char *word );	// renvoit l'indice de mot
					// -1 s'il n'y est pas
    char *operator[]( const int nb ) {	// synonyme de Word(nb)
	return Word( nb ); }

    int ColNumber( const char *subKey );

    // --- Nb lignes/colonnes (nbCol=nbElem dans le constructeur) ---
    int	NbLines( ) { return nbLines; };
    int	NbCols( ) { return nbCols; };

    char *Value( char *key, char *subKey ); // Renvoi le param key.subKey
    char *Value( int nb, char *subKey ); // idem (avec le ieme param)
};



class ParamElt {
    Parametre*	param;			// pointe sur le param lui meme
    ParamElt*	next;			// le suivant

public:
    ParamElt( Parametre *p, ParamElt *n=0 ) : param(p), next(n) {};
    
    ParamElt *insertAfter( Parametre *p );

    ParamElt *getNext() { return next; };
    Parametre *getParam() { return param; };
};


// 
// Liste de parametres (nouvelle mouture)
//
// Les ParamList sont chainees entre elles (par nextList)
// elles peuvent avoir des sous-listes (subList) qui heriteront
// du prefix courant (s'il y en a un).
//
// Les methodes Read/Status/Help sont appliquees en sequences:
//	- a tous les parametres de la liste
//	- a toutes les sous-listes de la liste
//
class ParamList {
// --------------
    String	prefix;			// Le prefixe des param
    int		level;			// Le niveau de cette liste
    ParamElt	*lesParam;		// La liste des parametres
    ParamList	*subList;		// Les sous listes
    ParamList	*nextList;		// La liste suivante (dans la sous-liste
					// du pere en cours)
    ParamList	*upper;			// La liste du dessus 
					// (qui contient this dans subList)

    void addSubList( ParamList* l );	// Ajout d'une liste dans la subList

public:
    // --- Constructeur ---
    ParamList( char *prfx,		// Prefix a ajouter aux elt de la liste
	       int lev,			// Le niveau ds la liste en cours (pour
					// ordonner les diffs sous-listes)
	       char *title=0,		// Un titre ajoute tout de suite apres
	       ParamList *up=0 );	// La liste du dessus (mainList sinon)

    void add( Parametre* param );	// Ajout un parametre dans cette liste

    // --- Declenche les methodes pour tous les elt de la liste ---
    void Read();
    void Help( );
    void Status( ostream& out );

    String	getPrefix() { return prefix; }

};

#endif // 		PARAMETRE_H

