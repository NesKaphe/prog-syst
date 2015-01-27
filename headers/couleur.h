
#ifndef COULEUR_H
#define COULEUR_H


#include <stdio.h>
// Efface l'écran et replace le curseur en haut à gauche
#define clrscr() printf("\033[H\033[2J")


#define BLACK    "\033[0;30m"
#define RED      "\033[0;31m"
#define GREEN    "\033[0;32m"
#define YELLOW   "\033[0;33m"
#define BLUE     "\033[0;34m"
#define PURPLE   "\033[0;35m"
#define CYAN     "\033[0;36m"
#define GREY     "\033[0;37m"

#endif

#define DEFAULT_COLOR "\033[0;m"
