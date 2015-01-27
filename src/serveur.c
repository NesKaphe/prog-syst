#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>


#include "protocol.h"

static pthread_cond_t  cond_msg = PTHREAD_COND_INITIALIZER ; //condiction qui est signalée quand le message est à notifier aux différents clients connectés
static pthread_mutex_t mtx_cond = PTHREAD_MUTEX_INITIALIZER; //le thread "notification_client" à besoin d'un mutex lui appartenant pour recevoir la condition c'est à cause de la fonction "pthread_cond_wait"

static pthread_mutex_t mtx_msg = PTHREAD_MUTEX_INITIALIZER ; //mutex du pour acceder à la variable globale message


int tab_client[SRV_BACKLOG];//vas servir au thread qui va notifier les différents clients


char message[BUFFSIZE];//le message est en globale car on ne veux pas passé le message en paramètre du thread "message_client"




/*============================================================================
void* notification_client()
==============================================================================
fonction thread qui va écrire sur la socket de chaque clients le message de 
notification.
==============================================================================*/
void* notification_client(){
	int s;
	printf("SERVEUR - <thread notification client> début\n");
	for(;;){
		printf("SERVEUR - <thread notification client> début boucle\n");
		//prise de possetion du verrou mtx_cond :
		s = pthread_mutex_lock (&mtx_cond ) ;
		if ( s != 0){
			perror("<serveur : lock>");
			return NULL;
		}

		//on attend qu'un message soit écrit :
		s = pthread_cond_wait (&cond_msg , &mtx_cond) ;
		if ( s != 0){
			perror("<serveur : cond_wait>");
			//return NULL;
		}
		printf("SERVEUR - <thread notification client> la condition est remplit\n");


		int i,bytes;
		struct stat info;
		for(i=0 ; i < SRV_BACKLOG ; i++){//on parcours tout le tableau de clients
			printf("tab_client[%d]=%d",i,tab_client[i]);
			//on verifie que le client est toujour présent :
		  	if(fstat(tab_client[i], &info) == 0){
			//if(tab_client[i] != -1){
				printf(" ++ Ce client est OK");
				bytes = write(tab_client[i] , message , BUFFSIZE );

				if (bytes < 0){
					if(errno != EPIPE){					
						perror("<serveur : write>") ;
						return NULL;
					}
					printf("problème write avec le client %d",tab_client[i]);
					//on replace le fd du client par -1 car il n'est plus connecté :
					tab_client[i]=-1;
					//break;//signal EPIPE reçu le client c'est déconnecté avant de la fin de l'écriture
				}


			}else{
				//on replace le fd du client par -1 car il n'est plus connecté :
				//(2eme cas c'était déjà un -1)
				tab_client[i]=-1;
			}
			printf("\n");//SERVEUR
		}

		//libération du vérou mtx_cond :
		s = pthread_mutex_unlock (&mtx_cond ) ;
		if ( s != 0){
			perror("<serveur : unlock>");
			return NULL;
		}

		//libération du vérou mtx_msg :
		s = pthread_mutex_unlock (&mtx_msg ) ;
		if ( s != 0){
			perror("<serveur : unlock>");
			return NULL;
		}

	}

	return NULL;
}









/*============================================================================
void* attendre_client(void* arg)
==============================================================================
fonction thread qui va lire les messages en provenance du client
==============================================================================*/
void* attendre_client(void* arg){

	printf("SERVEUR - <thread attendre_client> debut\n");

	int* pt_client = ((int*)arg);
	int fd_client = *pt_client;
	
	printf("SERVEUR - <thread attendre_client> fd_client =%d\n",fd_client);
	
	char buf[BUFFSIZE];//le message
	
	

	int bytes,s;
	//boucle d'attente du client :
	for(;;){
		memset(&buf, 0, BUFFSIZE);//mettre 0 à la place des messages

		//lecture du message reçu :
		bytes = read(fd_client,buf,BUFFSIZE);

		if (bytes < 0){

			printf("\tSERVEUR - <thread attendre_client>  dans la boucle bytes< 0\n"); 
			if(errno != EPIPE){					
				perror("<serveur : read>") ;
				return NULL;
			}
			break;//signal EPIPE reçu le client c'est déconnecté avant de la fin de la lecture
		}

		//quand le client meur le buf reçoit un EOF
		if(buf[0] == 0){
			int i;
			//on supprime le fd du client dans le tableau de clients
			for(i=0 ; i < SRV_BACKLOG ; i++){//on parcours tout le tableau de clients
				if(tab_client[i]==fd_client){
					tab_client[i] = -1;
					break;
				}
			}
			return NULL;//stopper le thread
		}
		printf("SERVEUR - <thread attendre_client> client %d à envoyé le message suivant :\t%s \n",fd_client,buf);


		//prendre le mutexe : (si il y a une notification en cour on attend que le mutex soit libéré)
				
		s = pthread_mutex_lock (&mtx_msg ) ;
		if ( s != 0){
			perror("<serveur : lock>");
			return NULL;
		}
		
		printf("SERVEUR - <thread attendre_client> j'ai pris le mutexe\n");
		
		//copier la buffer dans message :
		strncpy(message , buf , BUFFSIZE -1);

		//signaler que la condition est remplit (le relai sera pris par le thread notification client):
		s = pthread_cond_signal (&cond_msg ) ;
		if ( s != 0){
			perror("<serveur : unlock>");
			return NULL;
		}
		printf("SERVEUR - <thread attendre_client> j'ai signalé la condition\n");
		
	}
	return NULL;
}


void interseption_SIGPIPE(){

	printf("\n\n\tSIGPIPE\n\n");//message pour l'utilisateur
	
}




int main()//(int argc, char** argv) //mis en commentaire pour retirer les messages de compilation "unused parametre"
{

	//PARTIE INITIALISATION ==================================================

	
	//interception de signaux
	struct sigaction siga;
	memset(&siga,0,sizeof(siga));//remplit la structure sigaction 0

	 

	siga.sa_handler=&interseption_SIGPIPE;//pas de traitement spécifique
	sigaction(SIGPIPE,&siga,NULL);






	
	//initialisation du tableau de fd_clients :

	memset(&tab_client, -1 , SRV_BACKLOG*sizeof(int));

	int s = pthread_cond_signal (& cond_msg ) ;
	if ( s != 0){
		perror("<serveur : cond_signal>");
		return -1;
	}

	struct sockaddr_un addr;//le socket
	int srv_fd , cli_fd;//descriteur de fichier client et serveur


	//création ou connexion à la socket :
	if((srv_fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0){
		perror("<serveur : socket>");
		return -1;
	}

	//initialisation de la socket :
	memset(&addr, 0, sizeof(struct sockaddr_un));
	addr.sun_family = AF_UNIX;


	//pour mettre le bon chemin vers le home :
	memset(&addr, 0, sizeof(struct sockaddr_un));
	addr.sun_family = AF_UNIX;
	strncat(addr.sun_path, HOME, sizeof(addr.sun_path) -1);
	strncat(addr.sun_path, SRV_SOCK_NAME, sizeof(addr.sun_path) -1);

	printf("\nSERVEUR - le fichier de la socket est %s\n",addr.sun_path);

	//access :
	if(access(addr.sun_path , F_OK ) == 0)
		unlink( addr.sun_path );
	//bind :
	if(bind(srv_fd ,(struct sockaddr *)&addr,sizeof(struct sockaddr_un)) < 0){
		perror("<seveur : bind>");
		return -1;
	}

	//listen :
	if (listen( srv_fd , SRV_BACKLOG ) < 0){
		perror("<serveur : listen>" );
		return -1;
	}


	//initialisation du thread d'écriture dans la socket :
	pthread_t th_msg_cli;
	s = pthread_create(&th_msg_cli,NULL,(void*)notification_client,NULL);
	if( s != 0){
		perror("<serveur : create th_msg_cli>");
	}
	
	printf("SERVEUR - debut du serveur\n");

	//BOUCLE DU SERVEUR ==========================================================

	for (;;) {//boucle infinit

		//accepter un nouveau client :
		if ( (cli_fd = accept ( srv_fd , NULL , NULL ) ) < 0){
			perror("<serveur : accept>") ;
			return -1;
		}
		printf("SERVEUR - <serveur> un nouveau client enregisté fd =%d\n",cli_fd);
		//ajouter un nouveau client au serveur :
		//attention on ne ferme pas les clients car on veux pouvoir les notifiers tous
		int i;		
		for(i = 0 ; i < SRV_BACKLOG ; i++ ){
			if(tab_client[i] == -1){//insertion du nouveau client
				printf("SERVEUR - <serveur> nouveu client %d en position %d\n",cli_fd,i); 
				tab_client[i] = cli_fd;
				break;
			}	
		}

		pthread_t th_att_cli;// !!! regarder si on nouveau thread est créer à chaque boucle ou non
		s = pthread_create(&th_att_cli,NULL,(void*)attendre_client,&cli_fd);
		if( s != 0){
			perror("<serveur : create th_wait_cli >");
			//return -1; //on ne stope pas le serveur !!!
		}	
		
		

	}
	


	//dans le cas ou l'on sort de la boucle :
	//pour attendre la fin du thread d'écriture dans la socket :
	void* res;
	s = pthread_join(th_msg_cli,&res);
	if( s != 0){
		perror("<serveur : join>");
		return -1;
	}	
	

	exit(EXIT_SUCCESS);
} 
