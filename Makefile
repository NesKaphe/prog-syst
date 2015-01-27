DEBUG = no
PATHSRC = src/
PATHHEADERS = headers/
PATHBIN = bin/
CC = gcc

ifeq ($(DEBUG), yes)
	CFLAGS = -g -W -Wall -I$(PATHHEADERS) -lm -lpthread
else
	CFLAGS = -W -Wall -I$(PATHHEADERS) -lm -lpthread
endif

EXEC = exe serveur
OBJDAZI = bin/saisie_utilisateur.o bin/operations_binaire.o bin/dazibao.o bin/tlv.o bin/ecriture.o bin/insertion.o bin/interface.o bin/lecture.o bin/client.o bin/dates.o main.o
OBJSERV = bin/serveur.o

all: $(EXEC)

exe: $(OBJDAZI)
	$(CC) -o $@ $^ $(CFLAGS)

serveur: $(OBJSERV)
	$(CC) -o $@ $^ $(CFLAGS)

main.o: main.c
	$(CC) -o $@ -c $< $(CFLAGS)

bin/%.o: src/%.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	@rm -rf bin/*.o main.o

mrproper:
	@rm -rf $(EXEC)
