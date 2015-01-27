#ifndef INTERFACE_H
#define INTERFACE_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>


#include "couleur.h"
#include "lecture.h"
#include "ecriture.h"
#include "saisie_utilisateur.h"
#include "dazibao.h"
#include "client.h"


/*============================================================================
void intro()
==============================================================================
message d'introduction
============================================================================*/
void intro();



/*============================================================================
void fin()
==============================================================================
message de fin de programme
============================================================================*/
void fin();


/*============================================================================
int menu_ouverture_creation(dazibao* dazi)
==============================================================================
Menu qui demande à l'utilisateur si il veux creer ou ouvrir un dazibao et 
remplit la structure dazibao en paramêtre.
option 1 : terminer programmme
option 2 : creation d'un dazibao
option 3 : ouverture d'un dazibao
option 4 : connexion au serveur de notification

Retourne 0 en cas de réponse non valide.
Cette fonction retourne le numéro de l'action demandé par l'utilisateur.
En cas d'échec c'est le même numéros mes négatif qui est retourné.
============================================================================*/
int menu_ouverture_creation(dazibao* dazi);




/*============================================================================
int menu_option(dazibao* dazi)
==============================================================================
Menu option qui prend une structure dazibao en paramètre et intérragie avec
l'utilisateur.
option 1 : terminer programme
option 2 : ajouter un tlv au dazibao
option 3 : supprimer un tlv du dazibao
option 4 : afficher le contenu du dazibao
option 5 : faire appelle au menu ouverture et création
option 6 : faire une compaction
option 7 : faire l'extraction d'un tlv
option 8 : suivre un dazibao particulier


Retourne 0 en cas de réponse non valide.
Cette fonction retourne le numéro de l'action demandé par l'utilisateur.
En cas d'échec c'est le même numéros mes négatif qui est retourné.
============================================================================*/
int menu_option(dazibao* dazi);





/*============================================================================
affiche_menu_oc(dazibao *dazi)
==============================================================================
Affiche et gère les erreurs du menu affiché par menu_ouverture_creation
============================================================================*/
int affiche_menu_oc(dazibao *dazi);




/*============================================================================
affiche_menu_option(dazibao *dazi)
==============================================================================
Affiche et gère les erreurs du menu affiché par menu_option
============================================================================*/
int affiche_menu_option(dazibao *dazi);


#endif
