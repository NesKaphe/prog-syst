#include "operations_binaire.h"

#include <stdio.h>

/*
//nouvelle version  (qui marche pas )
int binaire_to_decimal(char *buf, int nbOctets) {
	int resultat=0,i,puissance=1;
	for(i = nbOctets-1; i > 0 ; i--) {
		resultat+= (((unsigned char)buf[i])*puissance);
		puissance*=256;
	}
	return resultat;
}
*/



/*============================================================================
int binaire_to_decimal(char *buf, int nbOctets)
==============================================================================
Prend en paramettre un "buf" qui contien un nombre sur "nbOctets"
Retourne ce chiffre sur un int.
============================================================================*/
int binaire_to_decimal(char *buf, int nbOctets) {
    int i;
    int j;
    int bitActuel = (nbOctets * 8) - 1;
    int resultat = 0;
    unsigned char valeurBitActuel;

    for(i = 0; i < nbOctets; i++) {
	for(j = 7; j >= 0; j--) {
	    valeurBitActuel = (buf[i] & (1 << j)) > 0;
	    resultat += valeurBitActuel * pow(2,bitActuel);
	    bitActuel--;
	}
    }
    return resultat;
}



/*============================================================================
int longueur_bin (int decimal_length, unsigned char* bin)
==============================================================================
Pour recupérer la longeur "decimal_length" et la tranformer en un unsigned char
de 3 ocet "bin"
Retourne 0
============================================================================*/
int longueur_bin(int decimal_length, unsigned char* bin){

	bin[2] = (unsigned char) decimal_length;
	bin[1] = (unsigned char) (decimal_length >> 8);
	bin[0] = (unsigned char) (decimal_length >> 16);
	
	 
	return 0;

}


/*============================================================================
int date_bin (int decimal_time, unsigned char* bin)
==============================================================================
Pour recupérer la longeur "decimal_time" et la tranformer en un unsigned char
de 4 ocet "bin"
retourne 0
============================================================================*/
int date_bin(int decimal_time, unsigned char* bin){

	bin[3] = (unsigned char)   decimal_time;
	bin[2] = (unsigned char) (decimal_time >> 8);
	bin[1] = (unsigned char) (decimal_time >> 16);
	bin[0] = (unsigned char) (decimal_time >> 24);
	
	 
	return 0;

}

