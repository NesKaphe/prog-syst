#include "interface.h"

/*============================================================================
void intro()
==============================================================================
message d'introduction
============================================================================*/
void intro(){

	clrscr();
	//couleur(jaune);
	printf(YELLOW);
	printf("PROGRAMME DAZIBOA :\n");
	printf("realise par : Alain Dias, Sebastien Ciupek, Clement Charasson\n\n");
	//couleur(blanc);
	printf(DEFAULT_COLOR);

}




/*============================================================================
void fin()
==============================================================================
message de fin de programme
============================================================================*/
void fin(){
	printf(RED);
	printf("FIN DU PROGRAMME\n\n");
	printf(DEFAULT_COLOR);
}





/*============================================================================
int menu_ouverture_creation(dazibao* dazi)
==============================================================================
Menu qui demande à l'utilisateur si il veux creer ou ouvrir un dazibao et 
remplit la structure dazibao en paramêtre.
option 1 : terminer programmme
option 2 : creation d'un dazibao
option 3 : ouverture d'un dazibao
option 4 : connexion au serveur de notification

Retourne 0 en cas de réponse non valide.
Cette fonction retourne le numéro de l'action demandé par l'utilisateur.
En cas d'échec c'est le même numéros mes négatif qui est retourné.
============================================================================*/
int menu_ouverture_creation(dazibao* dazi){
	printf(PURPLE);//couleur
	printf("OUVERTURE / CREATION :\n");
	printf("\t1.terminer\n");
	printf("\t2.Creer un dazibao\n");
	printf("\t3.Ouvrir un dazibao existant\n");
	if(dazi->notif->connecte == 0 && dazi->notif->nb_dazibao != 0) {
	    printf("\t4.Se connecter au serveur de notifications\n");
	}
	printf(DEFAULT_COLOR);
	//saisie clavier par l'utilisateur :
	int rep = saisie_chiffre();

	switch(rep){
		case 0:printf("<reponse invalide>\n");
			return 0;


		case 1: printf("terminer :");
			return 1;
			break;

		case 2:	printf("creation : ");
			if(creation_dazibao(dazi) < 0) return -2;//erreur
			return 2;
			break;

		case 3:	printf("ouverture :");
			if(ouverture_dazibao(dazi) == -1)//ouvre et vérifie le dazibao
				return -3;//erreur
			return 3;
			break;

		case 4: printf("Connexion :\n");
		        if(client(dazi->notif, 1)==-1) {
			    return -4;
		        }
			printf("Connexion reussie\n");
	                return 0;
	                break;

		default:printf("<reponse invalide>\n");
			return 0;
	}

	return 0;
}





/*============================================================================
int menu_option(dazibao* dazi)
==============================================================================
Menu option qui prend une structure dazibao en paramètre et intérragie avec
l'utilisateur.
option 1 : terminer programme
option 2 : ajouter un tlv au dazibao
option 3 : supprimer un tlv du dazibao
option 4 : afficher le contenu du dazibao
option 5 : faire appelle au menu ouverture et création
option 6 : faire une compaction
option 7 : faire l'extraction d'un tlv
option 8 : suivre un dazibao particulier


Retourne 0 en cas de réponse non valide.
Cette fonction retourne le numéro de l'action demandé par l'utilisateur.
En cas d'échec c'est le même numéros mes négatif qui est retourné.
============================================================================*/
int menu_option(dazibao* dazi){
        int ret = 0;
	printf(CYAN);
	printf("\nMENU:\t\tfichier : \"%s\"\n\n",dazi->chemin);
	printf("\t1.terminer\n");
	printf("\t2.Ajouter des tlv au dazibao \"%s\"\n",dazi->chemin);
	printf("\t3.Supprimer des tlv au dazibao \"%s\"\n",dazi->chemin);
	printf("\t4.Afficher le contenu de \" %s \"\n",dazi->chemin);
	printf("\t5.Ouvrir/Creer un autre dazibao\n");
	printf("\t6.Faire la compaction manuellement de \" %s \"\n",dazi->chemin);
	printf("\t7.Extraction d'un tlv de \" %s \"\n",dazi->chemin);
	if(dazi->notif->connecte == 0 && dazi->notif->nb_dazibao != 0) {
	    printf("\t8.Se connecter au serveur de notifications\n");
	}

	printf(DEFAULT_COLOR);

	//saisie clavier par l'utilisateur :
	int rep = saisie_chiffre();

	switch(rep){

		case 0: printf("<reponse invalide>\n");
			return menu_option(dazi);//on fait un apelle récursif si on a pas de bonne réponse

		case 1: printf("terminer :\n");
			return 1;
			break;

		case 2:	printf("ajouter :\n");
		        if((ret = ajout(dazi))==-1)
				return -2;
			printf("FAIT\n");
			if(ret)
			    client_ecriture(dazi);
			return 2;
			break;

		case 3:	printf("supprimer :\n");
		        if(affichage_simple(dazi) == -1)
				return -4;//erreur
			if(supprimer(*dazi) == -1)
			        return -3;
			client_ecriture(dazi);
			return 3;
			break;

		case 4:	printf("afficher :\n");
			if(affichage_simple(dazi) == -1)
				return -4;//erreur
			return 4;
			break;

		case 5:	printf("ouvrir/creer :\n");
			if(menu_ouverture_creation(dazi)<0)
				return -5;//erreur
			return 5;
			break;

		case 6: printf("compaction :\n");
		        if((ret = compaction(*dazi,1))==-1)//1 pour que la réponse soit "verbeuse"
				return -6;//erreur
			printf("%s\n", dazi->chemin);
			if(ret)
			    client_ecriture(dazi);
			return 6;
			break;


		case 7: printf("extraction :\n");
		        if(affichage_simple(dazi) == -1)
				return -4;//erreur affichage
			
			//!!!!!TOUT METTRE DANS LA FONCTION EXTRACTION TLV !!!!!!!!!!!!!!!!!!!!!
			int indice;
			if((indice = selection_indice(*dazi)) == -1)//demande à l'utilisateur l'élément à selectionner
				return -7;//erreur

			/////////PARTIE EXTRACTION IMMEDIATE/////////////////////////////
			//format du nom du fichier "extract_<time stamp>"
			//!!!!!!tout ceci est à mettre dans la fonction extraction tlv
			char nom[256],string[256];
			memset(&nom,0,256);
			memset(&string,0,256);
			strcat(nom,"extract_");
			int secondes = time(NULL);
			sprintf(string, "%d",secondes);
			strcat(nom,string);

			printf("Extraction immediate? 1 = oui\n");//pour faire une extraction dans le répertoire extract

			if(saisie_chiffre() == 1){
				
				if(extraction_tlv(*dazi,indice,"./extract",nom) == -1){
					return -7;
				}else
					return 7;
			}
			//////////////////////////////////////////////////////////////////
			///////PARTIE EXTRACTION DANS UN REPERTOIRE PARTICULIER///////////
			printf("Choir zone d'extraction :");
			char path[T_SAISIE];
			saisie_utilisateur(path);

			if(extraction_tlv(*dazi,indice,path,nom) == -1){
				return -7;
			}else

				return 7;
			/////////////////////////////////////////////////////////////////
			break;
		
	        case 8: printf("Connexion :\n");
		        if(client(dazi->notif,1)==-1) {
			    return -8;
		        }
			printf("Connexion reussie\n");
	                return 8;
	                break;


		default:printf("<reponse invalide>\n");
			return menu_option(dazi);//on fait un apelle récursif si on a pas de bonne réponse
	}

	return 0;
}



/*============================================================================
affiche_menu_oc(dazibao *dazi)
==============================================================================
Affiche et gère les erreurs du menu affiché par menu_ouverture_creation
============================================================================*/
int affiche_menu_oc(dazibao *dazi) {
    int moc;//menu_ouverture_creation (moc)
    //tant que l'utilisateur n'a pas rentré quelque chose de correcte on recommence:
    while((moc = menu_ouverture_creation(dazi))<2){
	printf(RED);
	//récupération des erreurs :
	switch(moc){
	case  1: fin(); return 0; break;//fin du programme
	case -2: printf("erreur - creation\n"); break;
	case -3: printf("erreur - ouverture\n"); break;
	default: break;
	}
	printf(DEFAULT_COLOR);
    }
    return 1;
}



/*============================================================================
affiche_menu_option(dazibao *dazi)
==============================================================================
Affiche et gère les erreurs du menu affiché par menu_option
============================================================================*/
int affiche_menu_option(dazibao *dazi) {
    int mo;//menu_option (mo)
    while((mo = menu_option(dazi))!=0){
	printf (RED);
	//récupération des erreurs :
	switch(mo){
	case 1 : fin(); return 0; break;//fin du programme
	case -2: printf("erreur - ajout\n"); break;
	case -3: printf("erreur - supprimer\n"); break;
	case -4: printf("erreur - afficher\n"); break;
	case -5: printf("erreur - ouvrir/creer\n"); break;
	case -6: printf("erreur - compaction\n"); break;
	default: break;//le printf est un debug
	}
	printf( DEFAULT_COLOR);
    }
    return 1;
}
