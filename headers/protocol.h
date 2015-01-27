#include <sys/un.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>


//protocole de communication entre le client et le serveur :

#define SRV_SOCK_PATH "home/.dazibao-notification-socket"//répertoir ou sera génré la socket de communication (pas utilisé)
#define HOME getenv("HOME") //pour trouvé ou ce trouve le repertoire "home"
#define SRV_SOCK_NAME "/.dazibao-notification-socket"//nom de la socket
#define BUFFSIZE 1024//taille de la socket
#define SRV_BACKLOG 10//nombre de client connecté au serveur

#define MSG_CLIENT %s //%d = pid du client %s = message (pas utilisé)
