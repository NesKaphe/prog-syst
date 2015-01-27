#ifndef OPERATIONS_BINAIRE
#define OPERATIONS_BINAIRE

#include <math.h>
#include <stdlib.h>

/*============================================================================
int binaire_to_decimal(char *buf, int nbOctets)
==============================================================================
Prend en paramettre un "buf" qui contien un nombre sur "nbOctets"
Retourne ce chiffre sur un int.
============================================================================*/
int binaire_to_decimal(char *buf, int nbOctets);



/*============================================================================
int longueur_bin (int decimal_length, unsigned char* bin)
==============================================================================
Pour recupérer la longeur "decimal_length" et la tranformer en un unsigned char
de 3 ocet "bin"
Retourne 0
============================================================================*/
int longueur_bin (int decimal_length, unsigned char* bin);



/*============================================================================
int date_bin (int decimal_time, unsigned char* bin)
==============================================================================
Pour recupérer la longeur "decimal_time" et la tranformer en un unsigned char
de 4 ocet "bin"
retourne 0
============================================================================*/
int date_bin (int decimal_time, unsigned char* bin);

#endif
