// ----------------------------------------------------------------------
// Where........: CMAP - Polytechnique 
// File.........: parametre.C
// Author.......: Bertrand Lamy (Equipe genetique)
// Created......: Fri Feb  3 18:35:08 1995
// Description..: Implementation des classes 
//		  Parametre
//		  <Type>Parametre
//		  EnsParametre
//
// Ident........: $Id: parametre.C,v 1.11 1996/05/30 16:55:07 marc Exp $
// ----------------------------------------------------------------------


#include <stdarg.h>
#include <string.h>
#include "parametre.h"
#include "lecture.h"

//#include <stdio.h>
#include <sstream>
using namespace std;


#define PA_SIZE_VAL	8		// Taille d'aff des valeurs
#define PA_COMMENT	"# "		// le separateur de commentaires

// --- Initialisation des variables de classe ---
// ----------------------------------------------
ParamList 	Parametre::mainList( 0, 0 );
ParamList *	Parametre::curList = & mainList;

FICH_IN * 	Parametre::Fin = 0;
int 		Parametre::Argc = 0;
char ** 	Parametre::Argv = 0;

int 		Parametre::sourcesAreOk = 0;


// --- Class Parametre ---
// =======================

// 
// Fixe la provenance des parametres: le ficher de parametres
// 
void Parametre::SetSources( WordParametre & FichParam, int argc, char **argv )
// --------------------------------------------------------------
{
    Argc = argc;
    Argv = argv;
    FichParam.defValue = * new String(argv[0]); // alloue et duplique
    FichParam.Read();	// lu uniquement sur la ligne de commande (+ getenv)
    if (FichParam.value != FichParam.defValue) // lu autre chose que le defaut
      {
	String nomLoc(FichParam.value); // pour le message d'erreur
	Fin = Ouvre_FICH_IN_Non_Fatale( FichParam.value ); // peut etre absent
	if( !Fin )		// rate : on essaye avec .param
	  {
	    FichParam.value = FichParam.value + ".param";
	    Fin = Ouvre_FICH_IN_Non_Fatale( FichParam.value );
	  }
	if (!Fin)		// la, ca ne va plus !
	  {
	    cerr << "Pas de fichier "<< (char *)nomLoc << " ni " << (char *)FichParam.value << " : on abandonne !\n" << endl; 
	    exit(1);
	  }
      }
    else			// on est reste au defaut (argv[0])
      {
	FichParam.value = FichParam.value + ".param";
	Fin = Ouvre_FICH_IN_Non_Fatale( FichParam.value ); // peut etre absent
      }

    // Fin des tentatives d'ouverture
    if( Fin ) {
      Fin_De_Fichier_Non_Fatale( Fin );
      M_Delim( Fin, '=' );
    }
    sourcesAreOk = 1;
}


// 
// Renvoie le fichier de parametres
// 
FICH_IN *Parametre::GetFile()
// --------------------------
{
    return Fin;
}

// 
// Ferme le fichier de lecture des parametres
// 
void Parametre::Ferme()
// --------------------
{
    if( Fin )
	Ferme_FICH_IN( Fin );
}

// 
// Regle la liste courante
// Les parametres ajoutes par la suite seront dans cette liste
// 
void Parametre::setCurrentList( ParamList *l )
// -------------------------------------------
{
    curList = l;
}
//
// Retourne la liste courante
//
ParamList * Parametre::getCurrentList( )
// -------------------------------------------
{
    return curList;
}

//
// Retourne la liste principale
//
ParamList * Parametre::getMainList( )
// -------------------------------------------
{
    return & mainList;
}

// 
// Verifie que les sources ont ete fixees
// 
void Parametre::CheckSources()
// ---------------------------
{
    if( !Fin  && !Argv ) {
	cerr << "Impossible d'initialiser les parametres\n";
	cerr << "Pas de sources specifiees (utiliser Parametre::SetSources)\n";
    }
}


// 
// Lecture de tous les parametres
// 
void Parametre::AllRead()
// ----------------------
{
    mainList.Read();
}

// 
// Affiche tous les messages d'aide
// 
void Parametre::AllHelp()
// ----------------------
{
    mainList.Help();
}

// 
// Affiche tous les statuts
// 
void Parametre::AllStatus( ostream& out )
// --------------------------------------
{
    mainList.Status( out );
}

// 
// Constructeur
// regles les attributs generique des Parametres
// l'ajoute dans une liste de parametre
// regle le prefix du parametre en fonction de celui de la liste
//
Parametre::Parametre( const char *key, const char *comment,
		      ParamList *pList )
    : read(0), Comment(comment)
// --------------------------------------------------------------------
{
    if( ! pList ) {			// Le param a ete cree sans liste
	if( curList )			// S'il y a une liste courante, on la prend
	    pList = curList;
	else
	    pList = &mainList;		// Sinon on prend la principale
    }
    pList->add( this );			// De ttes facons, on l'ajoute
    
    if( pList->getPrefix().Length() )
	KeyWord << pList->getPrefix() << "." << key;
    else
	KeyWord = key;
}

// 
// Destructeur
// 
Parametre::~Parametre()
// --------------------
{
}


// 
// Affiche l'aide
// 
void Parametre::Help( )
// --------------------
{
  cerr << (char *)KeyWord << " " << (char *)Comment ;
  cerr << " [";
  PrintDefault( cerr );
  cerr << "]" << endl;
}

// 
// Affichage du statut
// 
void Parametre::Status( ostream& out )
// -----------------------------------------
{
    char 	buffer[80];
    out	<<  (char *)KeyWord << " " ;
    SPrintValue( buffer );
    out <<  buffer << "\t" << PA_COMMENT << Comment << endl;
}


// --- Class DoubleParametre ---
// =============================


// 
// Le constructeur:
// Initialise les differents composants,
// et appelle, si c'est possible la methode Read.
// 
DoubleParametre::DoubleParametre( const char *key, 
// ------------------------------------------------
				  const char *comment, 
				  const double dflt,
				  ParamList *pList )
    : Parametre( key, comment, pList ), 
      value(dflt), defValue(dflt)
{
    if( sourcesAreOk )
	Read();
}


// 
// Lecture du parametre
// 
void DoubleParametre::Read()
// -------------------------
{
    CheckSources();
    value = Lecture_Reel( Fin, Argc, Argv, KeyWord, defValue );
    read = 1;
}

// 
// Affiche la valeur
// 
ostream &DoubleParametre::PrintValue( ostream &out )
// -------------------------------------------------
{
    return out << value ;
}

// 
// Affiche la valeur par defaut
// 
ostream &DoubleParametre::PrintDefault( ostream &out )
// -------------------------------------------------
{
    return out << defValue;
}

// 
// Affiche la valeur dans une chaine
// 
char *DoubleParametre::SPrintValue( char * out )
// -------------------------------------------------
{
    sprintf( out, "%g", value );
    return out;
}

// 
// Affectation
// 
DoubleParametre & DoubleParametre::operator=( const double a )
// -----------------------------
{
    value = a;
    return *this;
}

// --- Class IntParametre ---
// ==========================


// 
// Constructeur
// 
IntParametre::IntParametre( const char *key, const char *comment,
// --------------------------------------------------------------
			    const int dflt, ParamList *pList )
    : Parametre( key, comment, pList ), 
      value(dflt), defValue(dflt)
{
    if( sourcesAreOk )
	Read();
}


// 
// Lecture
// 
void IntParametre::Read()
// ----------------------
{
    CheckSources();
    value = (int)Lecture_Entier( Fin, Argc, Argv, KeyWord, defValue );
    read = 1;
}

// 
// Affiche la valeur
// 
ostream &IntParametre::PrintValue( ostream &out )
// -------------------------------------------------
{
    return out <<  value ;
}

// 
// Affiche la valeur par defaut
// 
ostream &IntParametre::PrintDefault( ostream &out )
// -------------------------------------------------
{
    return out << defValue;
}

// 
// Affiche la valeur dans une chaine
// 
char *IntParametre::SPrintValue( char * out )
// -------------------------------------------------
{
    sprintf( out, "%d", value );
    return out;
}

// 
// Affectation
// 
IntParametre & IntParametre::operator=( const int a )
// -----------------------------
{
    value = a;
    return *this;
}


// --- Class UintParametre ---
// ==========================

// 
// Constructeur avec valeur en %, on memorise la chaine d'init
// et le pointeur sur la valeur de reference
// 
UintParametre::UintParametre( const char *key, const char *comment,
// ----------------------------------------------------------------
			      const char *defValueStr, UintParametre *ref, 
			      ParamList *pList )
    : Parametre( key, comment, pList ), value(0), defValue(0),
      defString(defValueStr), From(ref), from(0)
{
    if( sourcesAreOk )
	Read();
}
// 
// Constructeur avec valeur en %, on memorise la chaine d'init
// et le pointeur sur la valeur de reference
// 
UintParametre::UintParametre( const char *key, const char *comment,
// ----------------------------------------------------------------
			      const char *defValueStr, unsigned int *ref, 
			      ParamList *pList )
    : Parametre( key, comment, pList ), value(0), defValue(0),
      defString(defValueStr), From(0), from(ref)
{
    if( sourcesAreOk )
	Read();
}

// 
// Constructeur classique, avec juste une valeur par defaut
// 
UintParametre::UintParametre(const char *key, const char *comment, 
// -------------------------------------------------------------
			   const unsigned int dflt, ParamList *pList )
    : Parametre( key, comment, pList ), value(dflt), defValue(dflt), 
      defString(0), From(0), from(0)
{
    if( sourcesAreOk )
	Read();
}


// 
// Lecture du parametre.
// Prend en compte s'il a ete cree avec ou sans valeur de reference
// 
void UintParametre::Read()
// -----------------------
{
    CheckSources();
    if( From ) {
	// --- Il faut lire la reference, si ce n'est deja fait ---
	if( ! From->isRead() )
	    From->Read();

	// --- C'est peut-etre un pourcentage ---
	char *ptr = Lecture_Ligne( Fin, Argc, Argv, KeyWord, defString );
	value = (unsigned int) Lit_Entier( ptr );
	if( Lit_Char() == '%' ) {
	    defValue = value;		// On memorise le %-tage
	    value = ((value > 100) ? (unsigned int) (*From) 
		     : (unsigned int)(value * ((unsigned int)(*From) / 100.0)));
	}
    }
    else if (from)		// reference ss forme d'unsigned int *
      {
	// --- C'est peut-etre un pourcentage ---
	char *ptr = Lecture_Ligne( Fin, Argc, Argv, KeyWord, defString );
	value = (unsigned int) Lit_Entier( ptr );
	if( Lit_Char() == '%' ) {
	    defValue = value;		// On memorise le %-tage
	    value = ((value > 100) ? (*from) 
		     : (unsigned int)(value * ((*from) / 100.0)));
	}
      }
    else {
	// --- Valeur absolue (constructeur sans reference) ---
	value = (unsigned int)Lecture_Entier( Fin, Argc, Argv, KeyWord, defValue );
    }
    read = 1;
}


// 
// Affiche la valeur
// 
ostream &UintParametre::PrintValue( ostream &out )
// -------------------------------------------------
{
    if( From && defValue )
	// --- La valeur par defaut exprime le pourcentage ---
	return out <<  defValue ;

    return out << value ;
}

// 
// Affiche la valeur par defaut
// 
ostream &UintParametre::PrintDefault( ostream &out )
// -------------------------------------------------
{
    if( From )
	return out << defString;
    return out << defValue;
}

// 
// Affiche la valeur dans une chaine
// 
char *UintParametre::SPrintValue( char * out )
// -------------------------------------------------
{
    if( From && defValue )
	sprintf( out, "%d %%", defValue );
    else
	sprintf( out, "%d", value );
    return out;
}

// 
// Affectation
// 
UintParametre & UintParametre::operator=( const unsigned int a )
// -----------------------------
{
    value = a;
    return *this;
}


// --- Class LongParametre ---
// ===========================

LongParametre::LongParametre( const char *key, const char *comment, 
			      const long dflt, ParamList *pList )
	: Parametre( key, comment, pList ), 
	  value(dflt), defValue(dflt)
{
    if( sourcesAreOk )
	Read();
}


// 
// Lecture
// 
void LongParametre::Read()
// -----------------------
{
    CheckSources();
    value = Lecture_Entier( Fin, Argc, Argv, KeyWord, defValue );
    read = 1;
}

// 
// Affiche la valeur
// 
ostream &LongParametre::PrintValue( ostream &out )
// -------------------------------------------------
{
    return out << value ;
}

// 
// Affiche la valeur par defaut
// 
ostream &LongParametre::PrintDefault( ostream &out )
// -------------------------------------------------
{
    return out << defValue;
}

// 
// Affiche la valeur dans une chaine
// 
char *LongParametre::SPrintValue( char * out )
// -------------------------------------------------
{
    sprintf( out, "%ld", value );
    return out;
}

// 
// Affectation
// 
LongParametre & LongParametre::operator=( const long a )
// -----------------------------
{
    value = a;
    return *this;
}

// --- Class WordParametre ---
// ===========================

// 
// Contructeur
// 
WordParametre::WordParametre( const char *key, const char *comment, 
			     const char* dflt, ParamList *pList )
    : Parametre( key, comment, pList ),
      value(dflt), defValue(dflt)
{
    if( sourcesAreOk )
	Read();
}

// 
// Lecture
// 
void WordParametre::Read()
// -----------------------
{
    CheckSources();
    value = Lecture_Ligne( Fin, Argc, Argv, KeyWord, defValue );
    // --- Enleve les blancs/TAB a la fin ---
    value.Trim();

    read = 1;
}

// 
// Affiche la valeur
// 
ostream &WordParametre::PrintValue( ostream &out )
// -------------------------------------------------
{
    return out << value ;
}

// 
// Affiche la valeur par defaut
// 
ostream &WordParametre::PrintDefault( ostream &out )
// -------------------------------------------------
{
    return out << defValue;
}

// 
// Affiche la valeur dans une chaine
// 
char *WordParametre::SPrintValue( char * out )
// -------------------------------------------------
{
    sprintf( out, "%s", (char *)value );
    return out;
}

// 
// Affectation
// 
WordParametre & WordParametre::operator=( char* a )
// -----------------------------
{
    value = a;
    return *this;
}

// --- Les titres dans les parametres ---
// --------------------------------------

// 
// Createur d'un titre
// 
TitleParametre::TitleParametre( const char *comment, 
				ParamList *pList )
// -------------------------------------------------
    : Parametre( 0, comment, pList ) 
{
}


void TitleParametre::Read()
{
}

void TitleParametre::Help( )
{
    cerr << "------ " << Comment << "\n";
}

void TitleParametre::Status( ostream& out )
{
    out << "\n" << PA_COMMENT << "\t" << Comment << "\n";
    out << PA_COMMENT << "\t" ;
    for( int i=0; i < Comment.Length(); i++ )
	out << "-";
    out << "\n";
}

// --- Dummy printings ---

ostream& TitleParametre::PrintValue( ostream &s ) { 
    return s; 
}

ostream& TitleParametre::PrintDefault( ostream &s ) { 
    return s;
}

char * TitleParametre::SPrintValue( char * ) { 
    return 0;
}



// --- Tableaux de parametres ---
// ==============================



// 
// Le constructeur.
// memorise les champs a crer apres la creation
// 
ArrayParametre::ArrayParametre( const char *key, 
// ----------------------------------------------
				const char *comment,
				const char *dflt,
				int	nbElem, ... )
    : WordParametre( key, comment, dflt )
{
    va_list	ar;
    va_start( ar, nbElem );
    nbCols = nbElem;
    argTmpl = new ArgTemplate*[nbCols];
    for( int i=0; i < nbCols; i++ ) {
	char	*arKey = va_arg( ar, char * );
	char	*arComment = va_arg( ar, char * );
	char	*arDflt = va_arg( ar, char * );
	
	argTmpl[i] = new ArgTemplate( arKey, arComment, arDflt );
    }
    va_end( ar );
    theList = getCurrentList();

    if( sourcesAreOk )
	Read();
}

// 
// Lecture des tableaux de parametres
// 
void ArrayParametre::Read()
// ------------------------
{
    char	aWord[80];
    ParamList	*saveList = getCurrentList(); // Sauve la liste courante

    // --- Remet la liste utilisee pour la creation de l'array ---
    setCurrentList( theList );

    WordParametre::Read();		// lecture de la valeur
    // --- On compte le nombre de mots dans la chaine ---
    Lit_Mot( value, aWord );
    nbLines = 1;
    while( Lit_Mot( (char *)0, aWord ) )
	nbLines++;

    argMatrix = new WordParametre*[nbLines*nbCols];
    Words = new String[nbLines];
    // --- On reparcourt tous les mots ---
    Lit_Mot( value, aWord );	
    for( int i=0; i < nbLines; i++, Lit_Mot( (char *)0, aWord) ) {
	Words[i] = aWord;
	
	for( int j=0; j < nbCols; j++ ) {
	    String aKey;
	    String fullComment;
	    aKey << aWord << "." << argTmpl[j]->keyword;
	    fullComment << argTmpl[j]->comment << " " << aWord;

	    argMatrix[i*nbCols + j] = 
		new WordParametre( aKey, fullComment, argTmpl[j]->defValue );
	}
    }
    setCurrentList( saveList );		// restaure la liste
    read = 1;
}

// 
// Affichage de l'aide
// 
void ArrayParametre::Help()
// ------------------------
{
    Parametre::Help();
    for( int i=0; i < nbCols; i++ ) {
      cerr << (char *)argTmpl[i]->keyword << " : " <<	
		      (char *)argTmpl[i]->comment ;
	cerr << "[" << argTmpl[i]->defValue << "]" << endl;
    }
}

// --- Accesseurs sur les valeurs stockees ---
// --- dans les tableaux ---

// 
// Renvoit le ieme mot-parametre de la chaine d'entete
// 
char *ArrayParametre::Word( int nb )
// ---------------------------------
{
    if( nb >= nbLines )
	return 0;
    return Words[nb];
}

int ArrayParametre::operator[]( const char *word )
{
    for( int i=0; i < nbLines; i++ )
	if( !strcmp( word, Words[i] ) )
	    return i;
    return -1;
}

int ArrayParametre::ColNumber( const char *subKey )
{
    for( int i=0; i < nbCols; i++ )
	if( !strcmp( subKey, argTmpl[i]->keyword ) )
	    return i;
    return -1;
}

char *ArrayParametre::Value( char *key, char *subKey )
{
    int	line = this->operator[](key);
    int	col = ColNumber( subKey );
    if( (line == -1) || (col == -1) 
	|| (line >= nbLines) || (col >= nbCols) )
	return 0;
    return( *argMatrix[line*nbCols+col] );
}

char *ArrayParametre::Value( int nb, char *subKey )
{
    int	col = ColNumber( subKey );
    if( (nb == -1) || (col == -1) 
	|| (nb >= nbLines) || (col >= nbCols) )
	return 0;
    return( *argMatrix[nb*nbCols+col] );
}


// --- Element de liste contenant un parametre ---
// ===============================================

ParamElt *ParamElt::insertAfter( Parametre *p )
{
    ParamElt	*old = next;
    return next = new ParamElt( p, old );
}

// --- Liste de parametres ---
// ===========================


//
// Createur
//
ParamList::ParamList( char *prfx, int lev, char *title,
		      ParamList *up )
    : prefix(prfx), level(lev), lesParam(0), 
      subList(0), nextList(0)
// ----------------------------------------------------
{
    if( up )
	upper = up;
    else
	upper = Parametre::getMainList();
    // --- quand la mainList est cree upper == this!!! ---
    if( upper != this ) {
	// --- On ajoute this en subList de up ---
	upper->addSubList( this );
	// --- et on recupere son prefix ---
	if( upper->prefix.Length() && prefix.Length() )
	    prefix = upper->prefix + "." + prefix;
	else if( upper->prefix.Length() ) 
	    prefix = upper->prefix;
    }
    Parametre::setCurrentList( this );	// this devient liste courante
    if( title && *title )		// et commence eventuellement
	new TitleParametre( title );	// par un titre
}


// 
// Ajoute une liste dans la chaine des sous liste
// 
void ParamList::addSubList( ParamList *l )
// ---------------------------------------
{
    if( ! subList )			// Premiere sous liste
	subList = l;
    else {
	if( subList->level > l->level ) { // A mettre en tete de liste
	    l->nextList = subList;
	    subList = l;
	}
	else {				// On va chercher sa place
	    ParamList *p = subList;
	    while( (p->nextList) && (p->nextList->level <= l->level) )
		p = p->nextList;
	    l->nextList = p->nextList;
	    p->nextList = l;
	}
    }

}


// 
// Ajoute un parametre dans la liste des parametres
// 
void ParamList::add( Parametre* param )
// ------------------------------------
{
    if( ! lesParam ) {			// Premier element ajoute
	lesParam = new ParamElt( param );
    }
    else {				// La liste n'est pas vide
	// --- Recherche de la bonne place ---
	ParamElt*	ptr = lesParam;
	while( ptr->getNext() )
	    ptr = ptr->getNext();
	ptr->insertAfter( param );
    }
}


// --- Declenchement des methodes Read/Help/Status sur ---
// --- tous les parametres et sur les sous listes ---
void ParamList::Read()
{
    ParamElt*	ptr = lesParam;
    while( ptr ) {
	if( ! ptr->getParam()->isRead() )
	    ptr->getParam()->Read();
	ptr = ptr->getNext();
    }
    ParamList*	sub = subList;
    while( sub ) {
	sub->Read();
	sub = sub->nextList;
    }
}
void ParamList::Status( ostream& out )
{
#ifdef DEBUG
    out << "##PARAM##  " << "prefix: [" << prefix
	<< "]   level: " << level << endl;
#endif
    ParamElt*	ptr = lesParam;
    while( ptr ) {
	ptr->getParam()->Status( out );
	ptr = ptr->getNext();
    }
    ParamList*	sub = subList;
    while( sub ) {
	sub->Status( out );
	sub = sub->nextList;
    }
}
void ParamList::Help()
{
    ParamElt*	ptr = lesParam;
    while( ptr ) {
	ptr->getParam()->Help();
	ptr = ptr->getNext();
    }
    ParamList*	sub = subList;
    while( sub ) {
	sub->Help();
	sub = sub->nextList;
    }
}
