#ifndef LECTURE_H
#define LECTURE_H

#include "dazibao.h"
#include "tlv.h"
#include "saisie_utilisateur.h"
#include <string.h>
#include <unistd.h>
#include <stdio.h>


/*============================================================================
int ouverture_dazibao(dazibao* dazi)
==============================================================================
demande le fichier que l'utilisateur veux ouvrir et regarde si il est vadlide
retourn -1 en en cas d'erreur
============================================================================*/
int ouverture_dazibao(dazibao* dazi);


/*============================================================================
int affichage_simple(dazibao* dazi)
==============================================================================
//Pour générer un affichage simple du dazibao.
//Retourne -1 en cas d'erreur.
============================================================================*/
int affichage_simple(dazibao* dazi);

#endif
