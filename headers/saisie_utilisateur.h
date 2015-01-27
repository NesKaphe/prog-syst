#ifndef SAISIE_UTILISATEUR_H
#define SAISIE_UTILISATEUR_H

#include <stdio.h>
//#include <stdio_ext.h>
#include <string.h>

#define T_SAISIE 256 //taille d'une saisie


/*============================================================================
void saisie_utilisateur(char* s)
==============================================================================
lit les entrées claviers faites par l'utilisateur et remplit "s" avec ce qui a 
été ecrit. Attention "s" doit être un tableau de taille T_SAISIE (256).
============================================================================*/
void saisie_utilisateur(char* s);


/*============================================================================
int saisie_chiffre()
==============================================================================
Demande à l'utilisateur de saisir un chiffre.
Cette fonction retourne que des chiffres positifs.
Retourne  -1 si il y a une abération. exemple : si l'utilisateur entre "abc"
============================================================================*/
int saisie_chiffre();
#endif
