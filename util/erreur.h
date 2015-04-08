/* ----------------------------------------------------------------------
 * Where........: CMAP - Polytechnique 
 * File.........: erreur.h
 * Author.......: Marc Schoenauer
 * Created......: Tue Mar 21 13:12:29 1995
 * Description..: Gestion des erreurs
 * 
 * Ident........: $Id: erreur.h,v 1.2 1995/04/19 13:43:54 lamy Exp $
 * ----------------------------------------------------------------------
 */

#ifndef H_ERREUR
#define H_ERREUR

#ifdef __cplusplus
extern "C" {
#endif

void grave_erreur(int, char *);
void legere_erreur(int, char *);

#ifdef __cplusplus
}
#endif


/* Liste des codes d'erreur.
   pour chaque appli, commence en 10 */

#define MaxLgErreur 128

#define Err_Ouv_Fich 0
#define Err_Fin_Fic 1
#define Err_alloc_memoire 3
#define Err_Fatale 4
#define Err_Vide 9
#define Err_nbindiv_trop_grand 10
#define Err_Chaine 11
#define Err_Bornes 12

#endif
