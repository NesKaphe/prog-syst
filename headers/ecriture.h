#ifndef ECRITURE_H
#define ECRITURE_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/file.h>

#include "interface.h"
#include "insertion.h"
#include "saisie_utilisateur.h"
#include "dazibao.h"



/*============================================================================
int creation_dazibao(dazibao* dazi)
==============================================================================
Interagie avec l'utilisateur pour créer un dazibao.
Crée un dazibao avec une entête. Et on remplit le champ chemin de la 
structure dazibao en paramètre.

Si le  fichier dazibao existe déjà on demande à l'utilisateur si il veux 
écraser le dazibao existant.

Retourne -1 si il y a une erreur.
Retourne -3 si l'utilisateur n'a pas voulut écraser le dazibao existant.
============================================================================*/
int creation_dazibao(dazibao* dazi);





/*============================================================================
int ajout(dazibao* dazi)
==============================================================================
Intéragie avec l'utilisateur pour ajouter des tlv dans la structure dazibao 
passé en paramètre.

Retourne la taille en octet écrite dans le fichier dazibao.
Retourne -1 ou une valeur négative en cas d'erreur.
Problème : en cas d'écriture incomplète le fichier est corrompus.
==============================================================================*/
int ajout(dazibao* dazi);





/*============================================================================
int supprimer(dazibao dazi)
==============================================================================
Interagie avec l'utilisateur pour supprimer un tlv dans un fichier dazibao.
Cette fonction remplace le tlv existant par un padN.

Retourne -1 en cas d'erreur.
==============================================================================*/
int supprimer(dazibao dazi);





/*============================================================================
int compaction(dazibao dazi,int verbos)
==============================================================================
La compaction supprime tout les padN et pad1                      !!!!!!!!!!!!à vérifier pour les pad 1 !!!!!!
en réecrivant le fichier dazibao. "verbos" permet ou non d'afficher avec des
message le résultat de la compaction.

Retourne 1 si la compaction à réduit le fichier.
Retourne 0 si ça n'a pas trouvé de tlv padN ou pad1 et donc n'a rien changé.
Retourne -1 en cas d'érreur.
============================================================================*/
int compaction(dazibao dazi,int verbos);




/*============================================================================
int calcul_longueur(int indice, dazibao dazi)         (utilisé par compaction)
==============================================================================
Fonction récursive qui permet de récupérer la taille exacte d'un tlv (en octet)
avec son entête.

Precision de "taille exacte" :
En effet, si nous avons un tlv composé (compound ou dated) nous voulons avoir 
la nouvelle taille après compaction, c'est à dire sans les padN et pad1.
Par exemple si un tvl compound contien un padN nous obtiendrons la taille de 
ce compound sans le padN.

Retourne la longueur d'un tlv à partir de son indice dans la structure dazibao

Il n'y a pas de retour d'erreur l'utilisateur de la fonction doit passer des 
arguments qui sont bon.
==============================================================================*/
int calcul_longueur(int indice, dazibao dazi);




/*============================================================================
int tlv_suivant(dazibao dazi, int indice)  
============================================================================
Permet de calculer l'indice dans le tableau de tlv du tlv suivant qui est au 
même niveau que l'indice passé en paramètre.

Voici un exemple d'affichage pour éclairé ce propos abscon :

 ...
 (5) Compound 			indice = 5
     | (1) Message texte	indice = 6
     | (2) Message texte	indice = 7
 (6) texte			indice = 8

Dans l'exemple présenté le tlv (5) et le (6) sont de même niveau. Mais leurs
indices dans le tableau de tlv doivent être calulés.

Il n'y a pas de retour d'erreur l'utilisateur de la fonction doit passer des 
arguments qui sont bon.
============================================================================*/
int tlv_suivant(dazibao dazi, int indice);




/*============================================================================
int extraction_tlv(dazibao dazi,int indice, char* path_dest,char* path_name)
============================================================================
Fonction qui intéragie avec l'utilisateur.
Extrait un tlv dans le dazibao "dazi" à l'indice "indice" du tableau de tlv 
et l'enregistre dans le path "path_dest" avec le nom "path_name".
Retour -1 si il y a une erreur.
============================================================================*/
int extraction_tlv(dazibao dazi,int indice, char* path_dest,char* path_name);




/*============================================================================
int selection_indice(dazibao dazi)		(utiliser par extraction)
============================================================================
Interraction avec l'utilisateur pour savoir ce qu'il veux extraire.
Retourne l'indice exacte dans le tlv ou se trouve l'élément selectionné 
par l'utilisateur.
(cette fonction pourrait être utilisé par supprimé mais à été codé plus tard)
============================================================================*/
int selection_indice(dazibao dazi);



//============================================================================
//int affiche_selection(dazibao dazi)
//============================================================================
//A RETIRER
//============================================================================
int affiche_selection(dazibao dazi);


#endif

