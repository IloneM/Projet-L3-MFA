// ----------------------------------------------------------------------
// Where........: CMAP - Polytechnique 
// File.........: main.C
// Author.......: Bertrand Lamy (Equipe genetique)
// Created......: Wed Apr 19 15:52:39 1995
// Description..: Main minimal A MODIFIER pour une appli utilsant RAL
//		  Remplacer 'montype' par ce qu'il faut.
//
// Ident........: $Id: samplemain.C,v 1.2 1996/05/30 17:26:01 marc Exp $
// ----------------------------------------------------------------------


#include <stdlib.h>
#include <strings.h>
#include <stream.h>

#include "popu.h"
#include "appli.h"
// A changer pour mettre le bon header
#include "montype.h"


int main( int argc, char *argv[])
{
   AppliStart( argc, argv );

   chromosome* chromotype = new montype; // Remplacer montype par le type
					// definit pour l'appli
   popu* la_popu = new popu(chromotype);
   
   la_popu->init_param(argc, argv);
   AppliStatus( argc, argv );

   int ires;
   while (ires = la_popu->generation())
      ;
   chromotype = la_popu->rend_meilleur();
   cout << "Meilleur de tous temps : \n" << chromotype->perf() << endl;
   chromotype->printout(cout);
   cout << "\n\n";

   delete la_popu;

   return 0;
}



