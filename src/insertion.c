#include"insertion.h"



int insert_entete(int fd_dazibao){

	unsigned char buf[4];
	memset(buf,0,4);//mettre le buffer à zéro
	buf[0]=53;

	if(write(fd_dazibao,buf,4)==-1){
	   perror("<write : insert_entete>");
	   return -1;
	}
	return 4;
}

						
int insert_type(int fd_dazibao,unsigned char type){

	if(write(fd_dazibao,&type,1)==-1){
	   perror("<write : insert_type>");
	   return -1;
	}
	return 1;
}


int insert_long(int fd_dazibao,int decimal_length){

	unsigned char length[3];
	longueur_bin(decimal_length,length);//convertion de la longeur décimal int en longeur binaire unsigned char
	if(write(fd_dazibao,length,3)==-1){
	   perror("<write : insert_long>");
	   return -1;
	}
	return 3;
}



int insert_value(int fd_dazibao,int fd_value){

	unsigned char buf[BUF_MAX_SIZE];
	int rc,size_RW = BUF_MAX_SIZE;

	int len = taille_fichier(fd_value);
	int memo_len = len;//retenir la longeur pour la retourner
	

	while(len>0){
		//connaitre la taille de se que l'on veux lire et écrire :
		if(len<BUF_MAX_SIZE){
			size_RW = len;
			len = 0;
		}else{
			len -= BUF_MAX_SIZE;
		}

		//LECTURE de la valeur à partir de fd_value://////////////////////////////////
		rc = read(fd_value,buf,size_RW);
		//vérification de rc :
		if(rc != size_RW){
			perror("<read : insert_value>") ;
			return -1;//erreur fichier remplir ou pas le champ error
		}
		if(rc < 0) {
			perror("<read : insert_value>") ;
			return -1;//erreur fichier remplir ou pas le champ error
		}


		//ECRITURE de la valeur dans fd_dazibao ://///////////////////////////////////

		rc = write(fd_dazibao,buf,size_RW);
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

	
	return memo_len;
}




int taille_fichier(int fd_fichier){
    struct stat sb;
    if (fstat(fd_fichier, &sb) == -1) {
        perror("<stat : taille_fichier>");
        return -1;
    }
    return sb.st_size;
}



int insert_tlv_generic(int fd_dazibao,char* file_path,unsigned char type){
	int len=4,tmp;//pour renvoyer la taille de se qui à été écrit dans le dazibao 

	//insert type :
	if(insert_type(fd_dazibao,type)==-1){
		return -1;
	}
	//ouverture du fichier :
	int fd_value = open(file_path,O_RDONLY);
	if(fd_value == -1){
		perror("<open : insert_tvl_generic>");
		return -1;
	}
	//conaitre la taille du fichier value :
	int length = taille_fichier(fd_value);
	//insert longueur :
	if((length == -1) || (insert_long(fd_dazibao,length)==-1)){
		close(fd_value);
		return -1;
	}
	//insert value :
	tmp = insert_value(fd_dazibao,fd_value);
	if(tmp==-1){
		close(fd_value);
		return -1;
	}
	close(fd_value);
	len += tmp;

	return len;
}





int insert_pad1(int fd_dazibao){
	if(insert_type(fd_dazibao,0)==-1)
		return -1;
	return 1;
}





int insert_padN(int fd_dazibao, int length){

	int len=4+length;//pour connaitre la taille total du tlv

	if(insert_type(fd_dazibao,1)==-1)//TYPE
		return -1;
	if(insert_long(fd_dazibao,length)==-1)//LONGUEUR
		return -1;

	///INSERTION DE LA VALEUR/////////////////////////////////////////////////
	//écrire dans le dazibao d'une suite de zero de longueur lenght


	unsigned char buf[BUF_MAX_SIZE];
	memset(buf,0,BUF_MAX_SIZE);//remplir le buf avec des zeros
	int rc,size_W = BUF_MAX_SIZE;// size_W = taille de l'écriture sur le fichier

	while(length>0){
		
		if(length<BUF_MAX_SIZE){
			size_W = length;
			length = 0;
		}

		rc = write(fd_dazibao,buf,size_W);
		//vérification de rc :

		if(rc < 0) {
			perror("<write : padN rc<0 >") ;
			return -1;//erreur fichier rempli le champ error
		}
		if(rc != size_W){
			perror("<write : padN rc != size_W >") ;
			return -1;//erreur fichier rempli le champ error
		}
		length -= rc;

	}
	///////////////////////////////////////////////////////////////////////////

	return len;	
}


int insert_compound(int fd_dazibao){

	if(insert_type(fd_dazibao,5)==-1)
		return -1;

	off_t pos_len = lseek(fd_dazibao, 0, SEEK_END);//mémoriser la position dans le fd_dazibao pour écrire la longeur plus tard	
	//insertion bidon :
	if(insert_long(fd_dazibao,-1)==-1)
		return -1; 

	int len = 0,tmp;
	//char type[2];//ancienne version
	//type[0]= 'o';//ancienne version

	char saisie[T_SAISIE];//new
	saisie[0]='n';
	saisie[1]='\n';
	
	//while(type[0]!='n'){//ancienne version
	while(strcmp(saisie,"n")!=0){//new
		printf("compound : Continuer à ajouter au compound? <n/o> \n");
		//saisie_utilisateur(type);//ancienne version
		saisie_utilisateur(saisie);//new

		//if(type[0]!='n'){//ancienne version
		if(strcmp(saisie,"n")!=0){//new
			printf("compound : ");
			tmp = insert_tlv(fd_dazibao,VERBOS);
			if(tmp == -1){printf("fichier dazibao corrompu\n"); return -1;}//le fichier dazibao est corrompu l'insertion c'est mal déroulé
			else len += tmp;
		}
	}
	
	//ecriture de la longueur
	lseek(fd_dazibao,pos_len,SEEK_SET);//repositionner la tete d'écriture dans le fichier

	if(insert_long(fd_dazibao,len)==-1)// 4 = taille de du dated
		return -1;

	lseek(fd_dazibao,0,SEEK_END);//repositionner le curseur à la fin du dazibao


	
	return len+4;
}

//inserer la date au moment de l'appel de la fonction

int insert_dated(int fd_dazibao){


	//insertion du type 6 DATED :
	if(insert_type(fd_dazibao,6)==-1)
		return -1;
	

	off_t pos_len = lseek(fd_dazibao, 0, SEEK_CUR);//mémoriser la position dans le fd_dazibao pour écrire la longeur plus tard
	//insertion bidon :
	if(insert_long(fd_dazibao,0)==-1)
		return -1;


	//insertion date :
	int seconds = time(NULL);//récupération de la date
	unsigned char sec[4];
	date_bin(seconds, sec);
	
	if(write(fd_dazibao,sec,4)==-1){
	   perror("<write : date>");
	   return -1;
	}
	
	
	printf("Quel est le sous-tlv que vous voulez rentrer?\n");
	int len2 = insert_tlv(fd_dazibao,NO_VERBOS);


	//ecriture de la longueur
	lseek(fd_dazibao,pos_len,SEEK_SET);//repositionner la tete d'écriture dans le fichier
	if(insert_long(fd_dazibao,len2+4)==-1)// 4 = taille de du dated
		return -1;
	

	lseek(fd_dazibao,0,SEEK_END);//repositionner le curseur à la fin du dazibao

	return len2 + 4 + 4;//taille total écrite dans le dazibao
}




int insert_tlv(int fd_dazibao,int option){

	if(option) printf("Quel type de tlv vous voulez entrer?\n");
	int type = saisie_chiffre();

	int retour = -1;//pour mémoriser le retour de la fonction
	char saisie[T_SAISIE];//dans le cas d'une saisie de l'utilisateur

	// On pose un verrou exclusif ici pour l'écriture:
	if(flock(fd_dazibao, LOCK_EX) == -1) {
		perror("<flock : insert_tlv>");
		return -1;
	}

	lseek(fd_dazibao, 0, SEEK_END);//on se met à la fin du fichier



	switch(type){
		case 0 :	printf("pad1 :\n");
				retour = insert_pad1(fd_dazibao);
				break;


		case 1 : 	printf("padN : Choisir taille du padN :\n");
				int len = saisie_chiffre();
				retour = insert_padN(fd_dazibao,len);
				break;


		case 2 : 	printf("txt : <entrer un chemin vers le fichier>\n\t");
				saisie_utilisateur(saisie);
				retour = insert_TXT(fd_dazibao,saisie); 
				break;

		case 3 : 	printf("png : <entrer un chemin vers le fichier>\n\t");
				saisie_utilisateur(saisie);
				retour = insert_image_PNG(fd_dazibao,saisie); 
				break;

		case 4 : 	printf("jpeg : <entrer un chemin vers le fichier>\n\t");
				saisie_utilisateur(saisie);
				retour = insert_image_JPEG(fd_dazibao,saisie);
				break;


		case 5 : 	printf("compound :\n");
				retour = insert_compound(fd_dazibao); 
				break;


		case 6 : 	printf("dated : \n");
				retour = insert_dated(fd_dazibao);
				break;



		
		default :  	printf("le tlv %d n'est pas pris n charge par le programme\n",type);
				retour = -1;
				break;//le tlv est inconnu gérer ce cas 

	}

	//libération du verrou :
	if(flock(fd_dazibao, LOCK_UN) == -1) {
	    perror("<flock : insert_tlv>");
	    return -1;
	}

	return retour;
}


int insert_TXT(int fd_dazibao, char* file_path){
	//test pour savoir si c'est un fichier txt utf 8:
	int test = fichier_valide(file_path,"text");
	int len;

	switch(test){
		case  1 :	//le fichier est valide (on peux l'insérer):
				len = insert_tlv_generic(fd_dazibao,file_path,2);
				if(len==-1)
					return -1;
				return len;
				break;

		case  -3 :	//la commande file n'est pas installé
				printf("la commande <file> n'est pas installée\n");
				//on demande à l'utilisateur si il veux inserer tout de même le fichier
				printf("Voulez-vous tout de même inserer? oui = 1\n");
				int rep = saisie_chiffre();
				if(rep!=1) return -1;
				len = insert_tlv_generic(fd_dazibao,file_path,3);
				if(len==-1)
					return -1;
				return len;
				break;

		case -2 : 	printf("le fichier \"%s\" n'existe pas\n",file_path);
				return -1;
				break;

		case  0 :	printf("\tle fichier n'est pas de type text codé en UTF-8 : insertion de \"%s\" refusé\n",file_path);
				return -1;
				break;

		default :	printf("\terreur : insertion de \"%s\" refusé\n",file_path);
				return -1;
				break;	
	}
	return -1;//impossible à atteindre
}

int insert_image_PNG(int fd_dazibao, char* file_path){

	//test pour savoir si c'est une image PNG :
	int test = fichier_valide(file_path,"png");
	int len;
	
	switch(test){
		case  1 :	//le fichier est valide (on peux l'insérer):
				len = insert_tlv_generic(fd_dazibao,file_path,3);
				if(len==-1)
					return -1;
				return len;
				break;

		case  -3 :	//la commande file n'est pas installé
				printf("la commande <file> n'est pas installée\n");
				//on demande à l'utilisateur si il veux inserer tout de même le fichier
				printf("Voulez-vous tout de même inserer? oui = 1\n");
				int rep = saisie_chiffre();
				if(rep!=1) return -1;
				len = insert_tlv_generic(fd_dazibao,file_path,3);
				if(len==-1)
					return -1;
				return len;
				break;

		case -2 : 	printf("le fichier \"%s\" n'existe pas\n",file_path);
				return -1;
				break;

		case  0 :	printf("\tle fichier n'est pas du type png : insertion de \"%s\" refusé\n",file_path);
				return -1;
				break;

		default :	printf("\terreur : insertion de \"%s\" refusé\n",file_path);
				return -1;
				break;	
	}
	return -1;//impossible à atteindre
}

int insert_image_JPEG(int fd_dazibao, char* file_path){
	int test = -9,test_tmp;
	int len;
	//test pour savoir si c'est une image JPEG :
	if(((test_tmp = fichier_valide(file_path,"jpeg"))==1) || (fichier_valide(file_path,"jpg")==1)){//patherne "jpg" et "jpeg" trouvé
		test = 1;
	}else{ 
		test = test_tmp;
	}

	switch(test){
		case 1 :	//le fichier est valide (on peux l'insérer):
				len = insert_tlv_generic(fd_dazibao,file_path,4);
				if(len==-1)
					return -1;
				return len;
				break;

		case  -3 :	//la commande file n'est pas installé
				printf("la commande <file> n'est pas installée\n");
				//on demande à l'utilisateur si il veux inserer tout de même le fichier
				printf("Voulez-vous tout de même inserer? oui = 1\n");
				int rep = saisie_chiffre();
				if(rep!=1) return -1;
				len = insert_tlv_generic(fd_dazibao,file_path,3);
				if(len==-1)
					return -1;
				return len;
				break;

		case -2 : 	printf("le fichier \"%s\" n'existe pas\n",file_path);
				return -1;
				break;

		case  0 :	printf("\tle fichier n'est pas du type jpeg/jpg : insertion de \"%s\" refusé\n",file_path);
				return -1;
				break;

		default :	printf("\terreur : insertion de \"%s\" refusé\n",file_path);
				return -1;
				break;

	}
	return -1;//impossible à atteindre
}




//prend en paramètre un chemin de fichier et une chaine.
//et retourne vrai si le fichier est du type passé en paramètre.
//retourne 0 pas trouvé
//retourne 1 trouvé
//retourne -1 erreur pipe
//retourne -2 la file_paht n'existe pas ou il y a une érreur
//retourne -3 la commande file n'est pas installé sur la machine ou autre érreur

int fichier_valide(char* file_path,char* type){

	int link[2];//pipe fils vers pere
	int pid;
	char mess[128];//message de retour du fils
	memset(mess,0,128);

	//vérifier que le fichier existe :
	struct stat   buffer;   
  	if(stat (file_path, &buffer) != 0){
		return -2;
	}
	//ouvrir le pipe :
	if(pipe(link) == -1){
		perror("<pipe : fichier_valide>");
		return -1;
	}
	//faire un processus fils (pour exécuter la commande <file>):
	if((pid = fork()) == -1){
		perror("<fork : fichier_valide>");
		return -1;
	}

	if(pid){//pere
		close(link[1]);
		//ici faire boucle while pour recopier le contenu se qui a été ecrit sur le pipe (meme si normalement ça marche sans boucle) !!!!!!!!!!!!!!!!!!!!!!!
		//attendre la mort du fils :
		int status;//on ne l'utilisera que en cas de problème
		if(wait(&status)==-1){
			perror("<wait : fichier_valide>");
			return -1;
		}
		read(link[0],mess,sizeof(mess));

		//si la commande <file> n'existe pas
		//la console répond command not found
		if(strstr(mess,"command not found")){
			return -3;
		}
		
			
		if(strstr(type,"text")){
			if(strstr(mess,"text")){
				if(strstr(mess,"us-ascii")){// tout caract�re ASCII se code en UTF-8 sous forme d'un unique octet, identique au code ASCII
					return  1;
				}
				else if(strstr(mess,"utf-8")){
					return  1;
				}
				else {
					return 0;
				}
			}
			else {
				return 0;
			}
		}
	
		if(strstr(mess,type)){
			return 1;//trouvé
		}else{
			return 0;//pas trouvé
		}
		
	}else{//fils
		close(link[0]);
		dup2(link[1],STDOUT_FILENO);//rediriger la sortie du fils vers le pipe
	        execl("/usr/bin/file", "file", "-bi", file_path, (char *) 0);
	}

	return 0;
}


