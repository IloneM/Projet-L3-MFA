// ----------------------------------------------------------------------
// Where........: CMAP - Polytechnique 
// File.........: appli.C
// Author.......: Bertrand Lamy
// Created......: Wed Apr 17 19:56:31 1996
// Description..: Points d'entree necessaire pour l'appli.
//		  Gere le fichier de parametre, de status, 
//		  et l'interruption des signaux
// 
// Ident........: $Id: appli.C,v 1.4 1996/05/30 16:57:31 marc Exp $
// ----------------------------------------------------------------------

#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sstream>
#include <fstream>
using namespace std;

#include "appli.h"
#include "parametre.h"
#include "mystring.h"
#include "myrand.h"

WordParametre FichParam("PARAM", "Nom du fichier de parametres (ligne de commande seulement)", "");


TitleParametre	titAppli( "Application", 0 );
LongParametre	nrand( "INIT_ALEA", "Graine d'aleatoire (0 = calee sur l'horloge)",
		       0 );		// graine d'aleatoire

static char	*nom_pgm = 0;		// Le nom du programme


// --- Gestion de l'arret par Control-C ---
static int	arret_demande = 0;

// 
// Le gestionnaire de signaux, mis en veille par AppliStart
// 
void signal_handler( int sig )
// ---------------------------
{
    // --- On veut la paix, jusqu'a la fin ---
    signal( SIGINT, SIG_IGN );
    signal( SIGQUIT, SIG_IGN );
    cerr << "Signal: " << sig << endl;
    cerr << "Interruption demandee ... on ferme" << endl ;
    arret_demande = 1;
}


// 
// Permet de savoir si on a appuye sur Control-C
// (renvoie alors 1)
// 
int AppliArretDemande()
// --------------------
{
    return arret_demande;
}

// 
// Renvoit le nom du programme 
// (argv[0], stocke dans nom_pgm par AppliStart)
// 
char *AppliNomPgm()
// ----------------
{
    return nom_pgm;
}


// 
// Initialisation de l'appli:
//	- ouvre le fichier de parametres
//	- lecture des parametres daja crees
//	- gestion de la graine aleatoire
// 
void AppliStart( int argc, char *argv[] )
// --------------------------------------
{
    String	nomFic;
    nom_pgm = strdup( argv[0] );	// a garder pour les sauvegardes

   Parametre::SetSources(FichParam, argc, argv);
   Parametre::AllRead();
 
   nrand = init_random(nrand);      /* pour des pts de depart differents */
   
   // --- Gestionnaire de signaux ---
   // --- On intercepte le Ctrl-C pour arreter proprement ---
   signal( SIGINT, signal_handler );
   signal( SIGQUIT, signal_handler );
}


// 
// Au secours!
// 
void AppliHelp( int /* argc */, char *argv[] )
// -------------------------------------
{
  if (argv)
    {
    cerr << argv[0] << " [NomFic] options" << endl;
    cerr << "    Nomfic = nom du fichier de reprise (init aleatoire si blanc)\n";
    cerr << "    Options        : -MotCle=valeur, avec MotCle =\n";
    }
    Parametre::AllHelp();
}



//
// Verifie les arguments, enfin si possible...
// (en fait certains parametres ne sont pas encore forcement declares...)
// Affiche le statut de tous les parametres (dans le fichier .status)
// 
void AppliStatus( int argc, char *argv[] )
// ---------------------------------------
{
    // --- Verifie si on a besoin d'aide ---
    if( ! VerifieArguments( argc, argv ) ) {
	AppliHelp( argc, argv );
	exit( -1 );
    }
    for( int i=1; i < argc; i++ )
	if( !strcmp( argv[i], "help" ) ) {
	    AppliHelp( argc, argv );
	    exit( -1 );  
	}

    //-----------------------------------------------------   
    // Ecriture de tous les parametres.
    //-----------------------------------------------------   
    filebuf	buffloc;
    String	nomFic;
    nomFic << nom_pgm << ".status";
    if( buffloc.open( nomFic, ios_base::out ) == 0) {
	cout << "Impossible d'ouvrir le fichier .status en ecriture\n";
	exit(1);
    }
    ostream fout(&buffloc);

    fout << "# Evolution Artificielle Ver.2.0\n";
    fout << "# ==============================\n\n";

    Parametre::AllStatus( fout );

    buffloc.close();
}
