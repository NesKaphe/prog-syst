#ifndef DAZIBAO_H
#define DAZIBAO_H

#include "tlv.h"


/* ============================================================================================
struct notifications_t
===============================================================================================
Permet de regrouper tous les elements necessaires a la notification :
  - "sockFD" Un descripteur de socket qui sera relié au serveur de notifications

  - "nb_dazibao" qui contiendra le nombre de dazibao que l'utilisateur veut suivre

  - "connecte" afin de connaitre l'etat de connexion au serveur de notification

  - "dazibao_a_suivre" qui contiendra une liste des nom de dazibao que l'utilisateur va suivre
=============================================================================================== */
struct notifications_t {
    int sockFD;
    int nb_dazibao;
    int connecte;
    char **dazibao_a_suivre;
};

typedef struct notifications_t notifications;




/*==============================================================================================
struct dazibao_t (fait reference à struct tlv_t present dans le fichier tlv.h)
================================================================================================
Permet de regrouper tous les elements necessaires au fonctionnement du programme :
  - "notif" qui nous permet a tout moment d'avoir acces à l'état de connexion et la liste des
    dazibao à suivre

  - "chemin" contenant le nom du dazibao actuel demandé par l'utilisateur

  - "tab_tlv" un tableau contenant tous les tlv du dazibao nommé par "chemin"
================================================================================================*/
struct dazibao_t {
    notifications *notif;//On va stocker ici la liste des dazibao qu'on desire suivre
    char *chemin;
    tlv *tab_tlv;
};
typedef struct dazibao_t dazibao;


#define D_END (-1)




/*==============================================================================================
int ouvrirDazibao(char *chemin, int type)
================================================================================================
Permet d'ouvrir le dazibao demandé, nommé par "chemin" s'il est valide. 
Le type est le mode d'acces que l'on passera a la fonction open.

Retourne un descripteur de fichier si l'ouverture s'est bien déroulée.
Retourne si une erreur s'est produite
================================================================================================*/
int ouvrirDazibao(char *chemin, int type);




/*==============================================================================================
int dazibaoValide(char *chemin)
================================================================================================
Ouvre le dazibao, nommé par "chemin", et verifie si c'est un dazibao en lisant son entête

Retourne 1 si le fichier passé en paramètre est un dazibao valide
Retourne 0 si le fichier passé en paramètre n'est pas un dazibao valide
Retourne -1 si une erreur s'est produite
================================================================================================*/
int dazibaoValide(char *chemin);




/*==============================================================================================
int lireDazibao(dazibao un_dazibao) (fait appel à lireTLV du fichier tlv.h)
================================================================================================
Ouvre le dazibao fournis en paramètre s'il est valide, le lit et remplis son tableau de tlv
avec le contenu du dazibao.

Un verrou de type partagé sera utilisé pendant la durée de l'operation

Retourne 1 si la lecture s'est déroulée avec succes
Retourne -1 en cas d'erreur
================================================================================================*/
int lireDazibao(dazibao un_dazibao);




/*==============================================================================================
int listeTypesDazibao(dazibao un_dazibao) (fait appel à affiche_type_tlv du fichier tlv.h)
================================================================================================
Parcours le tableau de tlv, précédemment remplis par lireDazibao, du dazibao passé en paramètre 
et affiche leur type sur la sortie standard sous forme d'arborescence

Retourne 1 si l'affichage s'est bien déroulé
Retourne 0 si une erreur s'est produite
================================================================================================*/
int listeTypesDazibao(dazibao un_dazibao);




/*=======================================================================================================
supprimerEntree(dazibao unDazibao, int num_entree_supprimer) (fait appel à supprimerTLV du fichier tlv.h)
=========================================================================================================
Va trouver le tlv ayant le numero num_entree_supprimer dans le tableau de tlv du dazibao fournis en
paramètre afin faire appel à supprimerTLV pour réécrire un padn par dessus et effacer ainsi le TLV

Un verrou de type exclusif sera utilisé pendant la durée de l'operation

Retourne 1 si le tlv demandé à bien été supprimé
Retourne 0 si aucun tlv n'a été supprimé (le tlv demandé n'a pas été trouvé)
Retourne -1 en cas d'erreur
=========================================================================================================*/
int supprimerEntree(dazibao unDazibao, int num_entree_supprimer);

#endif
