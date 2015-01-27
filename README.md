Auteurs : Alain Dias, Sebastien Ciupek, Clement Charasson

**********************
Liste des fichiers inclus dans le projet
**********************

Makefile

main.c

README

dossier bin

dossier extract

dossier src contenant les fichiers suivants :
client.c		
dates.c		
dazibao.c		
ecriture.c
insertion.c
interface.c
lecture.c
operations_binaires.c
saisie_utilisateur.c
tlv.c

dossier headers contentant les fichiers suivants :
client.h
couleur.h
dates.h
dazibao.h
ecriture.h
insertion.h
interface.h
lecture.h
operations_binaires.h
protocol.h
saisie_utilisateur.h
tlv.h

**********************
Compilation
**********************

A l'aide du shell se placer dans le dossier où se situe le fichier Makefile

Ecrire make et taper sur Entrée

2 exécutables sont créés : exe et serveur

**********************
Commandes
**********************

Créer un dazibao ou ouvrir un dazibao existant :

./exe


Ouvrir un dazibao en paramètre :

./exe -o <nom_fichier>

où <nom_fichier> est le chemin du dazibao


Etre notifié des modifications apportées aux Dazibaos d'une liste de Dazibaos :

Lancer le serveur avec la commande :

./serveur

Indiquer le(s) dazibao(s) que l'on veut suivre avec la commande:

./exe -s <nom_fichier1> <nom_fichier2>...<nom_fichiern>

où <nom_fichier1> <nom_fichier2>...<nom_fichiern> sont les chemins des dazibaos que l'on veut suivre chacun séparés par un espace.


Les options peuvent être utilisées en même temps:

Exemple :
Ouvrir le dazibao1 et être notifié des modifications apportées aux Dazibaos dazibao2 et dazibao3

La commande est :

./exe -o dazibao1 -s dazibao2 dazibao3


Pour valider un choix dans le programme il faut taper sur Entrée

**********************
Ouvrir ou créer un dazibao
**********************
Dans le menu, écrire le chiffre situé à gauche de l'action que vous voulez faire.

	1.terminer
	2.Creer un dazibao
	3.Ouvrir un dazibao existant

----------------------
1.terminer
----------------------
Cette action permet de quitter le programme.

----------------------
2.Creer un dazibao
----------------------
Cette action permet de créer un dazibao.
Ecrire le chemin du dazibao que vous vous voulez créer, le dazibao est créé dans le chemin indiqué.
Si le dazibao existe déjà, il est demandé si on veut l'écraser, écrire o pour l'écraser ou n pour ne pas l'écraser (dans ce cas aucun dazibao n'est créé).

----------------------
3.Ouvrir un dazibao existant
----------------------
Cette action permet d'ouvrir un dazibao existant.
Ecrire le chemin du dazibao existant.


**********************
Manipuler un dazibao
**********************

Dans le menu de manipulation du dazibao, écrire le chiffre situé à gauche de l'action que vous voulez effectuer.
	
	1.Terminer
	2.Ajouter des tlv au dazibao
	3.Supprimer des tlv au dazibao " nom_dazibao "
	4.Afficher le contenu de " nom_dazibao "
	5.Ouvrir/Creer un autre dazibao
	6.Faire la compaction manuellement de " nom_dazibao "
	7.Afficher un contenu d'un tlv dans " nom_dazibao "
	8.extraction tlv ..


----------------------
1.Terminer
----------------------

Cette action permet de quitter le programme.

----------------------
2.Ajouter des tlv au dazibao
----------------------

Cette action permet d'ajouter un tlv parmi les types suivant au dazibao :

	0 : pad1
	1 : padn
	2 : txt
	3 : png
	4 : jpeg
	5 : compound
	6 : dated

Pour ajouter un tlv : écrire le chiffre situé à gauche du type de tlv que vous voulez ajouter au dazibao.

Pour le cas d'un padN : écrire en chiffre la taille du padN voulu.

Pour le cas d'un txt, d'un png ou d'un jpeg : écrire le chemin du fichier que l'on veut ajouter. Le programme vérifie à l'aide la commande "file" si le fichier que l'on veut insérer est du même type que le tlv indiqué, si ce n'est pas le cas le tlv n'est pas créer. Si la commande "file" n'est pas installée on peut forcer la création du tlv.

Pour le cas d'un compound : avant d'ajouter un tlv, on demande si on veut continuer en tapant o pour continuer à en ajouter ou n pour ne pas continuer à en ajouter.

Pour le cas d'un dated : écrire le numéro du type de tlv que l'on veut insérer dans le dated.

----------------------
3.Supprimer des tlv au dazibao " nom_dazibao "
----------------------

Pour supprimer un tlv de type txt, png ou jpeg qui n'est pas contenu dans un autre tlv : écrire l'indice à situé à gauche du tlv que vous voulez supprimer.

Pour supprimer un tlv de type compound ou dated écrire l'indice situé à gauche du tlv que vous voulez supprimer.
Ecrire 1 pour Effacer l'intégralité du tlv

Pour supprimer un tlv situé dans un tlv de type compound ou dated :
Ecrire l'indice de son plus grand parent et descendre dans l'arbre pour arriver au tlv voulu.

Exemple :
Pour supprimer "(2) Message texte" dans ce dazibao :

 (1) Compound 
     | (1) Compound 
     | | (1) Compound 
     | | | (1) Message texte 
     | | | (2) Message texte 

la suite de commandes est :

1 entrée  (pour choisir le plus grand parent)
2 entrée (Effacer un tlv a l'intérieur du compound)
1 entrée (sélectionner l'indice du tlv dans son tlv)
2 entrée (Effacer un tlv a l'intérieur du compound)
1 entrée (sélectionner l'indice du tlv dans son tlv)
2 entrée (Effacer un tlv a l'intérieur du compound)
2 entrée (sélectionner l'indice du tlv dans son tlv)

----------------------
4.Afficher le contenu de " nom_dazibao "
----------------------

Cette action affiche les tlv contenu dans le dazibao sous forme d'arbre, il n'affiche pas les pad1 et padn
Pour chaque tlv est affiché : son type

Si ce tlv n'est pas dans un compound ou ou un dated est affiché : son indice d'apparition dans le dazibao 
 
Si ce tlv est dans un compound ou ou un dated est affiché : son indice d'apparition dans le tlv dans lequel il situé

Si ce tlv est un compound ou ou un dated ne comportant pas de tlv : on affiche qu'il est vide

Si ce tlv est un dated est affiché : sa date 

----------------------
5.Ouvrir/Creer un autre dazibao
----------------------

Cette action permet d'ouvrir ou de créer un dazibao

----------------------
6.Faire la compaction manuellement de " nom_dazibao "
----------------------
Cette action supprime les pad1 et padN du dazibao.
Elle affiche la taille en octet du dazibao avant la compaction et la taille en octet du dazibao après la compaction.

----------------------
8.extraction tlv
----------------------

Cette action permet d'extraire le fichier d'un tlv de type texte, png, jpg ou de type inconnu.
Le fichier extrait est créer dans le dossier extract.
Son nom est de la forme <extract_nbsec.type> où nbsec représente l'heure unix en secondes au moment de l'extraction  et type représente l'extension du fichier lorsque son type est connu.
Si le tlv extrait est de type inconnu son nom est de la forme <extract_nbsec>.

Pour extraire un tlv qui n'est pas dans un compound ou un dated, il faut écrire l'indice situé à gauche du tlv et taper sur entrée, puis écrire 1  pour choisir l'extraction immédiate.

Pour extraire un tlv situé dans un dated ou un compound, il faut écrire l'indice de son plus grand parent et descendre dans l'arbre pour arriver au tlv voulu.

Exemple :
Extraire (1) Message texte situé sur la dernière ligne dans ce dazibao :

(1) Message texte 
(2) Compound 
     | (1) Message texte 
     | (2) Compound 
     | | (1) Message texte 
 
la suite de commandes est :

2 entrée (sélectionner le plus grand parent)
2 entrée (sélectionner l'indice du tlv dans son tlv)
1 entrée (sélectionner l'indice du tlv dans son tlv)
1 entrée (extraction immediate)
