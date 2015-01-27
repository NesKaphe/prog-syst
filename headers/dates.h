#ifndef DATE_H
#define DATE_H

#include <stdio.h>
#include <time.h>
#include <string.h>


/*============================================================================
char *dated(int sec, char *result)
==============================================================================

Retourne la chaine de caractères result de la forme "JJ/MM/AAAA à HH:MM:SS" 
qui est la date correspondante à l'heure Unix en nombre de secondes donnée par 
sec.
Retourne NULL en cas d'erreur.
============================================================================*/
char *dated(int sec, char *result);

#endif
