#include "lecture.h"

/*============================================================================
int ouverture_dazibao(dazibao* dazi)
==============================================================================
demande le fichier que l'utilisateur veux ouvrir et regarde si il est vadlide
retourn -1 en en cas d'erreur
============================================================================*/
int ouverture_dazibao(dazibao* dazi){
	char chemin[T_SAISIE];
	saisie_utilisateur(chemin);
	int dv = dazibaoValide(chemin);

	if(dv == -1) return -1;//le dazibao n'existe pas ou est invalide
	strcpy(dazi->chemin,chemin);//écrire le chemin dans la structure dazibao	
	return 1;
}


/*============================================================================
int affichage_simple(dazibao* dazi)
==============================================================================
//Pour générer un affichage simple du dazibao.
//Retourne -1 en cas d'erreur.
============================================================================*/
int affichage_simple(dazibao* dazi){
	printf("\nAffichage =========================\n");
	lireDazibao(*dazi);//remplir notre dazibao
	int retour = listeTypesDazibao(*dazi);//afficher notre dazibao
	printf("===================================\n");
	return retour;
}
