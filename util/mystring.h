// ----------------------------------------------------------------------
// Where........: CMAP - Polytechnique 
// File.........: mystring.h
// Author.......: Bertrand Lamy (Equipe genetique)
// Created......: Fri Jun  9 17:50:12 1995
// Description..: Class string, avec allocation/liberation auto
// 
// Ident........: $Id: mystring.h,v 1.4 1996/04/26 15:21:29 lamy Exp $
// ----------------------------------------------------------------------

#ifndef _MYSTRING_H
#define _MYSTRING_H

//#include <iostream.h>

#include <iostream>
using namespace std;

class String {
    char	*str;
    int		length;

    void Allocate( const int n );
    static String BuildFromTwo( const char *, int, const char *, int );

public:
    // --- Constructeurs/Destructeur ---
    String();
    String( const char * );
    String( const String& );
    ~String();

    // --- Affectations ---
    String & operator=( const char * );
    String & operator=( const String& );

    operator char *() const;		// Cast en char *
    int Length() const { return length; }
    char operator[]( int i ) const;	// return ieme char

    // --- operateurs de comparaison: egualite et diff ---
    friend int operator==( const String& a, const String& b );
    friend int operator!=( const String& a, const String& b );
    // --- comparaison lexicographique ---
    friend int operator>( const String& a, const String& b );
    friend int operator<( const String& a, const String& b );

    // --- Verifie si une chaine est le debut d'une autre ---
    int isBeginingOf( const String& other );

    // --- Addition de deux chaines: concatenation ---
    friend String operator+( const String& a, const String& b );
    friend String operator+( const char *a, const String& b );
    friend String operator+( const String& a, const char *b );

    // --- Mise en forme de chaines "a la" ostream ---
    String& operator<<( const String& v );
    String& operator<<( const int v );
    String& operator<<( const long v );
    String& operator<<( const char *v );
    String& operator<<( const char v );
    String& operator<<( const float v );
    String& operator<<( const double v );

    friend ostream& operator<<(ostream&, const String&);

    // --- Autres manipulations courantes ---
    int Trim();				// Enleve blancs/TAB aux extremites
					// Renvoi 0 si rien n'a ete fait
};


#endif // 		_MYSTRING_H
