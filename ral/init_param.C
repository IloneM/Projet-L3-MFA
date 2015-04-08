// ----------------------------------------------------------------------
// Where........: CMAP - Polytechnique 
// File.........: init_param.C
// Author.......: Marc Schoenauer & Bertrand Lamy
// Created......: Tue Mar 21 13:32:00 1995
// Description..: Initialisation des parametres de la population,
//		  affichage du statut et de l'aide
// 
// Ident........: $Id: init_param.C,v 1.13 1998/03/10 13:14:36 marc Exp $
// ----------------------------------------------------------------------

#include <string.h>
#include <sstream>
using namespace std;
#include "myrand.h"
#include "popu.h"
#include "sel.h"


typedef char*	Pchar;


///////////////////////////////////////////////////
void popu::init_param(int argc, char *argv[])
///////////////////////////////////////////////////
{
  // les strategies d'arret
  //-----------------------
  if( ((int)MaxEval && (int)MaxGen)
      || !((int)MaxEval || (int)MaxGen)) {
    cout << "Un et un seul des parametres MAX_EVAL ou MAX_GEN "
	 << "doit etre non nul\n";
    exit( -1 );
  }
  if ((int)MaxEval)
    MaxAge = MaxEval;
  else
    MaxAge = MaxGen;

  // le probleme
  // -----------
  // choix eventuel du type de chromosome (et init des parametres idoines)
  chromosome* tempo=meilleur->init_param(this);
  if ( (tempo) && (tempo != meilleur) )  // si 0, ou le meme, ne rien changer
    {
      delete meilleur;
      meilleur = tempo;	// maintenant, c'est le bon type
    }
  // --- Il faut que meilleur soit un VRAI chromosome ---
  // car il risque d'etre evalue. Jusqu'a la, il n'etait pas forcement valide.
  /////////////////////////////
  tempo = meilleur->_clone_random();
  delete meilleur;
  meilleur = tempo; 

  //-----------------------------------------------------   
  // initialisation de la population
  // On charge les noms de fichiers qui nous interessent
  // c'est a dire les param de la forme:
  //		popname=<ficher>
  //-----------------------------------------------------
  int i;
  String	popuTag = Name + "=";
  for( i=1; i < argc; i++ ) {
    if( popuTag.isBeginingOf( argv[i] ) ) {
      // L'argument commence par le nom de la population 
      historiquec++;
    }
  }
  if (historiquec)
    {
      historique = new Pchar[historiquec];
      nb_historique = new int[historiquec];
    }
  int	idxHisto=0;
  for( i=1; i < argc; i++ ) {		// on recommence
    if( popuTag.isBeginingOf( argv[i] ) ) {
      historique[idxHisto] = &( argv[i][popuTag.Length()] );	
      nb_historique[idxHisto] = 0;	// A mettre a jour...
      idxHisto++;
    }
  }
  init();	// init aleatoire (historiquec=0) ou a partir des fichiers
  // il faut que le premier meilleur soit un membre de la premiere population:
  delete meilleur;
  // on force l'evaluation avant le clonage (pas tres elegant!)
  GetChromo(0)->perf();
  meilleur = GetChromo(0)->_clone(); 
}



