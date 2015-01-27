#ifndef DATE_H
#define DATE_H

#include <stdio.h>
#include <time.h>
#include <string.h>


/*============================================================================
char *dated(int sec, char *result)
==============================================================================

Retourne la chaine de caract�res result de la forme "JJ/MM/AAAA � HH:MM:SS" 
qui est la date correspondante � l'heure Unix en nombre de secondes donn�e par 
sec.
Retourne NULL en cas d'erreur.
============================================================================*/
char *dated(int sec, char *result);

#endif
