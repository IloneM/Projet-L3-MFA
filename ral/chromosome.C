// ----------------------------------------------------------------------
// Where........: CMAP - Polytechnique 
// File.........: chromosome.C
// Author.......: Marc Schoenauer
// Created......: Tue Mar 21 12:50:41 1995
// Description..: Implementation de la classe chromosom
// 
// Ident........: $Id: chromosome.C,v 1.7 1996/07/24 15:59:54 medioni Exp $
// ----------------------------------------------------------------------

//#include <stream.h>
#include <stdlib.h>
#include <sstream>
using namespace std;
#include "chromosome.h"


IntParametre chromosome::User( "AFF_CHROM", "Niveau d'affichage de TOUS les chromosomes", 1 );

int chromosome::nb_eval =0;

//-------------------------------------------
chromosome::~chromosome()
//----------------------------------------------
{}

// _readin lit la perf, et passe la main a la lecture specifique
//-------------------------------------------
chromosome* chromosome::_readin(FICH_IN* fin)
//----------------------------------------------
{
  double rloc = Lit_Reel(fin);
  chromosome* ptloc = readin(fin);
   ptloc->performance = rloc;
   return ptloc;
}

////////////////////////
// utilitaire pour tris ds tableaux de chromo

//----------------------------------------------
int comp_perf_maximise(const void* v1, const void* v2)
//----------------------------------------------
{
   chromosome* c1 = *(chromosome **)v1;
   chromosome* c2 = *(chromosome **)v2;
   return c1->comp_inf(c2);
}

//----------------------------------------------
int chromosome::comp_inf(chromosome* otre)
//----------------------------------------------
{
   double pp=otre->perf();
   if (perf() > pp)
      return -1;
   else
      if (perf() < pp)
	 return +1;
      else
	 return 0;
}

chromosome* chromosome::init_param(popu* p=0)	// lecture des parametres specifiques
  // et eventuellement mise a jour de meilleur (APRES lecture des parametres)
  {
    return this;	// par defaut, les parametres sont OK
  }


//----------------------------------------------
int comp_perf_minimise(const void* v1, const void* v2)
//----------------------------------------------
{
   chromosome* c1 = *(chromosome **)v1;
   chromosome* c2 = *(chromosome **)v2;
   return c2->comp_inf(c1);
}
