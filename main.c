#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>

#include "interface.h"
#include "saisie_utilisateur.h"
#include "dazibao.h"
#include "client.h"

int main(int argc,char** argv){
	intro();//message d'introduction
	int i;
	//préparation de la structure dazibao :
	dazibao dazi;//notre structure dazibao
	tlv tab_tlv[NB_TLV];//on suppose qu'il y aura moins de 4096 tlv dans un fichier dazibao
	char chemin[T_SAISIE];//pointeur vers le chemin du fichier
	dazi.chemin = chemin;//attribution des pointeurs
	dazi.tab_tlv = tab_tlv;//attribution des pointeurs

	struct sigaction ignore, save_sigpipe;
	notifications notif;

	memset(&notif, 0, sizeof(notifications));
	dazi.notif = &notif;

	memset(&ignore, 0, sizeof(struct sigaction));
	ignore.sa_handler = SIG_IGN;
	ignore.sa_flags = 0;
	if(sigaction(SIGPIPE, &ignore, &save_sigpipe) < 0) {
	    perror("sigaction");
	    exit(EXIT_FAILURE);
	}

	//OUVERTURE / CREATION DAZIBAO================================================================================
	//ouvrir le dazibao en paramètre :
	if(argc > 1) {
	    if(strcmp(argv[1], "-o") == 0) {
		if(argc > 2) {

		    strcpy(dazi.chemin,argv[2]);//copie du chemin dans le dazibao
			
		    if(argc > 4) {
			dazi.notif->nb_dazibao = 0;
			i = 3;
			while(i < argc && strcmp(argv[i], "-s") != 0) {
			    i++; //On cherche la position d'une eventuelle option -s pour le suivi
			}
			dazi.notif->nb_dazibao = argc - i;
			dazi.notif->dazibao_a_suivre = argv + i;
			client(dazi.notif,1); //On essaye de se connecter au serveur de notifications
		    }


		    //ouvrir le dazibao :		
		    if(lireDazibao(dazi)==-1){
			printf("impossible - d'ouvrir \"%s\"\n\"le fichier n'existe pas ou ça n'est pas un dazibao valide\"\n",argv[2]);
			fin();
			return -1;
		    }
		}
		else {
		    fprintf(stderr,"Erreur : Aucun fichier spécifié avec l'option -o\n");
		    fin();
		    return -1;
		}
	    }
	    else if(strcmp(argv[1], "-s") == 0) {
		dazi.notif->nb_dazibao = 0;
		i = 2;

		while(i < argc && strcmp(argv[i], "-o") != 0) {
		    i++; //On cherche la position d'une eventuelle option -o pour l'ouverture
		}

		dazi.notif->nb_dazibao = i - 2;
		dazi.notif->dazibao_a_suivre = argv + 2;
		client(dazi.notif,1); //On essaye de se connecter au serveur de notifcations

		if(i != argc) {
		    strcpy(dazi.chemin,argv[i+1]);//copie du chemin dans le dazibao
		    //ouvrir le dazibao :		
		    if(lireDazibao(dazi)==-1){
			printf("impossible - d'ouvrir \"%s\"\n\"le fichier n'existe pas ou ça n'est pas un dazibao valide\"\n",argv[1]);
			fin();
			return -1;
		    }
		}
		else {
		    //pas d'option -o (il faut ouvrir ou creer un dazibao)
		    if(affiche_menu_oc(&dazi) == 0) {
			return 0;
		    }
		}
	    }
	    else {
		fprintf(stderr,"Erreur : usage %s [OPTION <nom_fichier>]\n",argv[0]);
		fin();
		return -1;
	    }

	}else{
		//pas de 2 eme paramètre (il faut ouvrir ou creer un dazibao)
	        if(affiche_menu_oc(&dazi) == 0) {
		    return 0;
		}
	}
	//=================================================================================================================


	//MENU OPTION======================================================================================================
	affiche_menu_option(&dazi);
	
	return 0;
}
