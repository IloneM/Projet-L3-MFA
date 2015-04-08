#include "pile.h"

element::element(int i,element* s)
{
  contenu=i;
  suivant=s;
}

//element::~element()
//{
//  if (suivant!=0)
//    delete suivant;
//}

elem_queue::elem_queue(int i,elem_queue* s)
{
  contenu=i;
  suivant=s;
  precedent=0;
}

//elem_queue::~elem_queue()
//{
//  if (suivant!=0)
//    delete suivant;
//}
