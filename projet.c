#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <time.h>

struct arg
{
	sem_t S;   //
	sem_t T;   // sémaphores
	sem_t U;   //
	int nbCh;   // nombre de chaudrons
	int nbCu;   // nombre de cuillères
	pthread_cond_t condUstensileDispo;   // condition "ustensiles disponibles"
	pthread_mutex_t mut;   // mutex pour assurer l'accès exclusif aux variables critiques
};

void* mielline (void* argu)   // fonction mielline exécutée par le thread druide1
{
	struct arg *ptr=(struct arg*)argu;
	pthread_mutex_lock(&(ptr->mut));   // P(mut) --- algorithme de Dijkstra
	ptr->nbCh--;   // décrémentation de nbCh
	ptr->nbCu--;   // décrémentation de nbCu
	printf("Début mielline!\n");
	printf("nb chaudrons dispo: %d\n",ptr->nbCh);     //
	printf("nb cuillères dispo: %d\n\n",ptr->nbCu);   // facultatifs
	pthread_mutex_unlock(&(ptr->mut));   // V(mut)
	sleep(3+rand()%4);   // temps de préparation
	pthread_mutex_lock(&(ptr->mut));   // P(mut)
	ptr->nbCh++;   // incrémentation de nbCh
	ptr->nbCu++;   // incrémentation de nbCu
	printf("Fin mielline!\n");
	printf("nb chaudrons dispo: %d\n",ptr->nbCh);     //
	printf("nb cuillères dispo: %d\n\n",ptr->nbCu);   // facultatifs
	pthread_cond_signal(&(ptr->condUstensileDispo));   // on réveille les autres threads
	pthread_mutex_unlock(&(ptr->mut));   // V(mut)
	sem_post(&(ptr->S));   //
	sem_post(&(ptr->S));   // V2(S)
}

void* calissone (void* argu)   // fonction calissone exécutée par le thread druide2
{
	struct arg *ptr=(struct arg*)argu;
	pthread_mutex_lock(&(ptr->mut));   // P(mut)	
	ptr->nbCh--;   // décrémentation de nbCh
	printf("Début calissone!\n");
	printf("nb chaudrons dispo: %d\n",ptr->nbCh);
	printf("nb cuillères dispo: %d\n\n",ptr->nbCu);
	pthread_mutex_unlock(&(ptr->mut));   // V(mut)
	sleep(3+rand()%4);   // temps de préparation
	pthread_mutex_lock(&(ptr->mut));   // P(mut)
	ptr->nbCh++;   // incrémentation de nbCh
	printf("Fin calissone!\n");
	printf("nb chaudrons dispo: %d\n",ptr->nbCh);
	printf("nb cuillères dispo: %d\n\n",ptr->nbCu);
	pthread_cond_signal(&(ptr->condUstensileDispo));   // on réveille les autres threads
	pthread_mutex_unlock(&(ptr->mut));   // V(mut)
	sem_post(&(ptr->T));   //
	sem_post(&(ptr->T));   // V2(T)
}

void* navinette (void* argu)   // fonction navinette exécutée par les threads druide3 et druide4
{
	struct arg *ptr=(struct arg*)argu;
	sem_wait(&(ptr->S));   // P1(S)
	pthread_mutex_lock(&(ptr->mut));   // P(mut)
	while((ptr->nbCh==0) || (ptr->nbCu==0))   // tant que un chaudron et une cuillère ne sont pas dispo en même temps ...
		pthread_cond_wait(&(ptr->condUstensileDispo),&(ptr->mut));   // ... le thread reste en sommeil
	ptr->nbCh--;   // décrémentation de nbCh
	ptr->nbCu--;   // décrémentation de nbCu
	printf("Début navinette!\n");
	printf("nb chaudrons dispo: %d\n",ptr->nbCh);
	printf("nb cuillères dispo: %d\n\n",ptr->nbCu);
	pthread_mutex_unlock(&(ptr->mut));   // V(mut)
	sleep(3+rand()%4);   // temps de préparation
	pthread_mutex_lock(&(ptr->mut));   // P(mut)
	ptr->nbCh++;   // incrémentation de nbCh
	ptr->nbCu++;   // incrémentation de nbCu
	printf("Fin navinette!\n");
	printf("nb chaudrons dispo: %d\n",ptr->nbCh);
	printf("nb cuillères dispo: %d\n\n",ptr->nbCu);
	pthread_cond_signal(&(ptr->condUstensileDispo));   // on réveille les autres threads
	pthread_mutex_unlock(&(ptr->mut));   // V(mut)
	sem_post(&(ptr->U));   // V1(U)
}

void* noyonnaise (void* argu)   // fonction noyonnaise exécutée par le thread druide5
{
	struct arg *ptr=(struct arg*)argu;
	sem_wait(&(ptr->T));   // P1(T)
	pthread_mutex_lock(&(ptr->mut));   // P(mut)
	while(ptr->nbCh==0)   // tant qu'il n'y pas de chaudron dispo ...
		pthread_cond_wait(&(ptr->condUstensileDispo),&(ptr->mut));   // ... le thread reste en sommeil
	ptr->nbCh--;   // décrémentation de nbCh
	printf("Début noyonnaise!\n");
	printf("nb chaudrons dispo: %d\n",ptr->nbCh);
	printf("nb cuillères dispo: %d\n\n",ptr->nbCu);
	pthread_mutex_unlock(&(ptr->mut));   // V(mut)
	sleep(2+rand()%6);   // temps de préparation
	pthread_mutex_lock(&(ptr->mut));   // P(mut)
	ptr->nbCh++;   // incrémentation de nbCh
	printf("Fin noyonnaise!\n");
	printf("nb chaudrons dispo: %d\n",ptr->nbCh);
	printf("nb cuillères dispo: %d\n\n",ptr->nbCu);
	pthread_cond_signal(&(ptr->condUstensileDispo));   // on réveille les autres threads
	pthread_mutex_unlock(&(ptr->mut));   // V(mut)
	sem_post(&(ptr->U));   // V1(U)
}

void* fraisade (void* argu)   // fonction fraisade exécutée par le thread druide6
{
	struct arg *ptr=(struct arg*)argu;
	sem_wait(&(ptr->T));   // P1(T)
	pthread_mutex_lock(&(ptr->mut));   // P(mut)
	while((ptr->nbCh==0) || (ptr->nbCu==0))   // tant que un chaudron et une cuillère ne sont pas dispo en même temps ...
		pthread_cond_wait(&(ptr->condUstensileDispo),&(ptr->mut));   // ... le thread reste en sommeil
	ptr->nbCh--;   // décrémentation de nbCh
	ptr->nbCu--;   // décrémentation de nbCu
	printf("Début fraisade!\n");
	printf("nb chaudrons dispo: %d\n",ptr->nbCh);
	printf("nb cuillères dispo: %d\n\n",ptr->nbCu);
	pthread_mutex_unlock(&(ptr->mut));   // V(mut)
	sleep(2+rand()%5);   // temps de préparation
	pthread_mutex_lock(&(ptr->mut));   // P(mut)
	ptr->nbCh++;   // incrémentation de nbCh
	ptr->nbCu++;   // incrémentation de nbCu
	printf("Fin fraisade!\n");
	printf("nb chaudrons dispo: %d\n",ptr->nbCh);
	printf("nb cuillères dispo: %d\n\n",ptr->nbCu);
	pthread_cond_signal(&(ptr->condUstensileDispo));   // on réveille les autres threads
	pthread_mutex_unlock(&(ptr->mut));   // V(mut)
	sem_post(&(ptr->U));   // V1(U)
}

void* potion_magique (void* argu)   // fonction potion_magique exécutée par le thread druide7
{
	struct arg *ptr=(struct arg*)argu;
	sem_wait(&(ptr->U));   //
	sem_wait(&(ptr->U));   //
	sem_wait(&(ptr->U));   // P4(U)
	sem_wait(&(ptr->U));   //
	pthread_mutex_lock(&(ptr->mut));   // P(mut)
	while((ptr->nbCh==0) || (ptr->nbCu==0))   // // tant que un chaudron et une cuillère ne sont pas dispo en même temps ...
		pthread_cond_wait(&(ptr->condUstensileDispo),&(ptr->mut));   // ... le thread reste en sommeil
	ptr->nbCh--;   // décrémentation de nbCh
	ptr->nbCu--;   // décrémentation de nbCu
	printf("Début potion magique!\n");
	printf("nb chaudrons dispo: %d\n",ptr->nbCh);
	printf("nb cuillères dispo: %d\n\n",ptr->nbCu);
	pthread_mutex_unlock(&(ptr->mut));   // V(mut)
	sleep(1+rand()%2);   // temps de préparation
	pthread_mutex_lock(&(ptr->mut));   // P(mut)
	ptr->nbCh++;   // incrémentation de nbCh
	ptr->nbCu++;   // incrémentation de nbCu
	printf("Fin potion magique!\n");
	printf("nb chaudrons dispo: %d\n",ptr->nbCh);
	printf("nb cuillères dispo: %d\n",ptr->nbCu);
	pthread_mutex_unlock(&(ptr->mut));   // V(mut)
	exit(0);
}

int main()
{
	srand(time(NULL));   // pour varier les scénarios à chaque exécution du programme

	// déclaration des threads et de la structure voulus
	pthread_t druide1,druide2,druide3,druide4,druide5,druide6,druide7;
	struct arg st;

	sem_init(&(st.S),1,0);   // initialisation du sémaphore S "lié" à la mielline
	sem_init(&(st.T),1,0);   // initialisation du sémaphore T "lié" à la calissone
	sem_init(&(st.U),1,0);   // initialisation du sémaphore U "lié" à la potion magique

	st.nbCh=2;   // initialisation du nombre de chaudrons à 2
	st.nbCu=2;   // initialisation du nombre de cuillères à 2
	pthread_mutex_init(&(st.mut),NULL);   // création du mutex mut
	pthread_cond_init(&(st.condUstensileDispo),NULL);   // création de la condition condUstensileDispo

	// on affiche les données initiales
	printf("Départ\n");
	printf("nb chaudrons dispo: %d\n",st.nbCh);
	printf("nb cuillères dispo: %d\n\n",st.nbCu);
	
	// création des 7 threads nécessaires au déroulement de la recette
	pthread_create(&druide1,NULL,mielline,(void*)&st);
	pthread_create(&druide2,NULL,calissone,(void*)&st);
	pthread_create(&druide3,NULL,navinette,(void*)&st);
	pthread_create(&druide4,NULL,navinette,(void*)&st);
	pthread_create(&druide5,NULL,noyonnaise,(void*)&st);
	pthread_create(&druide6,NULL,fraisade,(void*)&st);
	pthread_create(&druide7,NULL,potion_magique,(void*)&st);
	
	pthread_exit(NULL);   // on termine le thread initial uniquement
}