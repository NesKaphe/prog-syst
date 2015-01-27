#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "operations_binaire.h"
#include "tlv.h"
#include "dazibao.h"
#include "saisie_utilisateur.h"
#include "dates.h"



/*==============================================================================================
int lireTLV(int dazibaoFD, tlv *tab_tlv, int position)
================================================================================================
Lis le prochain TLV dans le descripteur de fichier du dazibao puis remplis la
structure tlv à l'indice "position" du tableau de tlv "tab_tlv" selon la lecture du fichier

Retourne l'indice de la prochaine case vide du tableau "tab_tlv" si un tlv a été lu
Retourne 0 si end of file a été attein
Retourne -1 en cas d'erreur de lecture
================================================================================================*/
int lireTLV(int dazibaoFD, tlv *tab_tlv, int position) {
    
    int octetsLu;
    int longueur = 0;
    int position_depart = position;
    int position_parcours = position;
    char buf[4];


    //On verifie que l'on est pas en train de depasser le tableau
    if(position < NB_TLV - 1) {

	    //On commence par lire l'entête du TLV
	    octetsLu = read(dazibaoFD, buf, 1);
	    if(octetsLu == -1) {
		perror("read : <lireTLV>");
		return -1;
	    }   

	    if(octetsLu > 0) {
		memset(&tab_tlv[position], 0, sizeof(tab_tlv[position])); 
		tab_tlv[position].type = (int) buf[0];
		// Si le tlv n'est pas un pad1, on doit continuer a lire
		if(buf[0] != PAD1) {
		    octetsLu = read(dazibaoFD, buf, 3);
		    if(octetsLu < 3) {
			if(octetsLu == -1) {
			    perror("read : <lireTLV>");
			    return -1;
			}
		    }
		    
		    //On enregistre sa longueur
		    tab_tlv[position].longueur = binaire_to_decimal(buf, 3);

		    //On enregistre la position de son champ value
		    tab_tlv[position].value = lseek(dazibaoFD, 0, SEEK_CUR);
		    if(tab_tlv[position].value == -1) {
			perror("lseek : <lireTLV>");
			return -1;
		    }

		    if(tab_tlv[position].type == COMPOUND || tab_tlv[position].type == DATED) {
			longueur = tab_tlv[position].longueur;
			if(tab_tlv[position].type == DATED) {
			    if(lseek(dazibaoFD, 4, SEEK_CUR) == -1) {
				perror("lseek : <lireTLV>");
				return -1;
			    }
			    longueur -= 4;
			}
		
			if(longueur == 0) {
				position++;
			}
			else {
				position++;
				while(longueur > 0) {
				    position_parcours = position;
				    position = lireTLV(dazibaoFD, tab_tlv, position_parcours);
				    if(position  == -1) {
					return -1; //On remonte l'erreur de lecture
				    }

				    longueur -= tab_tlv[position_parcours].longueur + 4;

				    if(tab_tlv[position_parcours].type == PADN) {
					position_parcours--;
				    }
				    else {
					tab_tlv[position_depart].nb_sous_tlv++;
				    }
				}
			}
		    }
		    else {
			/* On avance finalement a la fin du tlv */
			if(lseek(dazibaoFD, tab_tlv[position].longueur, SEEK_CUR) == -1) {
			    perror("lseek : <lireTLV>");
			    return -1;
			}

			if(tab_tlv[position].type != PADN)
			    position++;
		    }
		}

		// On retourne finalement la prochaine position a remplir car la lecture a été un succes

		return position;
	    }

	    // On retourne D_END car aucun TLV n'a été lu (end of file)
	    return D_END;
    }
    return -2;
}




/*==============================================================================================
affiche_type_tlv(tlv *tab_tlv, int numero, int niveau, int dazibaoFD)
================================================================================================
Affiche le type du premier tlv du tableau et si, un type compound ou dated est rencontré, 
la fonction se rappelle sur le prochain tableau (tab_tlv + 1)

le numero sert pour l'affichage du numero du tlv que l'on va afficher

le niveau sert à construire l' affichage type arborescence

Retourne le nombre de TLV affiché en cas de reussite
Retourne -1 si une erreur a été rencontrée
================================================================================================*/
int affiche_type_tlv(tlv *tab_tlv, int numero, int niveau, int dazibaoFD) {

    char debut_ligne[64];
    char date[23]; //Pour les dated
    int i;
    int total = 1;
    int resultat;
    int numero_sous_tlv = 1;
    int nb_sous_tlv_restants;
    int timestamp; //Pour les dated

    debut_ligne[0] = '\0';

    for(i = 0; i < niveau; i++) {
	if(i == 0) {
	    strcat(debut_ligne, "    ");
	}
	strcat(debut_ligne, " |");
    }
    
    switch(tab_tlv[0].type) {
	
    case PAD1:
    case PADN:
	total = 0; //On ne les affiche pas, on fait comme s'ils n'existent pas
	break;
    case TEXT:
	printf("%s (%d) Message texte \n", debut_ligne, numero);
	break;
    case PNG:
	printf("%s (%d) Image PNG \n", debut_ligne, numero);
	break;
    case JPEG:
	printf("%s (%d) Image JPEG \n", debut_ligne, numero);
	break;
    case COMPOUND:
	if(tab_tlv[0].nb_sous_tlv == 0) {
	    printf("%s (%d) Compound vide \n", debut_ligne, numero);
	}
	else {
	    printf("%s (%d) Compound \n", debut_ligne, numero);

    	    nb_sous_tlv_restants = tab_tlv[0].nb_sous_tlv;
	    i = 1;
	    while(nb_sous_tlv_restants > 0) {
	        resultat = affiche_type_tlv(tab_tlv + i, numero_sous_tlv, niveau + 1, dazibaoFD);
	        if(resultat == -1) {
		    return -1;
	        }

	        total += resultat; //On ajoute le nombre de tlv affiché par la suite au total

	        numero_sous_tlv++;

	        nb_sous_tlv_restants--;
	        i += resultat;
	    }
	}
	break;
    case DATED:
	//On va lire le timestamp dans le fichier
	if(lseek(dazibaoFD, tab_tlv[0].value, SEEK_SET) == -1) {
	    perror("lseek : <affiche_type_tlv>");
	    return -1;
	}

	if(read(dazibaoFD, date, 4) == -1) {
	    perror("read : <affiche_type_tlv>");
	    return -1;
	}

	timestamp = binaire_to_decimal(date, 4);
	dated(timestamp, date);

	if(tab_tlv[0].nb_sous_tlv == 0) {
	    printf("%s (%d) Dated vide : %s \n", debut_ligne, numero, date);
	}
	else {
	    printf("%s (%d) Dated : %s \n", debut_ligne, numero, date);

	    nb_sous_tlv_restants = tab_tlv[0].nb_sous_tlv;
	    i = 1;
	    while(nb_sous_tlv_restants > 0) {
	        resultat = affiche_type_tlv(tab_tlv + i, numero_sous_tlv, niveau + 1, dazibaoFD);
	        if(resultat == -1) {
		    return -1;
	        }
	
	        total += resultat; //On ajoute le nombre de tlv affiché par la suite au total

	        numero_sous_tlv++;

	        nb_sous_tlv_restants--;
	        i += resultat;
	    }
	}
	break;
    default: // Types inconnus
	printf("%s (%d) TLV de type inconnu \n", debut_ligne, numero);
	break;
	
    }

    return total;
}




/*==============================================================================================
supprimerTLV(int dazibaoFD, tlv *tab_tlv)
================================================================================================
Supprime le TLV à la premiere adresse du tableau en le remplaçant par un padn. 

Si un type compound ou dated est rencontré, on demande a l'utilisateur s'il veut supprimer 
l'intégralité ou bien uniquement un tlv contenu à l'interieur puis on se rappelle recursivement 
en cas de suppression d'un tlv "contenu" (tab_tlv + choix_utilisateur)

Retourne 1 si le TLV a été supprimé
Retourne 0 si aucun TLV n'a été supprimé
Retourne -1 si une erreur est survenu durant la suppression
================================================================================================*/
int supprimerTLV(int dazibaoFD, tlv *tab_tlv) {

    int choix = 1;
    int longueur_restante = tab_tlv[0].longueur;
    int compteur_sous_tlv = 0;
    int i = 0;
    int resultatSuppression;
    int prochain_tlv = 0; 
    char buffer[1024] = {0};

    switch(tab_tlv[0].type) {

    case COMPOUND:
	printf("Le TLV est de type compound :\n");
	printf("\t 1- Effacer l'intégralité du compound\n");
	printf("\t 2- Effacer un tlv a l'intérieur du compound\n");
	printf("\t 3- Annuler votre choix\n");
	printf("Quel est votre choix ? ");
	choix = saisie_chiffre();
	break;

    case DATED:
	printf("Le TLV est de type dated :\n");
	printf("\t 1- Effacer l'intégralité du dated\n");
	printf("\t 2- Effacer un tlv a l'intérieur du dated\n");
	printf("\t 3- Annuler votre choix\n");
	printf("Quel est votre choix ? ");
	choix = saisie_chiffre();
	break;

    }

    if(choix == 1) { //On supprime la totalité du tlv (cas de base, pour tous les tlv)

	//On se place a la position du type dans le fichier
	if(lseek(dazibaoFD, (tab_tlv[i].value - 4), SEEK_SET) == -1) {
	    perror("lseek : <supprimerTLV>");
	    printf("%ld",(tab_tlv[i].value - 4));
	    return -1;
	}

	//On écrit le nouveau type du TLV dans le fichier
	buffer[0] = PADN;
	if(write(dazibaoFD, buffer, 1) < 1) {
	    perror("write : <supprimerTLV>");
	    return -1;
	}
	buffer[0] = 0;

	//On se place à l'emplacement de la valeur dans le fichier
	if(lseek(dazibaoFD, tab_tlv[i].value, SEEK_SET) == -1) {
	    perror("lseek : <supprimerTLV>");
	    return -1;
	}

	//On remplace le contenu du tlv par une suite de 0
	while(longueur_restante > 0) {
	    if(longueur_restante < 1024) {
		if(write(dazibaoFD, buffer, longueur_restante) == -1) {
		    perror("write : <supprimerTLV>");
		    return -1;
		}
		longueur_restante = 0;
	    }
	    else {
		if(write(dazibaoFD, buffer, 1024) == -1) {
		    perror("write : <supprimerTLV>");
		    return -1;
		}
		longueur_restante -= 1024;
	    }
	}

	//On trouve l'indice du prochain TLV dans le tableau
	prochain_tlv = 1 + tab_tlv[0].nb_sous_tlv;
	for(i = 1; i < prochain_tlv; i++) {
	    prochain_tlv += tab_tlv[i].nb_sous_tlv;
	}

	//On decale maintenant tous les tlv
	for(i = 0; tab_tlv[prochain_tlv].type > -1 ; i++) {
	    tab_tlv[i] = tab_tlv[prochain_tlv];
	    prochain_tlv++;
	}

	//On vide ensuite les cases en trop
	while(i < prochain_tlv) {
	    tab_tlv[i].type = -1;
	    i++;
	}

	return 1;
    }
    else if(choix == 2) {
	//On demande le sous_tlv à supprimer
	printf("Quel TLV voulez vous supprimer ? ");
	choix = saisie_chiffre();

	if(choix > tab_tlv[0].nb_sous_tlv) {
	    return 0;
	}

	i = 1; //On ne compte pas l'indice 0 qui est le tlv contenant
	while(tab_tlv[i].type > -1 && i < (choix)) {
	    choix += tab_tlv[i].nb_sous_tlv;
	    i++;
	    compteur_sous_tlv++;
	}

	resultatSuppression = supprimerTLV(dazibaoFD, (tab_tlv+i));

	if(resultatSuppression == -1) {
	    return -1;
	}

	tab_tlv[0].nb_sous_tlv--;
	
	return  resultatSuppression;
    }
    else {
	return 0;
    }
}
