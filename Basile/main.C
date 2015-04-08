// ----------------------------------------------------------------------
// Where........: CMAP - Polytechnique 
// File.........: main.C
// Author.......: Bertrand Lamy (Equipe genetique)
// Created......: Wed Apr 19 15:52:39 1995
// Description..: Main minimal A MODIFIER pour une appli utilsant RAL
//		  Remplacer 'montype' par ce qu'il faut.
//
// Ident........: main.C,v 1.2 1998/03/20 06:20:06 marc Exp
// ----------------------------------------------------------------------


#include <stdlib.h>
#include <strings.h>
//#include <stream.h>
#include <sstream>
using namespace std;

#include "popu.h"
#include "appli.h"
#include "reel.h"


int main( int argc, char *argv[])
{
   AppliStart( argc, argv );

   // un bitstring bidon: clone ds popu::init_param apres 
   // lecture des parametres et choix du type ds bitstring::init_param
   chromosome* chromotype = new reel((int)0); 
   popu* la_popu = new popu(chromotype);

   la_popu->init_param(argc, argv);
   AppliStatus( argc, argv );

   int ires;
   while (ires = la_popu->generation())
      ;
   chromotype = la_popu->rend_meilleur();
   cout << "popu ++  " << la_popu->Gen() <<  chromotype->NbEval() << 
					 chromotype->perf();
   chromotype->Affiche();
   delete la_popu;

   return 0;
}




