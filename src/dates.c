#include "dates.h"

/*============================================================================
char *dated(int sec, char *result)
==============================================================================

Retourne la chaine de caractères result de la forme "JJ/MM/AAAA à HH:MM:SS" 
qui est la date correspondante à l'heure Unix en nombre de secondes donnée par 
sec.
Retourne NULL en cas d'erreur.
============================================================================*/

char *dated(int sec, char *result)
{

    time_t epoch = sec;
    
    struct tm *timeptr=localtime(&epoch);
    if (timeptr==NULL){	
       perror("localtime : <dated>") ;
	   return NULL;
    };
    
    
    if( (strftime(result,23,"%e/%m/%Y Ã  %T", timeptr) )==0){	
        perror("strftime : <dated>") ;
	    return NULL;
    };
    
    return result;

    
}
