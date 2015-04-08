// -*-C++-*--------------------------------------------------------------
// Where........: CMAP - Polytechnique 
// File.........: genutil.h
// Author.......: Bertrand Lamy (Equipe genetique)
// Created......: Tue Mar 21 13:37:58 1995
// Description..: Utilitaires pour les algos genetiques
//			Gestion de masques
// 
// Ident........: $Id: genutil.h,v 1.1 1995/04/12 13:52:37 lamy Exp $
// ----------------------------------------------------------------------


#ifndef _GENUTIL_H
#define _GENUTIL_H


// Renvoient des masques pour CrossOver: une chaine de char avec 0/1

// classique a 1 point
extern void cree_masque_1(char* mask, int taille);

// et le uniforme 
extern void cree_masque_u(char* mask, int taille);

// Cree un masque selon le type:
//	0 -> Uniforme
//	1 -> masque a un point
extern void cree_masque(char* mask, int type, int taille);
 

#endif // 		_GENUTIL_H
