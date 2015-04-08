// -*-C++-*------------------------------------------------
// Where.........: CMAP - Polytechnique
// File..........: pile.h
// Author........: Francis SOURD
// Created.......: Apr 24 1996
// Description...: Gestion d'une pile d'entiers
// --------------------------------------------------------

//#include <stream.h>
#include <stdlib.h>
#include <iostream>
using namespace std;

////////////////////////////////////////////////
struct element
////////////////////////////////////////////////
{
  int contenu;
  struct element* suivant;
public:
  element(int i,element* s);
  //  ~element();
};

////////////////////////////////////////////////
struct elem_queue
////////////////////////////////////////////////
{
  int contenu;
  struct elem_queue* suivant;
  struct elem_queue* precedent;
public:
  elem_queue(int i,elem_queue* s);
  //~elem_queue();
};


////////////////////////////////////////////////
class pile
////////////////////////////////////////////////
{
  element* la_pile;
public:
  pile() {                  // constructeur
    la_pile = 0;
  }
  
  ~pile() {
    if (la_pile!=0) {
      delete[] la_pile;
    }
  }

  void put(int i) {
    element* cellule=new element(i,la_pile);
    la_pile=cellule;
  }

  int get() {
    if (la_pile==0) {
      cout << "La pile est vide\n";
      exit(-1);
    }
    int res=la_pile->contenu;
    element* old=la_pile;
    la_pile=la_pile->suivant;
    old->suivant=0;
    delete[] old;
    return res;
  }

  int vide() {
    return la_pile==0;
  }
};

////////////////////////////////////////////////
class queue
////////////////////////////////////////////////
{
  elem_queue* la_queue;
  elem_queue* tete;
public:  
  queue() {                  // constructeur
    la_queue = 0;
    tete=0;
  }
  
  ~queue() {
    if (la_queue!=0) {
      // delete la_queue;
    }
  }

  void put(int i) {
    elem_queue* cellule=new elem_queue(i,la_queue);
    if (la_queue!=0)
      la_queue->precedent=cellule;
    la_queue=cellule;
    if (tete==0) tete=la_queue;
  }

  int get() {
    if (tete==0) {
      cout << "La queue est vide\n";
      exit(-1);
    }
    int res=tete->contenu;
    elem_queue* old=tete;
    tete=tete->precedent;
    if (tete!=0) 
      tete->suivant=0;
    delete old;
    return res;
  }

  int vide() {
    return tete==0;
  }
};

