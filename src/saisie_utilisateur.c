#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "saisie_utilisateur.h"



/*============================================================================
void saisie_utilisateur(char* s)
==============================================================================
lit les entrées claviers faites par l'utilisateur et remplit "s" avec ce qui a 
été ecrit. Attention "s" doit être un tableau de taille T_SAISIE (256).
============================================================================*/
void saisie_utilisateur(char* s) {

	memset(s,0,T_SAISIE);//effacer le contenu de s
	int len;
	char c;

	do{
		fgets(s, T_SAISIE, stdin); // lecture d'une ligne venant de stdin

		len = strlen(s);
	}while(len<=1);

	if (len > 0 && s[len-1] == '\n' )  // si nou avons une nouvelle ligne
		s[len-1] = '\0';          // on tronque la ligne
	else {
		while((c=fgetc(stdin)) != '\n' && c != EOF);
		memset(s,0,T_SAISIE);
	}
	
}


/*============================================================================
int saisie_chiffre()
==============================================================================
Demande à l'utilisateur de saisir un chiffre.
Cette fonction retourne que des chiffres positifs.
Retourne  -1 si il y a une abération. exemple : si l'utilisateur entre "abc"
============================================================================*/
int saisie_chiffre(){

	char rep[T_SAISIE];
	saisie_utilisateur(rep);
	
	int i = 0;
	//notre filtre :
	for(i=0;i<T_SAISIE;i++){
		if((rep[i]=='\0')||(rep[i]=='\n'))//si nous avons atteint le bout de la chaine on sort de la boucle le chiffre est valide
			break;
		//si le ce n'est pas un chiffre c'est une erreur :
		if((rep[i]<'0')||(rep[i]>'9')){
			return -1;
		}
	}
	return atoi(rep);
}

