// ----------------------------------------------------------------------
// Where........: CMAP - Polytechnique 
// File.........: genutil.C
// Author.......: Marc Schoenauer
// Created......: Tue Mar 21 13:47:12 1995
// Description..: Creation de masque pour cross over
// 
// Ident........: $Id: genutil.C,v 1.1 1995/04/12 13:52:36 lamy Exp $
// ----------------------------------------------------------------------

#include <stdlib.h>
//#include <stream.h>
#include <math.h>
#include <string.h>
#include <sstream>
using namespace std;

#include "myrand.h"
#include "genutil.h"


// classique a 1 point
//----------------------------------------------
void cree_masque_1(char* mask, int taille)
//----------------------------------------------
{
int i ;
int pt = rnd(1,taille);             // lieu du croisement
for (i=0; i<pt; i++)
    mask[i]=0;
for (i=pt; i<taille; i++)
    mask[i]=1;
}


// et le uniforme 
//----------------------------------------------
void cree_masque_u(char* mask, int taille)
//----------------------------------------------
{
int i;
for (i=0; i<taille; i++)
    if (flip(.5))
        mask[i]=1;
    else
        mask[i]=0;
}

//----------------------------------------------
void cree_masque(char* mask, int type, int taille)
//----------------------------------------------
{
switch (type) {
    case 0 : cree_masque_u(mask, taille) ;
             break ;
    case 1 : cree_masque_1(mask, taille) ;
             break ;
    }
return;
}
 
