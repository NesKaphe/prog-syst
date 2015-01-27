#ifndef ECRITURE_DAZIBAO_H
#define ECRITURE_DAZIBAO_H

#define BUF_MAX_SIZE 4096
#define NO_VERBOS 0
#define VERBOS 1

#include "saisie_utilisateur.h"
#include "operations_binaire.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/file.h>



/*============================================================================
int insert_entete (int fd_dazibao)
==============================================================================
Prend un fichier dazibao (fd_dazibao) et écrit l'entete d'un dazibao.
Le fichier doit être vide.
Retourne la taille en octet de ce qui a été écrit.
Retourne -1 en cas d'erreur.
============================================================================*/
int insert_entete(int fd_dazibao);




/*============================================================================
int insert_type	(int fd_dazibao,unsigned char type)
==============================================================================
Prend un fichier dazibao (fd_dazibao) et le numéros de tlv (type)
Cette fonction écrit le type dans le fichier dazibao.
Le fichier doit avoir l'entete d'un dazibao.
Retourne la taille en octet de ce qui a été écrit.
Retourne -1 en cas d'erreur.
============================================================================*/
int insert_type(int fd_dazibao,unsigned char type);




/*============================================================================
int insert_long(int fd_dazibao,int decimal_length)
==============================================================================
Prend un fichier dazibao (fd_dazibao) et une longeur décimal (decimal_length).
Cette fonction ecrit la longueur dans le fichier dazibao.
Le fichier doit avoir l'entete d'un dazibao.
Retourne la taille en octet de ce qui a été écrit.
Retourne -1 en cas d'erreur.
============================================================================*/
int insert_long	(int fd_dazibao,int decimal_length);




/*============================================================================
int insert_value(int fd_dazibao,int fd_value)
==============================================================================
Prend un fichier dazibao (fd_dazibao) et un fichier valeur (fd_value)
Cette fonction écrit le contenu du fichier valeur dans le fichier dazibao.
Le fichier doit avoir l'entete d'un dazibao.
Retourne la taille en octet de ce qui a été écrit.
Retourne -1 en cas d'erreur.
============================================================================*/
int insert_value(int fd_dazibao,int fd_value);




/*============================================================================
int insert_tlv(int fd_dazibao,int option)
==============================================================================
Prend un fichier dazibao (fd_dazibao) et une option.
Cette fonction est une "interface" qui permet à l'utilisateur d'insérer un
 nouveau tlv a partir de son numéros."insert_tlv" fait appelle aux fonctions 
correspondantes. Exemple si je fait 2 (tvl txt)
la fonction insert_TXT sera appelé.
Si l'option est VERBOS on affiche un message qui intérroge l'utilisateur dans
le cas contraire on écrit NO_VERBOS.

Le fichier doit avoir l'entete d'un dazibao.
Retourne la taille en octet de ce qui a été écrit.
Retourne -1 en cas d'erreur.
============================================================================*/
int insert_tlv(int fd_dazibao,int option);




/*============================================================================
int taille_fichier(int fd_fichier)
==============================================================================
Retourne la taille du fichier (fd_fichier) en paramètre.
Retourne -1 en cas d'erreur.
============================================================================*/
int taille_fichier(int fd_fichier);



/*============================================================================
int insert_tlv_generic	(int fd_dazibao,char* file_path,unsigned char type)
==============================================================================
Prend en paramètre un fichier dazibao (fd_dazibao), le chemin vers le
fichier (file_path), et le numéros de tlv (type)
Insertion d'un tvl simple qui suit le schémas suivant Type, Longueur, Valeur.
Exemple texte, image jpg, image png ou autres nouveauté (pdf,docx, ...)
Retourne la taille en octet de ce qui a été écrit.
Retourne -1 en cas d'erreur.
============================================================================*/
int insert_tlv_generic	(int fd_dazibao,char* file_path,unsigned char type);




/*============================================================================
int insert_pad1(int fd_dazibao)
==============================================================================
Insertion d'un pad1.
Retourne la taille en octet de ce qui a été écrit (1 octet).
Retourne -1 en cas d'erreur.
============================================================================*/
int insert_pad1(int fd_dazibao);





/*============================================================================
int insert_padN(int fd_dazibao, int length)
==============================================================================
Insertion d'un padN d'une longueur "lenght".
Retourne la taille en octet de ce qui a été écrit.
Retourne -1 en cas d'erreur.
============================================================================*/
int insert_padN(int fd_dazibao, int length);




/*============================================================================
int insert_TXT(int fd_dazibao, char* file_path)
==============================================================================
Prend en paramètre un fichier dazibao (fd_dazibao), le chemin vers le
fichier texte (file_path).
Insertion d'un texte dans un fichier dazibao. Cette fonction empèche 
l'insertion d'un autre type de fichier qu'un texte respectant le format UTF8.
Retourne la taille en octet de ce qui a été écrit.
Retourne -1 en cas d'erreur.
============================================================================*/
int insert_TXT(int fd_dazibao, char* file_path);


/*============================================================================
int insert_image_PNG(int fd_dazibao, char* file_path)
==============================================================================
Prend en paramètre un fichier dazibao (fd_dazibao), le chemin vers le
fichier image png (file_path).
Insertion d'une image png dans un fichier dazibao. Cette fonction empèche 
l'insertion d'un autre type de fichier que le format png.
Retourne la taille en octet de ce qui a été écrit.
Retourne -1 en cas d'erreur.
============================================================================*/
int insert_image_PNG(int fd_dazibao, char* file_path);


/*============================================================================
int insert_image_JPEG(int fd_dazibao, char* file_path)
==============================================================================
Prend en paramètre un fichier dazibao (fd_dazibao), le chemin vers le
fichier image png (file_path).
Insertion d'une image jpg/jpeg dans un fichier dazibao. Cette fonction empèche 
l'insertion d'un autre type de fichier que le format jpg/jpeg.
Retourne la taille en octet de ce qui a été écrit.
Retourne -1 en cas d'erreur.
============================================================================*/
int insert_image_JPEG(int fd_dazibao, char* file_path);



/*============================================================================
int fichier_valide(char* file_path,char* type)
==============================================================================
Prend en paramètre le chemin vers un fichier (file_path), et un type de 
fichier (type). Cette fonction utilise la commande file pour déterminé si le
fichier passé en paramètre correspond au type.
Retourne 0 pas trouvé
Retourne 1 trouvé
Retourne -1 erreur pipe
Retourne -2 le file_path n'existe pas ou il y a une érreur
Retourne -3 la commande file n'est pas installé sur la machine ou autre erreur
============================================================================*/
int fichier_valide(char* file_path,char* type);


/*============================================================================
int utf8(char *file_path)
==============================================================================
test pour savoir si c'est un fichier est au format utf8
Renvoi 1 si le fichier est codé en utf8
Renvoi 0 si le fichier n'est pas codé en utf8
============================================================================*/
int utf8(char *file_path);

#endif
