// ----------------------------------------------------------------------
// Where........: CMAP - Polytechnique 
// File.........: appli.h
// Author.......: Bertrand Lamy
// Created......: Wed Apr 17 19:56:18 1996
// Description..: Points d'entree necessaires au bon fonctionnement
//		  de l'application
// 
// Ident........: $Id: appli.h,v 1.1 1996/04/24 23:31:05 lamy Exp $
// ----------------------------------------------------------------------


#ifndef _APPLI_H
#define _APPLI_H

// Ouverture des fichiers de parametres
void AppliStart( int argc, char *argv[] );

// Verification de la ligne de commande, et ecriture du fichier de status
void AppliStatus( int argc, char *argv[] );

// Renvoie 1 si on a appuye sur Control-C; il faut alors sortir.
int AppliArretDemande();

// Renvoie le nom du pgm (argv[0])
char *AppliNomPgm();


// Help est normalement appele par AppiStatus, si besoin est.
void AppliHelp( int argc, char *argv[] );

#endif // 		_APPLI_H

