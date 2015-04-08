// ----------------------------------------------------------------------
// Where........: CMAP - Polytechnique 
// File.........: mystring.C
// Author.......: Bertrand Lamy (Equipe genetique)
// Created......: Fri Jun  9 17:50:59 1995
// Description..: Class string avec allocation auto
// 
// Ident........: $Id: mystring.C,v 1.5 1996/04/30 22:21:49 lamy Exp $
// ----------------------------------------------------------------------

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "mystring.h"

inline int mystrlen( const char *s ) {
    int	len = 0;
    if( s )
	len = strlen( s );
    return len;
}

// 
// Fonction de copie:
// renvoie le pointeur sur le dernier octet
// 
static char *Copy( char *to, const char *from )
// --------------------------------------------
{
    char	*pTo = to;
    const char	*pFrom = from;
    if( pFrom ) 
	while( *pFrom )	
	    (*pTo++) = (*pFrom++);
    *pTo = 0;
    return pTo;
}


// 
// Allocation de la memoire, doit etre utilisee partout
// pour allouee l'espace memoire
// On suppose qu'on alloue toujours la place necessaire
//	(length == strlen par la suite)
// 
void String::Allocate( const int n )
// ---------------------------------
{
    length = n;
    str = new char[n+1];
}


String::String()
{
    str = 0;
    length = 0;
}

String::String( const char *value )
{
    Allocate( mystrlen( value ) );
    Copy( str, value );
}

String::String( const String& value )
{
    Allocate( value.Length() );
    Copy( str, (char *)value );
}

String::~String()
{
    if( str )
	delete [] str;
    length = 0;
}


String & String::operator=( const char *value )
{
    if( str )
	delete [] str;
    Allocate( mystrlen( value ) );
    Copy( str, value );
    return( *this );
}

String & String::operator=( const String& value )
{
    if( str != value.str ) {
	if( str )
	    delete [] str;
	Allocate( value.Length() );
	Copy( str, (char *) value );
    }
    return *this;
}


// --- Cast en char * ---
String::operator char *() const
{
  return( str );
}

char String::operator[]( int i ) const
{
    if( (i > length) || (i < 0) )
	return 0;
    return str[i];
}

// --- Operateurs de comparaison ---
// =================================

int operator==( const String& a, const String& b )
{
    return( strcmp( (char *)a, (char *)b) == 0 );
}

int operator!=( const String& a, const String& b )
{
    return( strcmp( (char *)a, (char *)b) != 0 );
}

int operator>( const String& a, const String& b )
{
    return( strcmp((char *)a, (char *)b) > 0 );
}

int operator<( const String& a, const String& b )
{
    return( strcmp((char *)a, (char *)b) < 0 );
}

// --- Compte les caracteres communs ---
int String::isBeginingOf( const String& other )
{
    int	result = 0;
    for( int i=0; (i < Length()) && (i < other.Length()); i++ ) {
	if( str[i] == other.str[i] )
	    result++;
	else	
	    break;
    }
    return ( result == Length() );
}


// --- Manipulation ---
// ====================


// 
// Fonction bas niveau de creation d'un chaine a partir de deux ss/chaines
// 
String String::BuildFromTwo( const char *a, int aLen, 
// -------------------------------------------------
			    const char *b, int bLen )
{
    String *result = new String;
    result->Allocate( aLen + bLen );	
    char *p = Copy( result->str, a );
    Copy( p, b );
    return *result;
}


// 
// Concatenation de deux chaines
// 
String operator+( const String& a, const String& b )
// -----------------------------------
{	
    return String::BuildFromTwo( (char *)a, a.Length(), (char *)b, b.Length() );
}

// 
// Concatenation de deux chaines: char * et String
// 
String operator+( const char *a, const String& b )
// -----------------------------------
{	
    return String::BuildFromTwo( a, mystrlen(a), (char *)b, b.Length() );
}

// 
// Concatenation de deux chaines: String et char *
//
// 
String operator+( const String& a, const char *b )
// -----------------------------------
{	
    return String::BuildFromTwo( (char *)a, a.Length(), b, mystrlen(b) );
}


// --- Mise en forme "a la" ostream ---


String& String::operator<<( const String& v )
// ------------------------------------------
{
    *this = *this + v;
    return *this;
}


String& String::operator<<( const char *v )
// ------------------------------------------
{
    *this = *this + v;
    return *this;
}

String& String::operator<<( const char v )
// ---------------------------------------
{
    static char	buffer[2] = { 0, 0 };
    buffer[0] = v;
    *this = *this + buffer;
    return *this;
}

String& String::operator<<( const int v )
// --------------------------------------
{
    char	buffer[80];
    sprintf( buffer, "%d", v );
    *this = *this + buffer;
    return *this;
}

String& String::operator<<( const long v )
// ---------------------------------------
{
    char	buffer[80];
    sprintf( buffer, "%ld", v );
    *this = *this + buffer;
    return *this;
}


String& String::operator<<( const float v )
// ----------------------------------------
{
    char	buffer[80];
    sprintf( buffer, "%f", v );
    *this = *this + buffer;
    return *this;
}

String& String::operator<<( const double v )
// -----------------------------------------
{
    char	buffer[80];
    sprintf( buffer, "%lf", v );
    *this = *this + buffer;
    return *this;
}


// --- Interaction avec les stream ---

ostream& operator<<(ostream& stream, const String& aString)
{
    return stream << aString.str ;
}


// 
// Definit les car. blanc
// 
inline int Trimable( char c )
// --------------------------
{
    return ( (c == ' ') || (c == '\t') );
}

// 
// Enleve les blancs/TAB aux extremites
// 
int String::Trim()
// ----------------
{
    int		i;
    int		left=0, right=0;

    for( i=0; (i < Length()) && Trimable( str[i] ); i++ )
	left++;
    for( i=Length()-1; (i >= 0) && Trimable( str[i] ); i-- )
	right++;

    if( left+right ) {
	char	*old = str;
	old[Length() - right] = '\0';
	Allocate( Length() - (left+right) );
	Copy( str, old+left );
	delete [] old;
    }
    return (left+right);
}

