 #include "ecriture.h"


/*============================================================================
int creation_dazibao(dazibao* dazi)
==============================================================================
Interagie avec l'utilisateur pour créer un dazibao.
Crée un dazibao avec une entête. Et on remplit le champ chemin de la 
structure dazibao en paramètre.

Si le  fichier dazibao existe déjà on demande à l'utilisateur si il veux 
écraser le dazibao existant.

Retourne -1 si il y a une erreur.
Retourne -3 si l'utilisateur n'a pas voulut écraser le dazibao existant.
============================================================================*/
int creation_dazibao(dazibao* dazi){

	//demander à l'utilisateur le nom du fichier :
	printf("Saisir nom dazibao:\n");//sera creer dans le répertoire courrant
	char file_path[256];
	saisie_utilisateur(file_path);

	//vérifier que le fichier n'existe pas :
	struct stat buffer;
  	if(stat (file_path, &buffer) == 0){
			printf("le fichier \"%s\" existe deja\n",file_path);
			printf("l'opération suivante va ecraser le fichier existant <o/n>\n");
			char saisie[256];
			saisie_utilisateur(saisie);
			//l'utilisateur veux supprimer le fichier :
			if(saisie[0]=='o'){
				if(remove(file_path)==-1){
					perror("<remove : creation_dazibao>");
					return -1;//erreur remove
				}
			}else{
				return -3;//l'utilisateur ne veux pas écraser le fichier existant
			}	
	}

	//créaction du fichier dazibao :
	int fd_dazi =  open(file_path,O_WRONLY|O_CREAT,0664);
	if(fd_dazi == -1){
		perror("<creation_dazibao : open>");
		return -1;
	}
	lseek(fd_dazi, 0, SEEK_END);//on se met à la fin du fichier (on ne peux pas utiliser O_APPEND!!!!!ça pose problème et je sais pas pourquoi)

	if(fd_dazi == -1){
		perror("<open : creation_dazibao>");
		return -1;
	}

	//ajouter une entete :
	if(insert_entete(fd_dazi) == -1){
		close(fd_dazi);
		remove(file_path);
		//pas besoin de perror il est déjà signalé dans insert_entete
		return -1;
	}	
	close(fd_dazi);//fermer le fichier dazibao
	strcpy(dazi->chemin,file_path);//copier le nom du dazibao
	lireDazibao(*dazi);//pour effacer le tableau de tvl
	return 1;
}



/*============================================================================
int ajout(dazibao* dazi)
==============================================================================
Intéragie avec l'utilisateur pour ajouter des tlv dans la structure dazibao 
passé en paramètre.

Retourne la taille en octet écrite dans le fichier dazibao.
Retourne -1 ou une valeur négative en cas d'erreur.
Problème : en cas d'écriture incomplète le fichier est corrompus.
==============================================================================*/
int ajout(dazibao* dazi){
        int retour = 0;
	printf("\t0 : pad1\n\t1 : padn\n\t2 : txt\n\t3 : png\n\t4 : jpeg\n\t5 : compound\n\t6 : dated\n");

	int fd_dazi = open(dazi->chemin,O_WRONLY);
	
    if(fd_dazi <0){
		perror("<open : ajout>");
		return -1;
	}
	
	retour = insert_tlv(fd_dazi,VERBOS);

	close(fd_dazi);
	return retour;	
}



/*============================================================================
int supprimer(dazibao dazi)
==============================================================================
Interagie avec l'utilisateur pour supprimer un tlv dans un fichier dazibao.
Cette fonction remplace le tlv existant par un padN.

Retourne -1 en cas d'erreur.
==============================================================================*/
int supprimer(dazibao dazi) {
    int tlv_a_supprimer;
    int resultat_suppression;

    printf("Quel TLV voulez vous supprimer ?");
    tlv_a_supprimer = saisie_chiffre();
    resultat_suppression = supprimerEntree(dazi, tlv_a_supprimer);
    if(resultat_suppression != 0) {  
	if(resultat_suppression == -1) {///PROBLEME : CETTE LIGNE N'A PAS DE SENS CAR ELLE NE PEUX PAS ETRE ATTEINTE il faut probablament la mettre plus haut
	    return -1;
	}
	printf("L'element selectionné a bien été supprimé\n");
    }
    else {
	printf("Aucun element n'a été supprimé\n");
    }

    return resultat_suppression;
}



/*============================================================================
int compaction(dazibao dazi,int verbos)
==============================================================================
La compaction supprime tout les padN et pad1                  
en réecrivant le fichier dazibao. "verbos" permet ou non d'afficher avec des
message le résultat de la compaction.

Retourne 1 si la compaction à réduit le fichier.
Retourne 0 si ça n'a pas trouvé de tlv padN ou pad1 et donc n'a rien changé.
Retourne -1 en cas d'érreur.
============================================================================*/
int compaction(dazibao dazi,int verbos){//verbos permet d'affichager ou non des messages durant la compaction

	//ouverture du fichier dazibao :
	int fd_dazi =  open(dazi.chemin,O_RDWR);
	lseek(fd_dazi, 0, SEEK_END);//on se met à la fin du fichier

	if(fd_dazi == -1){
		perror("<open : compaction>");
		return -1;
	}

	//calculer la taille :
	int taille;
	int taille2;
	if(verbos) taille = taille_fichier(fd_dazi);

	if(verbos) printf("taille du dazibao avant compaction = %d\n",taille);

	/////////DEBUT CALCUL////////////////
   	int indice = 0;
	int tron=4;

    if((lireDazibao(dazi))==-1){
		perror("<lireDazibao : compaction>");
		return -1;		
	}
 	if(flock(fd_dazi, LOCK_EX) == -1) {
		perror("<flock : compaction>");
		return -1;
    	}
	if(lseek(fd_dazi, 4, SEEK_SET)==-1){
		perror("<lseek : compaction>");
		return -1;
	};



	//TRON = ENDROIT COURRANT OU L'ON ECRIT DANS LE FICHIER

	
	while(dazi.tab_tlv[indice].type > 1) { 
			
		int longueur=0;

		tron+=insert_type(fd_dazi,dazi.tab_tlv[indice].type);//on insert le type

		longueur=calcul_longueur(indice, dazi)-4;//champ length, -4 car dans la fonction calcul_longueur on ajoute 4 pour ce tlv dans la valeur de retour

		tron+=insert_long(fd_dazi,longueur);//on insert la longueur
		
		if(dazi.tab_tlv[indice].type == DATED){//on insert la date
			unsigned char dat[4];
			lseek(fd_dazi,dazi.tab_tlv[indice].value, SEEK_SET);
	 		if(read( fd_dazi,  dat, 4 ) == -1) {
		  	  perror("<read : compaction>");
		 	   return -1;
			}
			lseek(fd_dazi,tron, SEEK_SET);
			if(write(fd_dazi, dat, 4) == -1) {
		 		perror("<write : compaction>");
		  	  	return -1;
			}
			tron+=4;

		}

		if(dazi.tab_tlv[indice].type == JPEG ||dazi.tab_tlv[indice].type == PNG||dazi.tab_tlv[indice].type == TEXT||(dazi.tab_tlv[indice].type>DATED)) {//possibilité de réduire les arguments
			int len = dazi.tab_tlv[indice].longueur;
			unsigned char buf[BUF_MAX_SIZE];
			int rc,size_RW = BUF_MAX_SIZE;
			int pos=0;
 
		while(len>0){
			if(len<BUF_MAX_SIZE){
				size_RW = len;
				len = 0;
			}else{
				len -= BUF_MAX_SIZE;
			}

			if(lseek(fd_dazi,dazi.tab_tlv[indice].value+pos, SEEK_SET)==-1){
				perror("<lseek : compaction>");
				return -1;
			};
			if(read( fd_dazi,  buf, size_RW ) == -1) {
			   	perror("<read : compaction>");
			    	return -1;
			}
			


			if(lseek(fd_dazi, tron+pos, SEEK_SET)==-1){
				perror("<lseek : compaction>");
				return -1;
			};
			rc = write(fd_dazi,buf,size_RW);
		
			if(rc != size_RW){
				perror("<write : compaction>") ;
				return -1;//erreur fichier remplir ou pas le champ error
			}
			if(rc < 0) {
				perror("<write : compaction>") ;
				return -1;//erreur fichier remplir ou pas le champ error
			}
			pos+=rc;
		}

		tron+=dazi.tab_tlv[indice].longueur;


		}

		indice++;
	}
	

 	//on coupe  apres le nombre d'octet qu'on a reecrit
 	if((ftruncate(fd_dazi, tron) )== -1) {
  		 perror("<ftruncate : compaction>");
   		return -1;
 	}
	if(flock(fd_dazi, LOCK_UN) == -1) {
		perror("<flock : compaction>");
		return -1;
    	}
	///////////////FIN CALCUL////////////////////////
	if(verbos) taille2 = taille_fichier(fd_dazi);
	if(verbos) printf("taille du dazibao après compaction = %d\n",taille2);

	if((close(fd_dazi))==-1) {
  		 perror("<close : compaction>");
   		return -1;
 	}//fermeture du dazibao
	
	if(taille == taille2)
	    return 0; //La compaction n'a rien changé
	return 1;//pas d'erreur
}


/*============================================================================
int calcul_longueur(int indice, dazibao dazi)         (utilisé par compaction)
==============================================================================
Fonction récursive qui permet de récupérer la taille exacte d'un tlv (en octet)
avec son entête.

Precision de "taille exacte" :
En effet, si nous avons un tlv composé (compound ou dated) nous voulons avoir 
la nouvelle taille après compaction, c'est à dire sans les padN et pad1.
Par exemple si un tvl compound contien un padN nous obtiendrons la taille de 
ce compound sans le padN.

Retourne la longueur d'un tlv à partir de son indice dans la structure dazibao

Il n'y a pas de retour d'erreur l'utilisateur de la fonction doit passer des 
arguments qui sont bon.
==============================================================================*/
int calcul_longueur(int indice, dazibao dazi){
	

	int i;
	int longueur=0;


	if((dazi.tab_tlv[indice].type==COMPOUND)||(dazi.tab_tlv[indice].type==DATED)){

		i= indice+1;
		int nb=dazi.tab_tlv[indice].nb_sous_tlv;
		while(nb>0) {
			longueur += calcul_longueur(i,dazi);//on additionne la longueur des sous tlv
			i = tlv_suivant(dazi,i);//on recupere l'indice du prochain tlv au meme niveau
			nb--;
		}

		if(dazi.tab_tlv[indice].type==COMPOUND)
			return 4+longueur;//longueur contient les longueurs additionnees de ses sous tlv
	

		if(dazi.tab_tlv[indice].type==DATED)
			return 8+longueur;//longueur contient les longueurs additionnees de ses sous tlv
	}
        else {
		if(dazi.tab_tlv[indice].type >=0) {
			return dazi.tab_tlv[indice].longueur+4;	
		}
	}


	return longueur;

}





/*============================================================================
int tlv_suivant(dazibao dazi, int indice)  
============================================================================
Permet de calculer l'indice dans le tableau de tlv du tlv suivant qui est au 
même niveau que l'indice passé en paramètre.

Voici un exemple d'affichage pour éclairé ce propos abscon :

 ...
 (5) Compound 			indice = 5
     | (1) Message texte	indice = 6
     | (2) Message texte	indice = 7
 (6) texte			indice = 8

Dans l'exemple présenté le tlv (5) et le (6) sont de même niveau. Mais leurs
indices dans le tableau de tlv doivent être calulés.

Il n'y a pas de retour d'erreur l'utilisateur de la fonction doit passer des 
arguments qui sont bon.
============================================================================*/
int tlv_suivant(dazibao dazi, int indice) {

  int restants = dazi.tab_tlv[indice].nb_sous_tlv;
  int suivant = indice + 1;

  while(restants > 0) {
    restants += dazi.tab_tlv[suivant].nb_sous_tlv - 1;
    suivant++;
  }	
  return suivant;

}



/*============================================================================
int extraction_tlv(dazibao dazi,int indice, char* path_dest,char* path_name)
============================================================================
Fonction qui intéragie avec l'utilisateur.
Extrait un tlv dans le dazibao "dazi" à l'indice "indice" du tableau de tlv 
et l'enregistre dans le path "path_dest" avec le nom "path_name".
Retour -1 si il y a une erreur.
============================================================================*/
int extraction_tlv(dazibao dazi,int indice, char* path_dest,char* path_name){


	int type = dazi.tab_tlv[indice].type;
	//verification de l'indice :
	if( (type != JPEG) && (type!=PNG) && (type!=TEXT)){
		printf("extraction impossible - type = %d\n",type); //voir si une extraction impossible est un pad1 si c'est le cas interdir!!!!!
		printf("Forcer l'extraction? <1 = oui/autre = non>\n");
		if(saisie_chiffre()!=1)//sortir avec une erreur si l'utilisateur n'a pas voulut forcer l'extraction
			return -1;
	}

	//ouverture du fichier dazibao :
	int fd_dazi =  open(dazi.chemin,O_RDWR,0664);
	lseek(fd_dazi, dazi.tab_tlv[indice].value, SEEK_SET);//on se met à la position de début de la valeur

	if(fd_dazi == -1){//erreur
		perror("<open : extraction>");
		return -1;
	}

	//formation du chemin+nom+extention 
	//où on va écrire le fichier à extraire
	char path[512];
	memset(&path,0,512);
	strcat(path,path_dest);
	strcat(path,"/");
	strcat(path,path_name);

	switch(type){

		case TEXT	: strcat(path,".txt");break;

		case JPEG	: strcat(path,".jpg");break;

		case PNG 	: strcat(path,".png");break;

		default 	: break;//rien si tlv autre

	}

	//création du fichier de sortie :
	int fd_dest =  open(path,O_WRONLY|O_CREAT,0664);
	if(fd_dest == -1){//erreur
		perror("<extraction : open>");
		return -1;
	}
	


	int len = dazi.tab_tlv[indice].longueur;

	//Partie extraction et écriture dans le fichier de destination ==========================================

	unsigned char buf[BUF_MAX_SIZE];
	int rc,size_RW = BUF_MAX_SIZE;
	

	

	while(len>0){
		//connaitre la taille de se que l'on veux lire et écrire :
		if(len<BUF_MAX_SIZE){
			size_RW = len;
			len = 0;
		}else{
			len -= BUF_MAX_SIZE;
		}

		//LECTURE de la valeur à partir de fd_value:====================================
		rc = read(fd_dazi,buf,size_RW);
		//vérification de rc :
		if(rc != size_RW){
			perror("<read : insert_value>") ;
			return -1;//erreur fichier remplir ou pas le champ error
		}
		if(rc < 0) {
			perror("<read : insert_value>") ;
			return -1;//erreur fichier remplir ou pas le champ error
		}


		//ECRITURE de la valeur dans fd_dest :==========================================

		rc = write(fd_dest,buf,size_RW);
		//vérification de rc :
		if(rc != size_RW){
			perror("<write : insert_value>") ;
			return -1;//erreur fichier remplir ou pas le champ error
		}
		if(rc < 0) {
			perror("<write : insert_value>") ;
			return -1;//erreur fichier remplir ou pas le champ error
		}
	}


	//================================================================================================




	close(fd_dazi);//fermeture du dazibao
	close(fd_dest);
	return 1;
}






/*============================================================================
int selection_indice(dazibao dazi)		(utiliser par extraction)
============================================================================
Interraction avec l'utilisateur pour savoir ce qu'il veux extraire.
Retourne l'indice exacte dans le tlv ou se trouve l'élément selectionné 
par l'utilisateur.
(cette fonction pourrait être utilisé par supprimé mais à été codé plus tard)
============================================================================*/
int selection_indice(dazibao dazi){

	//demander à l'utilisateur ce qu'il veux selectionner :
	printf("Choisir l'indice du tlv à extraire :\n");
	int selection;

	while (1){//boucle pour demander à l'utilisateur se qu'il veux selectionner
		selection = saisie_chiffre();
		if(selection==-1){
			printf("fin selection\n");
			return -1;
		}

		if(selection>NB_TLV){//l'indice demandé est plus grand que le nombres max de TLV
			return -1;
		}
		break;
	}

	//recherche du vrai indice dans le tableau
	int indice = 0;
	while(--selection){
		indice = tlv_suivant(dazi,indice);
	}



	while (1){//boucle atteindre un tlv simple



		int type = dazi.tab_tlv[indice].type;//connaitre le type

		if(type == DATED){//tlv dated
			printf("vous avez selectionné un dated\n");

			
			//on cherche cherche à trouver le sous tlv interne qui est 
			//n'y un PADN n'y un PAD1 n'y un DATED
			indice++;//indice du sous tlv
			while(     (dazi.tab_tlv[indice].type == PADN) 
				|| (dazi.tab_tlv[indice].type == PAD1) 
				|| (dazi.tab_tlv[indice].type == DATED)){

				indice = tlv_suivant(dazi,indice);
			}
			//si nous sommes arrivé sur un tlv simple on sort
			if(dazi.tab_tlv[indice].type != COMPOUND)
				break;
			
			
		}else if(type == COMPOUND){
			
			printf("vous etes dans un compound choisir le sous-tlv\n");
			int ch = saisie_chiffre();
			if(ch == -1) return -1;
			
			if(ch>dazi.tab_tlv[indice].nb_sous_tlv){//erreur on a choisit un indice en dehors du compound
				return -1;
			}

			indice++;//indice du premier sous tlv 
			//si c'est PAD on incrémente plusieurs fois
			while( (dazi.tab_tlv[indice].type == PADN) || (dazi.tab_tlv[indice].type == PAD1) )		
				indice = tlv_suivant(dazi,indice);//indice du tlv suivant
			int i;
			for(i=1 ; i<ch; i++){
				indice = tlv_suivant(dazi,indice);	
			}
			
		}else {//si c'est un tvl simple on tien notre indice
			break;//on sort de la boucle
		}

	}
	
	return indice;
}

