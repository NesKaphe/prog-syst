#include "client.h"
#include "protocol.h"
#include "dazibao.h"


 /* ============================================================================================
void thread_notifications_main(void *arg)
===============================================================================================
"Main" du thread de notifications
=============================================================================================== */
void *thread_notifications_main(void * arg) {
    notifications *notif = (notifications *) arg;

    client_lecture(*notif); //On lance la lecture de socket si la condition est remplie

    return NULL;
}



  /* ============================================================================================
int client_connect(int verbos)
=================================================================================================
Initialise une socket et se connecte au serveur de notifications

Parametre : Verbos permet d'afficher ou non les erreurs rencontrées sur la sortie standard

Retourne serveurFD le descripteur de socket en cas de succes
Retourne -1 en cas d'erreur (serveur impossible à joindre)
=============================================================================================== */
int client_connect(int verbos) {

    struct sockaddr_un sun;
    int serveurFD;

    if((serveurFD = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        if(verbos)
	    perror("socket : <client_connect>");
	return -1;
    }

    memset(&sun, 0, sizeof(struct sockaddr_un));
    sun.sun_family = AF_UNIX;
    //strncpy(sun.sun_path, SRV_SOCK_PATH, sizeof(sun.sun_path) - 1);
    strncat(sun.sun_path, HOME, sizeof(sun.sun_path) -1);
    strncat(sun.sun_path, SRV_SOCK_NAME, sizeof(sun.sun_path) -1);

    if(connect(serveurFD, (struct sockaddr *) &sun, sizeof(struct sockaddr_un)) < 0) {
        if(verbos)
	    perror("connect : <client_connect>");
        return -1;
    }

    return serveurFD;
}




  /* ============================================================================================
int client_lecture(notifiacations notif)
=================================================================================================
Va attendre des notifications venant du serveur et va les afficher sur la sortie standard
=============================================================================================== */
int client_lecture(notifications notif) {
    char buf[BUFFSIZE];
    char *p,*p2;
    int octetsLu;
    int notifications = 0;
    int i;

    while(1) {
	octetsLu = read(notif.sockFD, buf, BUFFSIZE); //On enregistre a la suite de ce qui existe deja dans le buffer
	if(octetsLu < 0) {
	    if(errno == EINTR) {
		continue;
	    }
	    perror("<read : client_lecture>");
	    return -1;
	}
	if(octetsLu == 0) {
	    break;
	}

	p = memchr(buf, '\n', sizeof(buf));
	if(p == NULL) {
	    fprintf(stderr, "Notification trop longue \n");
	    return -1;
	}
	
	if(buf[0] == 'C') {
	    for(i = 0; i < notif.nb_dazibao; i++) {
		p2 = strstr(buf, notif.dazibao_a_suivre[i]);
		if(p2 != NULL && p2[strlen(notif.dazibao_a_suivre[i])+1] == '\0') {
		    printf("Dazibao changé %s\n", buf);
		    notifications++;
		}
	    }
	}
	else {
	    fprintf(stderr, "Notification de type inconnu : %c\n", buf[0]);
	}
    }

    return notifications;
}




  /* ============================================================================================
int client_ecriture(dazibao *dazi)
=================================================================================================
Va notififier le serveur d'une modification du dazibao passé en parametre

Retourne 1 en cas de reussite
Retourne -1 en cas d'échec
=============================================================================================== */
int client_ecriture(dazibao *dazi) {
    char buf[BUFFSIZE] = {'\0'};

    buf[0] = 'C';
    if(dazi->chemin[0] != '/') {
	buf[1] = '/';
    }
    strncat(buf,dazi->chemin,BUFFSIZE);
    strncat(buf,"\n",BUFFSIZE);
    if(dazi->notif->connecte == 0) {
	if(client(dazi->notif, 0)==-1) {
	    return -1;
	}
    }

    if(write(dazi->notif->sockFD, buf, strlen(buf)) < 0) {
	if(errno == EPIPE) {
	    fprintf(stderr, "Err : Communication avec le serveur de notifications impossible\n");
	    dazi->notif->connecte = 0; //On se marque comme deconnecté pour ne plus communiquer
	    //Avec le serveur jusqu'a la prochaine reconnexion
	    return -1;
	}
	else {
	    perror("<write : client_ecriture>");
	    return -1;
	}
    }

    return 1;
}




  /* ============================================================================================
int client(notifications *notif, int verbos)
=================================================================================================
Va se connecter à la socket et lancer le thread de notifications, verbos est utilisé pour afficher
les erreurs rencontrées sur la sortie standard

Retourne 1 en cas de reussite
Retourne -1 en cas d'erreur système
=============================================================================================== */
int client(notifications *notif, int verbos) {

    int s;
    
    s = client_connect(verbos);

    if(s == -1) {
	if(verbos) {
	    printf("Impossible de se connecter au serveur de notifications\n");
	    printf("Réessayez plus tard.\n");
	}
	return -1;
    }

    notif->sockFD = s;
    notif->connecte = 1;

    s = pthread_create(&pthread, NULL, thread_notifications_main, (void *) notif);
    if(s != 0) {
	perror("pthread_create : <client>");
	return -1;
    }

    return 1;
}


