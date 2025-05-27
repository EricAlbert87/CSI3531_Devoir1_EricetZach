/*------------------------------------------------------------
Fichier: cpr.c

Nom: Zachary Shewan et Eric Albert
Numero d'etudiant: 300273108 et 300353640

Description: Ce programme contient le code pour la creation
             d'un processus enfant et y attacher un tuyau.
	     L'enfant envoyera des messages par le tuyau
	     qui seront ensuite envoyes a la sortie standard.

Explication du processus zombie
(point 5 de "A completer" dans le devoir):

Un processus zombie est un programme dont l'exécution est terminée, mais qui reste visible dans le système parce que son 
« parent » (le programme qui l'a lancé) n'a pas encore récupéré les informations relatives à sa terminaison. Dans ce programme, 
les parents ne demandent pas immédiatement ce qu'il est arrivé à leurs enfants une fois qu'ils ont terminé. En conséquent, les 
enfants deviennent des zombies pendant un bref moment. Vous pouvez voir ces zombies avec la commande ps ou en regardant dans le dossier /proc.

Traduit avec DeepL.com (version gratuite)
	(s.v.p. completez cette partie);

-------------------------------------------------------------*/
#include <stdio.h>
#include <sys/select.h>

/* Prototype */
void creerEnfantEtLire(int );

/*-------------------------------------------------------------
Function: main
Arguments: 
	int ac	- nombre d'arguments de la commande
	char **av - tableau de pointeurs aux arguments de commande
Description:
	Extrait le nombre de processus a creer de la ligne de
	commande. Si une erreur a lieu, le processus termine.
	Appel creerEnfantEtLire pour creer un enfant, et lire
	les donnees de l'enfant.
-------------------------------------------------------------*/

int main(int ac, char **av)
{
    int numeroProcessus; 

    if(ac == 2)
    {
       if(sscanf(av[1],"%d",&numeroProcessus) == 1)
       {
           creerEnfantEtLire(numeroProcessus);
       }
       else fprintf(stderr,"Ne peut pas traduire argument\n");
    }
    else fprintf(stderr,"Arguments pas valide\n");
    return(0);
}


/*-------------------------------------------------------------
Function: creerEnfantEtLire
Arguments: 
	int prcNum - le numero de processus
Description:
	Cree l'enfant, en y passant prcNum-1. Utilise prcNum
	comme identificateur de ce processus. Aussi, lit les
	messages du bout de lecture du tuyau et l'envoie a 
	la sortie standard (df 1). Lorsqu'aucune donnee peut
	etre lue du tuyau, termine.
-------------------------------------------------------------*/

void creerEnfantEtLire(int prcNum)
{
    /* S.V.P. completez cette fonction selon les
       instructions du devoirs. */

	// Verification du nombre de processus
	if (prcNum <= 0){
		fprintf(stderr, "Le nombre de processus doit être positif.\n");
		exit(1);
	}
	else if (prcNum == 1){
		// Cas de base et fin de la recursion (assure que ceci est le dernier processus enfant)
        char msg[32];
        int len = snprintf(msg, sizeof(msg), "Processus 1 commence\n");
        write(1, msg, len);

        sleep(5);
        len = snprintf(msg, sizeof(msg), "Processus 1 termine\n");
        write(1, msg, len);
        sleep(10);
        return;
	}

	// Creation du tuyau
	int fd[2];
	if (pipe(fd) == -1) {
        perror("Pipe");
        exit(1);
    }

	// Creation du processus enfant
	int pid;
	pid = fork();
	if (pid<0){
		fprintf(stderr, "Erreur lors de la création de l'enfant");
		exit(-1);
	}
	else if (pid==0){
		// Enfant
		close(fd[0]); // close the read pipe

		// Rediriger la sortie standard vers le tuyau
		dup2(fd[1], 1);
		close(fd[1]); // close the original write pipe


		char msg[32];
        int len = snprintf(msg, sizeof(msg), "Processus %d commence\n", prcNum);
        write(1, msg, len);
		// il ne faut pas fermer le tuyau pour obtenir les messages dans la bonne ordre
		
		// Recursion (exécute l'enfant)
		char *args[] = {"./cpr", (snprintf(msg, sizeof(msg), "%d", prcNum - 1), msg), NULL};
        execvp("./cpr", args);

        // If execvp fails (en cas d'erreur)
		perror("execvp failed");
        exit(1);
	}
	else {
		// Parent
		close(fd[1]); // close the write pipe

		char buffer[256];
		int n;
		// lecture des messages du tuyau
		while ((n = read(fd[0], buffer, sizeof(buffer))) > 0) {
			write(1, buffer, n); // this possibly writes to the stdout instead of passing it up the pipe chain
		}
		close(fd[0]);

		printf("Processus %d termine\n", prcNum);
		fflush(stdout);
		sleep(10);
	}
}
