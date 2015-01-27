#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
#include <string.h>
#include "dazibao.h"
#include "operations_binaire.h"
#include "tlv.h"
#include "saisie_utilisateur.h"



/*==============================================================================================
int ouvrirDazibao(char *chemin, int type)
================================================================================================
Permet d'ouvrir le dazibao demandé, nommé par "chemin" s'il est valide. 
Le type est le mode d'acces que l'on passera a la fonction open.

Retourne un descripteur de fichier si l'ouverture s'est bien déroulée.
Retourne si une erreur s'est produite
================================================================================================*/
int ouvrirDazibao(char *chemin, int type) {

    int fd;

    if(dazibaoValide(chemin) < 1) {
	return -1;
    }

    fd = open(chemin, type);
	

    if(fd == -1) {
	perror("open : <ouvrirDazibao>");
	return -1;
    }

    // On decale la position dans le file descriptor de 4 octets (taille de l'entête)
    lseek(fd, 4, SEEK_CUR);

    return fd;
}




/*==============================================================================================
int dazibaoValide(char *chemin)
================================================================================================
Ouvre le dazibao, nommé par "chemin", et verifie si c'est un dazibao en lisant son entête

Retourne 1 si le fichier passé en paramètre est un dazibao valide
Retourne 0 si le fichier passé en paramètre n'est pas un dazibao valide
Retourne -1 si une erreur s'est produite
================================================================================================*/
int dazibaoValide(char *chemin) {
    
    int fd;
    int octetsLu;
    char buf[5];

    fd = open(chemin, O_RDONLY);

    if(fd == -1) {
	return -1;
    }

    if(flock(fd, LOCK_SH) == -1) {
	perror("flock : <dazibaoValide>");
	close(fd); //On oublie pas de fermer le descripteur  de fichier
	return -1;
    }
    
    //On commence par lire l'entête de 4 octets
    octetsLu = read(fd, buf, 4);
    if(octetsLu < 1) {
	if(octetsLu == -1) {
	    perror("read : <dazibaoValide>");
	    close(fd); //On oublie pas de fermer le descripteur  de fichier
	    return -1;
	}
	return 0; //Sinon, le dazibao n'est pas valide puisque l'entête n'est pas complet
    }

    if(flock(fd, LOCK_UN) == -1) {
	perror("flock : <dazibaoValide>");
	close(fd); //On oublie pas de fermer le descripteur  de fichier
	return -1;
    }

    // On teste maintenant la valeur de magic afin de voir s'il vaut 53 et renvoyer 0 sinon
    if(buf[0] != 53) {
	close(fd); //On oublie pas de fermer le descripteur  de fichier
	return 0; // Dazibao non valide
    } 
    
    // On teste maintenant la valeur de la version afin de voir s'il vaut 0 et renvoyer 0 sinon
    if(buf[1] != 0) {
	close(fd); //On oublie pas de fermer le descripteur  de fichier
	return 0; //Dazibao non valide
    }
    //Aucun traitement a faire de plus, car le champ MBZ est ignoré, on retourne 1
    close(fd); //On oublie pas de fermer le descripteur de fichier
    return 1;
}




/*==============================================================================================
int lireDazibao(dazibao un_dazibao) (fait appel à lireTLV du fichier tlv.h)
================================================================================================
Ouvre le dazibao fournis en paramètre s'il est valide, le lit et remplis son tableau de tlv
avec le contenu du dazibao.

Un verrou de type partagé sera utilisé pendant la durée de l'operation

Retourne 1 si la lecture s'est déroulée avec succes
Retourne -1 en cas d'erreur
================================================================================================*/
int lireDazibao(dazibao un_dazibao) {

    int dazibaoFD;
    int position = 0; //Position dans le tableau de TLV

    if((dazibaoFD = ouvrirDazibao(un_dazibao.chemin, O_RDONLY)) == -1) {
	perror("<open : lireDazibao>");
	return -1;
    }

    memset(un_dazibao.tab_tlv, -1, NB_TLV * sizeof(tlv));//on remplit de -1 le tableau de tlv

    // On pose un verrou partagé ici pour la lecture
    if(flock(dazibaoFD, LOCK_SH) == -1) {
	perror("flock : <lireDazibao>");
	return -1;
    }

    while((position = lireTLV(dazibaoFD, un_dazibao.tab_tlv, position)) != D_END && position != -2) {
	if(position == -1) {
	    return -1;
	}	
    }

    // Fin du verrou
    if(flock(dazibaoFD, LOCK_UN) == -1) {
	perror("flock : <lireDazibao>");
	return -1;
    }

    close(dazibaoFD);

    return 1;
}




/*==============================================================================================
int listeTypesDazibao(dazibao un_dazibao) (fait appel à affiche_type_tlv du fichier tlv.h)
================================================================================================
Parcours le tableau de tlv, précédemment remplis par lireDazibao, du dazibao passé en paramètre 
et affiche leur type sur la sortie standard sous forme d'arborescence

Retourne 1 si l'affichage s'est bien déroulé
Retourne 0 si une erreur s'est produite
================================================================================================*/
int listeTypesDazibao(dazibao un_dazibao) {
    int i = 0;
    int nb_tlv = 0;
    int numero_tlv = 1;
    int dazibaoFD;

    if((dazibaoFD = ouvrirDazibao(un_dazibao.chemin, O_RDONLY)) == -1) {
	perror("<open : <listeTypesDazibao>");
	return -1;
    }

    // On pose un verrou partagé ici pour la lecture
    if(flock(dazibaoFD, LOCK_SH) == -1) {
	perror("flock :<listeTypesDazibao>");
	return -1;
    }

    while(un_dazibao.tab_tlv[i].type > -1) {
	if(un_dazibao.tab_tlv[i].type != PAD1 && un_dazibao.tab_tlv[i].type != PADN) {
	    if((nb_tlv = affiche_type_tlv((un_dazibao.tab_tlv) + i, numero_tlv, 0, dazibaoFD)) == -1) {
		return -1;
	    }
	    i += nb_tlv;
	    
	    numero_tlv++;
	}
	else {
	    i++;
	}
    }

    // Fin du verrou
    if(flock(dazibaoFD, LOCK_UN) == -1) {
	perror("flock : <listeTypesDazibao>");
	return -1;
    }

    close(dazibaoFD);

    return 1;
}





/*=======================================================================================================
supprimerEntree(dazibao unDazibao, int num_entree_supprimer) (fait appel à supprimerTLV du fichier tlv.h)
=========================================================================================================
Va trouver le tlv ayant le numero num_entree_supprimer dans le tableau de tlv du dazibao fournis en
paramètre afin faire appel à supprimerTLV pour réécrire un padn par dessus et effacer ainsi le TLV

Un verrou de type exclusif sera utilisé pendant la durée de l'operation

Retourne 1 si le tlv demandé à bien été supprimé
Retourne 0 si aucun tlv n'a été supprimé (le tlv demandé n'a pas été trouvé)
Retourne -1 en cas d'erreur
=========================================================================================================*/
int supprimerEntree(dazibao unDazibao, int num_entree_supprimer) {

    int dazibaoFD;
    int i = 0;
    int resultatSuppression;

    if((dazibaoFD = ouvrirDazibao(unDazibao.chemin, O_RDWR)) == -1) {
	return -1;
    }


    // On pose un verrou exclusif ici pour l'écriture
    if(flock(dazibaoFD, LOCK_EX) == -1) {
	perror("flock : <supprimerEntree>");
	return -1;
    }

    while(unDazibao.tab_tlv[i].type > -1 && i < (num_entree_supprimer - 1)) {
	//Le numero de l'entrée ne prends pas en compte les sous TLV des TLV precedents
	//On ajoute donc le nombre de sous_tlv rencontré au numero de l'entrée pour tomber
	//Sur le bon indice du tableau
	num_entree_supprimer += unDazibao.tab_tlv[i].nb_sous_tlv;

	i++;
    }

    if(i != (num_entree_supprimer - 1) || unDazibao.tab_tlv[i].type == -1) {
	if(flock(dazibaoFD, LOCK_UN) == -1) {
	    perror("flock : <supprimerEntree>");
	    return -1;
	}
	return 0;
    }

    resultatSuppression = supprimerTLV(dazibaoFD, ((unDazibao.tab_tlv) + i));

    if(flock(dazibaoFD, LOCK_UN) == -1) {
	perror("flock : <supprimerEntree>");
	return -1;
    }

    close(dazibaoFD);

    return resultatSuppression;
}
