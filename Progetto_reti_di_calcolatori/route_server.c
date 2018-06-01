#include <sys/types.h>
#include <unistd.h> 
#include <arpa/inet.h> 
#include <sys/socket.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "wrapper_project.h"
#define PIANI 5
#define INTERNI 3
#define MAXSIZE 1024

int main()
{
   /* Descrittore per l'ascolto tra ROUTE SERVER e INFO SERVER */
   int list_fd, conn_fd;
   /* Variabile STRCT SOCKADDR_IN per memorizzare i valori dell'INFO SERVER 
      e del ROUTE SERVER */
   struct sockaddr_in route_server, info_server;
   /* Variabile BUFFER per registrare i dati in entrata e in uscita
      dall'INFO SERVER */
   char buffer[MAXSIZE + 1];
   /* LEN è una variabile che inizializza il valore in ingresso 
      dell'INFO SERVER*/
   socklen_t len;
   /* In queste variabili scrivo i valori di ritorno delle funzioni RECV e
      SEND */
   int n_write, n_read;
   /* Inserisco in questa variabile di tipo TIME_T l'orologio al tempo della
      macchina UNIX */
   time_t time_up;
   /* Matrice delle adiacenze (senza segno) per trascrivere i le connessioni
      tra un nodo e l'altro attraverso un arco */
   unsigned int matrice_adiacenze[PIANI][PIANI];
   /* Valore di ritorno che ci consentirà calcolare il percorso dall'inizio
      fino al servizio richiesto dal client, bypassato dall'INFO SERVER */
   int percorso, recive = 0;

   system("clear");

   /* Settaggio del tempo attraverso TIME()*/
   time(&time_up);

   /* Chiamata alla funzione per inizializzare a zero tutta la matrice delle 
      adiacenze */
   matrix(matrice_adiacenze);
   
   /* Chiamata alla funzione per settare staticamente la matrice delle
      adiancenze */
   set_matrix(matrice_adiacenze);

   printf("Route server connessio alla porta:");
   printf("%d\n", route_server.sin_port);
   printf("\nOrario UNIX: %s\n", ctime(&time_up));

   /* Creazione del socket per la comunicazione */
   if((list_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
   {
      fprintf(stderr, "Errore in SOCKET().\n");
      exit(1);
   }

   /* Inizializzazione della variabile per ROUTE SERVER */
   route_server.sin_family = AF_INET;
   route_server.sin_port = htons(3600);
   route_server.sin_addr.s_addr = htonl(INADDR_LOOPBACK); /*Wild card*/

   /* Assegno al socket un indirizzo del route server */
   if(bind(list_fd,(struct sockaddr *) &route_server, 
       sizeof(route_server)) == -1)
   {
      fprintf(stderr, "Errore in BIND().\n");
      exit(1);
   }

   /*Messa in ascolto del ROUTE SERVER */
   if(listen(list_fd, 1024) < 0)
   {
      fprintf(stderr, "Errore in LISTEN().\n");
      exit(1);
   }

   /* Ciclo infinito */
   while(1)
   {
      /* Inizializzo la lunghezza dell'INFO SERVER nella variabile LEN */
      len = sizeof(info_server);

      /* Accetta la connessione da parte dell'INFO SERVER*/
      conn_fd = accept(list_fd, (struct sockaddr *) &info_server, &len);

      /* Se il valore di ritorno dalla funzione ACCEPT() e minore di zero, 
         allora c'è un errore */
      if(conn_fd < 0)
      {
         fprintf(stderr, "Errore di connessione.\n");
         exit(1);
      }

      inet_ntop(AF_INET, &info_server, buffer, sizeof(buffer));
      printf("Info server connesso : %s\n", inet_ntoa(info_server.sin_addr));
      printf("Info server connesso alla porta: %d\n", info_server.sin_port);

      /* Il route server riceve informazioni tramite un buffer 
         dall'INFO SERVER */
      n_read = recv(conn_fd, &recive, sizeof(recive), 0);
      
      /* Se il vaore di ritorno dalla RECV() è minore di zero, allora c'è stato
         un errore durante la ricezione del messaggio*/
      if(n_read < 0)
      {
         fprintf(stderr, "Errore durante la RECV().\n");
         exit(1);
      }

      /* Stampo il messaggio bypassato dal client all'info server */
      printf("Messaggio bypassato. = %d \n", recive);
      
      /*Se la condizione è vera, allora si procede con il calcolo del percorso*/
      if(recive == 1)
      {
         printf("\n");

         /* Chiamata alla funzione per il calcolo del percorso */
         percorso = walktrhough_graph(matrice_adiacenze, recive);
         
         /* Se il valore di ritorno e il suo predicato risulta essere vero, 
            allora il percorso è stato calcolato */
         if(percorso >= 0)
         {
            fprintf(stdout, "Percorso calcolato\n");
         }
         else /*Nel caso fosse falso, si stampa il seguente messaggio di errore*/
         {
            fprintf(stderr, "Errore nel calcolo del percorso.\n");
         }

         /*Invio l'esito all'info server*/
         n_write = send(conn_fd, &percorso, sizeof(percorso), 0);

         if(n_write < 0)
         {
            fprintf(stderr, "Errore durante la SEND().\n");
            exit(1);
         }
      }
      else if(recive == 2)
      {
         printf("\n");
         
         percorso = walktrhough_graph(matrice_adiacenze, recive);
         
         if(percorso >= 0)
         {
            fprintf(stdout, "Percorso calcolato\n");
         }
         else
         {
            fprintf(stderr, "Errore nel calcolo del percorso.\n");
         }

         n_write = send(conn_fd, &percorso, sizeof(percorso), 0);

         if(n_write < 0)
         {
            fprintf(stderr, "Errore durante la SEND().\n");
            exit(1);
         }
      }
      else if(recive == 3)
      {
         printf("\n");
         
         percorso = walktrhough_graph(matrice_adiacenze, recive);
         
         if(percorso >= 0)
         {
            fprintf(stdout, "Percorso calcolato\n");
         }
         else
         {
            fprintf(stderr, "Errore nel calcolo del percorso.\n");
         }

         n_write = send(conn_fd, &percorso, sizeof(percorso), 0);

         if(n_write < 0)
         {
            fprintf(stderr, "Errore durante la SEND().\n");
            exit(1);
         }
      }
      else if(recive == 4)
      {
         printf("\n");
         
         percorso = walktrhough_graph(matrice_adiacenze, recive);
         
         if(percorso >= 0)
         {
            fprintf(stdout, "Percorso calcolato\n");
         }
         else
         {
            fprintf(stderr, "Errore nel calcolo del percorso.\n");
         }

         n_write = send(conn_fd, &percorso, sizeof(percorso), 0);

         if(n_write < 0)
         {
            fprintf(stderr, "Errore durante la SEND().\n");
            exit(1);
         }
      }
      else if(recive == 5)
      {
         printf("\n");
         
         percorso = walktrhough_graph(matrice_adiacenze, recive);
         
         if(percorso >= 0)
         {
            fprintf(stdout, "Percorso calcolato\n");
         }
         else
         {
            fprintf(stderr, "Errore nel calcolo del percorso.\n");
         }

         n_write = send(conn_fd, &percorso, sizeof(percorso), 0);

         if(n_write < 0)
         {
            fprintf(stderr, "Errore durante la SEND().\n");
            exit(1);
         }
      }
      else /* In caso il messaggio bypassato sia errato */
      {
         fprintf(stderr, "Comando errato!!\n");
         exit(1);
      }
      /*Chiudo il descrittore file della connessione */
      close(conn_fd);
   }
   exit(0);
}

/* Corpo della funzione MATRIX che inizializza a zero tutti i valori 
   all'interno della matrice delle adiacenze */
void matrix(unsigned int matrice_adiacenze [PIANI][PIANI])
{
   int i, j;

   for(i = 0; i < PIANI; i++)
   {
      for(j = 0; j < PIANI; j++)
      {
         matrice_adiacenze[i][j] = 0;
      }
   }
}

/* Corpo della procedura per settare staticamente la matrice della adiacenze
   per poi poter calcolare il percorso dall'inizio */
void set_matrix(unsigned int matrice_adiacenze[PIANI][PIANI])
{
   matrice_adiacenze[0][1] = 1;
   matrice_adiacenze[0][1] = 1;
   matrice_adiacenze[1][0] = 1;
   matrice_adiacenze[1][2] = 1;
   matrice_adiacenze[2][0] = 1;
   matrice_adiacenze[2][1] = 1;
   matrice_adiacenze[2][3] = 1;
   matrice_adiacenze[3][2] = 1;
   matrice_adiacenze[4][3] = 1;
}

/* Funzione per il calcolo del percorso all'interno dell'edificio tramite
   matrice delle adiacenze */
int walktrhough_graph(unsigned int matrice_adiacenze[PIANI][PIANI], int recive)
{
   /* Indici della matrice */
   int i, j;
   /*Variabili per calcolare il cammino da nodo a nodo e se sono adiacenti */
   int adiacenze, cammino = 0;
   /*Il calcolo del percorso parte dall'inizio fino al nodo richiesto*/
   i = 0;

   /* Se il valore ricevuto direttamente dal client è minore di 0 o maggiore
      del numero di nodi disponibili, allora c'è stato un errore */
   if(recive < 0 || recive >= PIANI)
   {
      /*Ritorno un valore di errore dall'info server al client*/
      return -1;
   }

   /* Inizio del ciclo while per il calcolo del percorso dall'inizio*/
   while(i < PIANI)
   {
      /* Setto le adiacenze a zero ad ogni nodo */
      adiacenze = 0;
      
      /* Verifico da nodo a nodo */
      for(j = 0; j < PIANI; j++)
      {
         /* Verifico se tramite la matrice delle adiacenze al passo j-esimo c'è
            un nodo adiacente*/
         if(matrice_adiacenze[i][j] == 1)
         {
            adiacenze++;
         }
         
         /* Se il nodo al passo i-esimo è uguale al nodo scelto dall'utente, 
            allora inviamo il cammino calcolato dal route all'info server */
         if(recive == i)
         {
            return cammino;
         }
      }
      /* Incremento il cammino da nodo a nodo e l'indice della matrice delle
         adiacenze */
      i++;
      cammino++;
   }
}
