#ifndef CLIENT_H
#define CLIENT_H

#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>

#include "dazibao.h"

#define SUIVI_MAX 100

pthread_t pthread;


 /* ============================================================================================
void thread_notifications_main(void *arg)
===============================================================================================
"Main" du thread de notifications
=============================================================================================== */
void *thread_notifications_main(void *arg);




  /* ============================================================================================
int client_connect(int verbos)
=================================================================================================
Initialise une socket et se connecte au serveur de notifications

Parametre : Verbos permet d'afficher ou non les erreurs rencontrées sur la sortie standard

Retourne serveurFD le descripteur de socket en cas de succes
Retourne -1 en cas d'erreur (serveur impossible à joindre)
=============================================================================================== */
int client_connect(int verbos);




  /* ============================================================================================
int client_lecture(notifiacations notif)
=================================================================================================
Va attendre des notifications venant du serveur et va les afficher sur la sortie standard
=============================================================================================== */
int client_lecture(notifications notif);




  /* ============================================================================================
int client_ecriture(dazibao *dazi)
=================================================================================================
Va notififier le serveur d'une modification du dazibao passé en parametre

Retourne 1 en cas de reussite
Retourne -1 en cas d'échec
=============================================================================================== */
int client_ecriture(dazibao *dazi);




  /* ============================================================================================
int client(notifications *notif, int verbos)
=================================================================================================
Va se connecter à la socket et lancer le thread de notifications, verbos est utilisé pour afficher
les erreurs rencontrées sur la sortie standard

Retourne 1 en cas de reussite
Retourne -1 en cas d'erreur système
=============================================================================================== */
int client(notifications *notif, int verbos);

#endif
