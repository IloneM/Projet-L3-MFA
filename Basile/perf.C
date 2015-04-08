
#include <math.h>
#include <string.h>
#include <popu.h>
#include "myrand.h"
#include "reel.h"
#include <float.h>

#define C_label 1
// The class to predict
#define C_relevant 2
// Relevant feature
#define C_irrelevant -1
// To be omitted
#define C_identifier 3
// If we need to check examples.

#define C_length 1000
double ww[C_length];
int segment[300]; // length of MSS
int nb_segment; // number of MSS
#define C_gap 3.

int C_OK = -1;
int C_malade = 1;
int total_malade;
int total_OK;
double Xoffset; // operationnel iff poids bornes et possiblement negatifs. 
int Single; // class to be predicted against the others.
int _User; // verbose or not.

typedef double (*fperf)(int, double*);
fperf quelle_perf, Challenge;       // the fitness
typedef double (*fcompute)(double*,int);
fcompute compute;       // the fitness

typedef double (*fcomp)();
fcomp quel_comp;

char *Nom_DATA;
int App_Tst;

char Ligne[100];
void Read_Chall(char *, int, int);

// DONNEES DE L'APPLI
double* data; // les exemples
int* label; // label de chaque exemple, 1, 3, 5
//double *memoire; // boosting like
long *ex_id; // boosting like
#define C_char 10
int nb_ex; // number of examples; by default, in training
int nb_ex_test; // number of examples; in test; both are read in .form
int* ordre_index; // rank examples
double  *pile; // value of each example

int nb_att; // number of features
int nb_true_att; // number of relevant features
int att_log; // take the logarithm of the features;
int* relevant; // some features are relevant.

double pile_min, pile_max;
int reel::Linear;
int reel::Multi; // nombre de centres
int gMulti; // la globale

// for further regularization
double Seuil_SRM; // reward of small coefficients
double reel::SRM;

int Read_label(int kp,char *Lig)
{
  label[kp] = atoi(Lig);
  if (_User > 1)
    printf(" example %ld ( %d ) \t Class  %d \n",ex_id[kp], kp, label[kp]);
  if (label[kp] == Single) {
    label[kp] = C_malade; total_malade++;}
  else {
    label[kp] = C_OK; total_OK++; }
  return kp + 1;
}

int Read_Format()
{
  nb_true_att = 0;
  sprintf(Ligne,"%s.form",Nom_DATA);
  FICH_IN *FIC = Ouvre_FICH_IN(Ligne);
  nb_ex = Lit_Entier(FIC);
  nb_ex_test = Lit_Entier(FIC);
  nb_att = Lit_Entier(FIC); 
  relevant = new int[nb_att];
  for (int i=0; i< nb_att; i++) {
    Lit_Mot(FIC, Ligne); // name of the attribute
    relevant[i] = Lit_Entier(FIC);
    switch (relevant[i]) { 
	case C_relevant: nb_true_att++; break;
	case C_identifier: break;
    }
    printf("Att %s (%d) Statut %i \n",Ligne,i,relevant[i]);
  }
  Ferme_FICH_IN(FIC);
//  nb_true_att = nb_true_att + 2; // we add att(i,2) - att(i-1,2); att(i,4) - att(i-1,4)
  return nb_true_att;
}

void Free_Chall() 
{
    free (data); free (label); free(ordre_index); free(pile); free(ex_id);
} 

void Alloc_Chall()
{
  data = new double[nb_ex * nb_true_att];
  if (data == NULL) {
    cout << " Lack memory " << endl;
    exit(1); }
  label = new int[nb_ex];
  //predit = new int[nb_ex];
  ordre_index = new int[nb_ex];
  pile = new double[nb_ex];
  //memoire = new double[nb_ex];
  ex_id = new long[nb_ex];
}

double Read_att(char *Lig)
{
  double u;
//	double u = Lit_Reel(FIC);
  u = atof(Lig);
  if (att_log) {  
    if (u >= 0) 
	return log(u + 1e-7); // adding 1: no way for Nokia; 
    printf(" Should not be log !\n");
    exit(1);
  }
  return u;
}

void Read_Chall(int app_tst)
{
  App_Tst = app_tst;
  FICH_IN *FIC;
  if (app_tst)
  {
    sprintf(Ligne,"%s.learn",Nom_DATA);
  }
  else
  {
    nb_ex = nb_ex_test;
    sprintf(Ligne,"%s.test",Nom_DATA);
  }
  //  cout << Ligne << " ICI LIGNE " << endl;
  FIC = Ouvre_FICH_IN(Ligne);
/*  nb_ex = Lit_Entier(FIC);
  int ii = Lit_Entier(FIC); 
  if (ii != nb_att) {
    printf("Number of features incompatible %s.learn, .test and .format \n", Nom_DATA);
      exit(1);
  }*/
  cout << Nom_DATA << " nb  att " << nb_att << " nb_ex " << nb_ex << endl;
  if (data != NULL) Free_Chall();
  Alloc_Chall();
  int k = 0;
  int kp = 0;
  total_malade = total_OK = 0;
//  int total_1 = 0, total_moins1 = 0;
  for (int i = 0; i < nb_ex; i++) { 
    for (int j = 0; j < nb_att; j++) {
      Lit_Mot(FIC,Ligne); 
      if (_User > 2)
	printf ("j (%d), relevan %d %s \n",j,relevant[j],Ligne);
      switch(relevant[j]) {
	case C_label: 
	  Read_label(i,Ligne); break;
	case C_relevant: 
	  data[k++] = Read_att(Ligne); break;
	case C_irrelevant: 
	 break;
	case C_identifier:   
	  ex_id[i]=atol(Ligne); break;
      }
    }
//       if (i > 0) { 
// 	data[k++] = data[i*nb_true_att + 1] - data[(i-1)*nb_true_att + 1]; // attribute 2
// 	data[k++] = data[i*nb_true_att + 3] - data[(i-1)*nb_true_att + 3]; // attribute 4 
//       }
//       else {
//  	data[k++] = 0; // attribute 2
// 	data[k++] = 0; // attribute 4 
//       }
    }
    nb_segment = 0;
  for (int i = 1; i < nb_ex; i++)
  {
    double diff = 0;
    for (int j = 0; j < nb_true_att - 2; j++) {
      diff += fabs(data[i*nb_true_att+j] - data[(i-1)*nb_true_att+j]);}
    if (diff > C_gap) { 
      segment[nb_segment++] = i;
      printf("%d %g %d\n",i,diff,nb_segment);
    }
      
  }

   cout << Nom_DATA << " nb  att " << nb_att << " nb_ex " << nb_ex << " nb segment " << nb_segment << endl;
   Ferme_FICH_IN(FIC);
}




void reel::affiche_SV()
{
  // New version, 8/6/2005 : no normalization inside. 
  FILE * fich = fopen ("Roger.weights", "w");
  // Old version: only writes the weight of relevant features
/*  for (int i = 0; i < nb_true_att; i++) {
    fprintf( fich, " %d %f \n", i, les_vals[i]);
  }*/
  int j=0;
  for (int i=0; i < nb_att * reel::Multi; i++) {
    if (relevant[i] == C_relevant)
     fprintf( fich, " %d %f \n", i, les_vals[j++]);
    else
      fprintf( fich, " %d 0 \n", i);
  }
  fclose (fich);
  if (!Linear) {
    FILE * fich = fopen ("Roger.centers", "w");
    j = 0;
    for (int i = 0; i < nb_true_att * reel::Multi; i++) {
      if (relevant[i] == C_relevant)
	fprintf( fich, " %d %f \n", i, les_vals[nb_true_att + j++]);
      else
	fprintf( fich, " %d 0 \n", i);
    }
    fclose (fich);
  }
} 

int  comp_pile(const void* v1, const void* v2)
{ // 17 sept, ordre decroissant
  int i1 = *(int *)v1;
  int i2 = *(int *)v2;
  if (pile[i1] < pile[i2]) return 1;
  if (pile[i1] > pile[i2]) return -1;
  return 0;
}

//----------------------------------------------

void reel::affiche_pile()
{
  for (int i=0; i < nb_ex; i++)
    ordre_index[i] = i;
  qsort(ordre_index, nb_ex, sizeof(int), comp_pile);

  pile_min = pile[ordre_index[0]];
  pile_max = (pile[ordre_index[nb_ex-1]]);
  cout << " min " << pile_min << " max " << pile_max << endl;
  FILE * fich_ordre = fopen("resul.ordo","w");
  FILE * fich_roc = fopen ("resul.roc", "w");
  int echec_left = 0;
  int echec_right = 0;
  // cout << " ECHEC ";
  double x=0, y = 0;
  double aire = 0;
  int barre_haut = (total_OK + total_malade) / 2; 
  fprintf(fich_roc, " 0 0  \n");
   for (int i=0; i < nb_ex; ) {
    for (int k=0; k < 100; k++){
      aire += ((100 - k) * pile[i++]);
      k++;
      fprintf(fich_roc, " %d %5f \n",i-1, pile[i-1]);
    }
  }
  cout << "\\ ******* aire " << App_Tst << " " << aire  << endl;
  fclose(fich_roc);
  fclose(fich_ordre);
} // End of reel::affiche_pile


int reel::strategie_avant(popu* pop)
//--------------------------------
{

  double taux;
  switch(strategie) {
  case UN_CINQ:
    if (force_mut == 1)	// pas de mecanisme d'adaptation
      return 1;
    if (!nb_mut_total) {	// il n'y a pas eu de mutations
      nb_mut_reussies=0;	// on ne sait jamais
      return 1;
    }
    taux = (double) nb_mut_reussies / nb_mut_total;
    if (taux < seuil)    	// pas assez de mutation reussies -> diminuer sigma_global
      sigma_global /= force_mut;
    else
      sigma_global *= force_mut;

    if (User > 1)
      cout << "Mutations " << nb_mut_total << " (" << nb_mut_reussies << ") -> " <<   sigma_global << endl;
    nb_mut_reussies = nb_mut_total = 0; // pour la generation suivante
    break;
  }
  //  percentage /= 1.0001;
  //  cout << " " << (int) (percentage * nb_ex) << endl;
//   for (int i=0 ; i < pop->GetSize(); i++) {
//     (pop->GetChromo(i))->raz_perf();
//     (pop->GetChromo(i))->perf();
//   }
/*  // remise a zero de la memoire
    for (int i = 0; i < nb_ex; i++)
      memoire [i] = 1.0;*/
  return 1;
}


double compar_roc () // on compare label (la targette) et pile :
{
  for (int i=0; i < nb_ex; i++)
    ordre_index[i] = i;
  qsort(ordre_index, nb_ex, sizeof(int), comp_pile);
  // remplir le tableau des prediction :
  int y = 0;
  double aire_ROC = 0;
  for (int i=0; i < nb_ex; i++)
    {
      if (label[ordre_index[i]] == C_malade) 
	y++; 
      else if (label[ordre_index[i]] == C_OK) 
	aire_ROC += y;
    }
  aire_ROC /= double (total_malade * total_OK);
  if (aire_ROC < 0) 
    printf(" Curieux \n\n"); 
  return (aire_ROC);
}
double compar_Basile () // on compare label (la targette) et pile :
{
  double aire_ROC = 0;
  double toto = 0;
  int iex = 0;
  for (int i=0; i < nb_segment; i++) {
    int k = 0;
    while(iex < segment[i]) { 
      aire_ROC += ww[k++] * pile[iex++];
      if (k >= C_length) { printf("exit long sequence k %d \n",k); exit(1);}
    }
  }
    
//  aire_ROC /= toto;
/*  if (aire_ROC < 0) 
    printf(" Curieux \n\n"); 
 */ 
  return (aire_ROC);
}

double compar_pas_roc () // on compare label (la targette) et pile :
{
  cout << " passe par la " << endl;
  int i;
  for (i=0; i < nb_ex; i++)
    ordre_index[i] = i;
  qsort(ordre_index, nb_ex, sizeof(int), comp_pile);
  // remplir le tableau des prediction :
  int score_ill = 0;
  i=0;
  while (label[ordre_index[i++]] == C_malade)
    score_ill++;
  int score_healthy = 0;
  i = nb_ex - 1;
  while (label[ordre_index[i--]] != C_malade)
    score_healthy++;
  return score_ill + score_healthy;
}

double reel::get_exemple_tourmente()
{
  int igagnant = rnd(nb_ex);
  return (igagnant + .0001) / nb_ex;
}

double L1(int nval, double *val)
{
  double w=0;
  for (int i=0; i < nval; i++)
    w += fabs(val[i]);
  //  printf("L1 %g\n",w);
  return -w;
}
double L2(int nval, double *val)
{
  double w=0;
  for (int i=0; i < nval; i++)
    w += val[i]*val[i];
  //  printf("L2 %g\n",w);
  return -w;
}

// compute the score of an example, in linear and non-linear mode

double compute_L(double *val, int iex)
{
  double score_loc, score_loc_loc;
  score_loc = 10E7;
  for (int k=0; k < gMulti; k++) {
    score_loc_loc = 0;
    double _L1 = 0;
    for (int j = 0; j < nb_true_att ; j++) { 
    score_loc_loc += data[(iex) * nb_true_att + j]  * val[j + nb_true_att * k];
    _L1 += fabs(val[j + nb_true_att * k]);}
    score_loc_loc /= _L1;
    if (score_loc_loc < score_loc) score_loc  = score_loc_loc;
  }
  return score_loc;
}

double compute_NL(double *val, int iex)
{
  double score_loc, score_loc_loc;
  score_loc = 10E7;
  for (int k=0; k < gMulti; k++) {
    score_loc_loc = 0;
    for (int j = 0; j < nb_true_att ; j++)
      score_loc_loc+= fabs(data[(iex) * nb_true_att + j ] - val[j+ nb_true_att + k * 2 * nb_true_att]) * val[j + k * 2 * nb_true_att];
     if (score_loc_loc < score_loc) score_loc  = score_loc_loc;
  }
  return score_loc;
}


double	Augure(int nval, double *val)
{ 
  double perf = 0;
  double score_loc = 0.0;
  for (int i_ex = 0; i_ex < nb_ex; i_ex++) // un patient
    pile[i_ex] = compute(val,i_ex);
  return compar_Basile();
  return compar_roc();
  return compar_roc() + L2(nval,val);
}

void reel::printout(ostream& fout)
///////////////////////////////////////////////////
{
  int i;
  fout << perf() << " " ;
  if (Linear)
    for (i=0; i<nb_true_att ; i++)
      fout << les_vals[i] << "  " ;
  else
    for (i=0; i<nb_true_att ; i++)
      fout << les_vals[i] << " (" << les_vals[i+nb_att] << ") " ;

  fout << "\n************************\n" ;
  switch(strategie) {
  case UN_CINQ :		// tt le monde mute avec sigma_global
    for (i=0; i<nval; i++)
      fout << sigma_global << " " ;
    break;			
  case UN_ADAPT :		// tt le monde mute avec sigma[0]
    for (i=0; i<nval; i++)
      fout << sigma_loc[0] << " " ;
    break;			
  case ADAPT :			// les sigma[i]
    for (i=0; i<nval; i++)
      fout << sigma_loc[i] << " " ;
    break;			
  }
}

void reel::Effectue_Test()
{
  Challenge(nval,les_vals);
  //  for (int i_ex = 0; i_ex < nb_ex; i_ex++) // un patient
  //    printf("%d %g %d \n",i_ex,pile[i_ex],label[i_ex]);

  cout << " L1 norm " << L1(nval,les_vals) << endl;
  affiche_SV();
  affiche_pile();
  system("mv resul.roc resul.roc.train");
  system("mv resul.ordo resul.ordo.train");
  //  system("mv resul.pile resul.pile.train; mv resul.clas  resul.clas.train");

  // ET MAINTENANT LE TEST
  Read_Chall(0);
  Challenge(nval, les_vals);
  affiche_pile();
  system("mv resul.roc resul.roc.test");
  system("mv resul.ordo resul.ordo.test");
  //  system("mv resul.pile resul.pile.test; mv resul.clas  resul.clas.test");
}

void reel::Affiche()
  ///////////////////////////////////////////////////
{
  cout << "\n !!!!!!!!!!!!!!!!!!!!!! \n" ;
  Effectue_Test();
  return;
  switch(strategie) {
  case UN_CINQ :		// tt le monde mute avec sigma_global
    for (int i=0; i<nval; i++)
      cout << sigma_global << " " ;
    break;			
  case UN_ADAPT :		// tt le monde mute avec sigma[0]
    for (int i=0; i<nval; i++)
      cout << sigma_loc[0] << " " ;
    break;			
  case ADAPT :			// les sigma[i]
    for (int i=0; i<nval; i++)
      cout << les_vals[i] << " " ;
    //      cout << sigma_loc[i] << " " ;
    break;			
  }
}


// variable globale pour controler le % de high-scores pris en consideration
double pourcent_high_score = 0.001; // initialement 1% 


///////////////////////////////////////////////////////// 
double reel::calcule_tout()
///////////////////////////////////////////////////////// 
{

   return quelle_perf(nval, les_vals);
}

///////////////////////////////////////////////////////// 
void reel::maj_probleme(char* nom, int User, int nval)
///////////////////////////////////////////////////////// 
{ 
  int i;
  _User = User;
  Nom_DATA = (char*) * new  WordParametre("DATA", 
			  "Nom du fichier des data", "breast");
  reel::yadebornes = (int) * new IntParametre("BORNES", 
                                 "Variables bornees (1) ou non (0) [1]",1);
  reel::Modele_Log = (int) * new IntParametre("LOG", "Codage log", 0);
  reel::AvecNeg = (int) * new IntParametre("NEG", "Poids negatifs", 0);
  reel::Rock = (int) * new IntParametre("RUC", "Optimise Aire Roc", 1);
  Single = (int) * new IntParametre("Sing","one class against all", 1);
  reel::SRM = (double) * new DoubleParametre("SRM","Weight regularization L1", .1);
  Seuil_SRM = reel::SRM;
  cout << " ++++ ROC " << reel::Rock << " NEG " << reel::AvecNeg << " SRM " << Seuil_SRM << " Single " << Single << endl;
  Read_Format();
  reel::Linear = (int) * new IntParametre("NL","Function assumed linear (1) or not (0)", 1);
  reel::Multi = (int) * new IntParametre("Multi","Function min of k forms (1)", 1);

  reel::nval = (reel::Linear) ? nb_true_att * reel::Multi : 2 * nb_true_att * reel::Multi;
  compute = (reel::Linear) ? compute_L : compute_NL;
  gMulti= reel::Multi;

  att_log = Modele_Log;
  Read_Chall(1);
  ww[0] = 10E4;
  for (int i=1; i < C_length; i++)
    ww[i] = ww[i-1] * .999;

  if (!strcasecmp(nom, "ROC"))
    {
      quelle_perf = Augure;
      Challenge = &Augure;
      cout << "Fonction ROC, NEG " << AvecNeg << " Roc " << reel::Rock << " Log " << reel::Modele_Log << endl;
      if (reel::yadebornes)
	Xoffset = (reel::AvecNeg) ? .5 : 0.;
      else Xoffset = 0;
      //      printf ("***** ROC %d\n",reel::Rock);
      quel_comp = (reel::Rock) ? &compar_roc : &compar_pas_roc;
      return ;
    }
 cout << "Mauvais choix de fonction\n";
 cout << "Fonctions disponibles: ROC (Lineaire) \n";
 exit(1);
}
      
