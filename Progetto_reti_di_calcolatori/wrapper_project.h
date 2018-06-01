#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <time.h>
#define PIANI 5
#define INTERNI 3
#define MAXSIZE 1024

/* Struttura che definisce la variabile per la costruzione della lista dei 
   servizi.
   Per questa struttura EDIFICIO, le variabili PIANO ed INTERNO si possono 
   intendere come la loro posizione all'interno di una matrice, dove le righe
   sono i piani; gli interni le colonne. */
struct EDIFICIO
{
   /* Identifica il nome del servizio nell'edificio */
   char servizio[MAXSIZE];
   /* Identifica il piano dove è ubicato il servizio */
   unsigned int piano;
   /* Identifica l'interno dove è ubicato il servizio*/
   unsigned int interno;
};

/* Procedura per la stampa del menù iniziale */
void intro_menu();

/* Procedura per le prime istruzioni */
static void inizialized();

/* Procedura d'inizio progetto */
void start_project();

/* Procedura che ci conferma l'apertura del lato server */
static void open_server();

/* Procedura che ci permette di individuare l'errore degli argomenti passati
   al main */
static void error();

/* Prototipo della procedura per la stampa a video dell'errore del path */
static void error_argument();

/* Prototipo per la costruzione statica della lista servizi */
void build_list(struct EDIFICIO []);

/* Procedura prototipo per la costruzione della matrice delle adiacenze 
   per la costruzione del grafo */
void matrix(unsigned int [PIANI][PIANI]);

/* Procedura per settare staticamente la matrice delle adiacenze */
void set_matrix(unsigned int [PIANI][PIANI]);

/* Funzione per calcolare il percorso di un grafo dall'inizio fino al nodo
   scelto */
int walktrhough_graph(unsigned int [PIANI][PIANI], int );
