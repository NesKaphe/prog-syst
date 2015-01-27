#ifndef TLV_H
#define TLV_H

#include <sys/types.h> //Pour avoir acces au type off_t de la position retournée par lseek


/*==============================================================================================
struct tlv_t
================================================================================================
Représente un tlv :
  - "type" pour le type lu dans le fichier

  - "longueur" pour la longueur lu dans le fichier

  - "value" contiendra uniquement la position du debut du champ Value dans le fichier (lseek)

  - "nb_sous_tlv" est utilisé pour les tlv "conteneurs" comme par exemple les compound mais vaut
    toujours 0 pour les types de "base" qui ne contiennent aucun sous_tlv
================================================================================================*/
struct tlv_t {
    int type;
    int longueur;
    off_t value;
    int nb_sous_tlv;
};

typedef struct tlv_t tlv;

#define NB_TLV 4096 //Nombre de tlv maximum du tableau de tlv

/* DEFINITION DES TYPES DE TLV CONNUS */
#define PAD1 0
#define PADN 1
#define TEXT 2
#define PNG 3
#define JPEG 4
#define COMPOUND 5
#define DATED 6
/* FIN */



/*==============================================================================================
int lireTLV(int dazibaoFD, tlv *tab_tlv, int position)
================================================================================================
Lis le prochain TLV dans le descripteur de fichier du dazibao puis remplis la
structure tlv à l'indice "position" du tableau de tlv "tab_tlv" selon la lecture du fichier

Retourne l'indice de la prochaine case vide du tableau "tab_tlv" si un tlv a été lu
Retourne 0 si end of file a été attein
Retourne -1 en cas d'erreur de lecture
================================================================================================*/
int lireTLV(int dazibaoFD, tlv *tab_tlv, int position);




/*==============================================================================================
affiche_type_tlv(tlv *tab_tlv, int numero, int niveau, int dazibaoFD)
================================================================================================
Affiche le type du premier tlv du tableau et si, un type compound ou dated est rencontré, 
la fonction se rappelle sur le prochain tableau (tab_tlv + 1)

le numero sert pour l'affichage du numero du tlv que l'on va afficher

le niveau sert à construire l' affichage type arborescence

Retourne le nombre de TLV affiché en cas de reussite
Retourne -1 si une erreur a été rencontrée
================================================================================================*/
int affiche_type_tlv(tlv *tab_tlv, int numero, int niveau, int dazibaoFD);




/*==============================================================================================
supprimerTLV(int dazibaoFD, tlv *tab_tlv)
================================================================================================
Supprime le TLV à la premiere adresse du tableau en le remplaçant par un padn. 

Si un type compound ou dated est rencontré, on demande a l'utilisateur s'il veut supprimer 
l'intégralité ou bien uniquement un tlv contenu à l'interieur puis on se rappelle recursivement 
en cas de suppression d'un tlv "contenu" (tab_tlv + choix_utilisateur)

Retourne 1 si le TLV a été supprimé
Retourne 0 si aucun TLV n'a été supprimé
Retourne -1 si une erreur est survenu durant la suppression
================================================================================================*/
int supprimerTLV(int dazibaoFD, tlv *tab_tlv);


#endif
