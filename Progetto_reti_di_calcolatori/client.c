#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
#include "wrapper_project.h"
#define MAXSIZE 1024
#define PIANI 5
#define INTERNI 3

int main(int argc, char *argv[])
{
   int sockfd;/* File descrittore per la porta socket */
   int n_read, n_write; /* Valori in byte per le funzioni READ e WRITE */
   char buffer[MAXSIZE + 1]; /* Buffer per inserire i dati da inviare */
   /* Variabile nel quale inserire i dati fondamentali per la connessione
      quali il PROTOCOLLO, il DOMINIO ecc */
   struct sockaddr_in server;
   /* Altra variabile buffer chiamata LABEL che ci servirà qualora il BUFFER è
      è già stato utilizzato */
   char label[MAXSIZE + 1] ;
   /*La variabile sezione ci permette di inviare un intero, il quale rappresen-
     terà il nodo di riferimento del nostro grafo */
   int sezione, percorso;
   int scelta_servizio, salva_piano = 0, salva_interno = 0;
   int i;

   /*Se gli argomenti sono minori o maggiori di 3 allora il programma non va*/
   if(argc != 3)
   {
      error();
      exit(1);
   }

   system("clear");
   /* Procedura per la stampa del menù iniziale */
   intro_menu();
   /* Inizializzazione primo utilizzo lato client */
   inizialized();

   /* Se l'argomento passato al main è -i, allora il client chiede al server
      la lista dei servizi all'interno dell'edificio */
   if(strcmp(argv[2],"-i") == 0)
   {
      printf("\nRichista lista dei servizi nell'edificio.\n");
      /* Condizione + chiamata a SOCKET() per creare un descrittore file 
         a cui associamo un numero di byte per poter effettuare 
         la connessione al server. In caso di errore, restituisce -1.*/
      if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
      {
         fprintf(stderr,"Errore: SOCKET\n");
         exit(1);
      }

      printf("\n");
      printf("Socket creato.\n");
      printf("\n");

      /* Assegnazione di PORTA e DOMINIO alla variabile struttura server */
      server.sin_family = AF_INET;
      server.sin_port = htons(1024);

      /* Condizione + procedura per convertire dei caratteri (in questo
         caso è ARGV) in un indirizzo appartenente al dominio richiesto
         (in questo caso AF_INET) all'interno della struttura SERVER (più
         precisamente in SIN_ADDR).*/
      if(inet_pton(AF_INET, argv[1], &server.sin_addr) < 0)
      {
         fprintf(stderr,"Errore in INET_PTON per %s\n", argv[1]);
         exit(1);
      }

      /* Il client si connette all'info server tramite la CONNECT(), 
         utilizzando un descrittore file.
         Se il valore di ritorno è minore di zero allora c'è un errore
         durante la connesione con l'info server */
      if(connect(sockfd, (struct sockaddr *) &server, 
         sizeof(server)) < 0)
      {
         fprintf(stderr,"Errore di connessione.\n");
         exit(1);
      }

      /*Pulisco il buffer*/
      bzero(buffer,1024);
      printf("Connessione effettuata.\n");
      /* Inserisco staticamente il valore di passaggio da dare dal client
         all'info server */
      snprintf(buffer, sizeof(buffer),"1");

      /* Invio al server il valore che ho inserito all'interno del buffer 
         precedentemente */
      n_write = send(sockfd, &buffer, strlen(buffer), 0);

      /* Se il valore di ritorno di SEND() è minore di zero, allora c'è stato
         un errore durante l'invio del messaggio */
      if(n_write < 0)
      {
         fprintf(stderr,"Errore in write.\n");
         exit(1);
      }

      /*Stampa della lista dei servizi*/
      printf("Lista servizi ottenuta.\n");
      for(i = 0; i < PIANI; i++)
      {
         n_read = recv(sockfd, &label, sizeof(label), 0);

         printf("%d - ", i+1);
         puts(label);
         printf("\n");
      }
      
      printf("Premere un tasto per continuare!\n");
      getchar();
      printf("Quale servizio scegli?");
      /* Scrivo nel buffer la mia scelta da inviare, poi, all'info server */
      scanf("%d", &scelta_servizio);

      /* Invio al info server la mia scelta */
      n_write = send(sockfd, &scelta_servizio, sizeof(scelta_servizio), 0);
      
      if(n_write < 0)
      {
         fprintf(stderr,"Errore durante la SEND().\n");
         exit(1);
      }

      printf("\nMessaggio inviato.\n");

      /* Ricezione della risposta del server per piano e interno */
      n_read = recv(sockfd, &salva_piano, sizeof(salva_piano), 0);
      printf("Servizio al piano = %d°", salva_piano);
      printf("\n");
      n_read = recv(sockfd, &salva_interno, sizeof(salva_interno), 0);
      printf("Servizio all'interno numero = %d", salva_interno);
      printf("\n");

      close(sockfd);
      exit(0);
   }
   /* Se l'argomento passato al main è -r allora il client sta richiedendo il
      percorso all'interno dell'edificio al route server bypassando la
      richiesta all'info server */
   else if(strcmp(argv[2],"-r") == 0)
   {
      printf("Richiesta percorso edificio.\n");
      /* Condizione + chiamata a SOCKET() per creare un descrittore file 
         a cui associamo un numero di byte per poter effettuare 
         la connessione al server. In caso di errore, restituisce -1.*/

      if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
      {
         fprintf(stderr,"Errore: SOCKET\n");
         exit(1);
      }

      printf("\n");
      printf("Socket creato.\n");
      printf("\n");

      /* Assegnazione di PORTA e DOMINIO alla variabile struttura server */
      server.sin_family = AF_INET;
      server.sin_port = htons(1024);

      /* Condizione + procedura per convertire dei caratteri (in questo
         caso è ARGV) in un indirizzo appartenente al dominio richiesto
         (in questo caso AF_INET) all'interno della struttura SERVER (più
         precisamente in SIN_ADDR).*/
      if(inet_pton(AF_INET, argv[1], &server.sin_addr) < 0)
      {
         fprintf(stderr,"Errore in INET_PTON per %s\n", argv[1]);
         exit(1);
      }

      /* Effettuo la connessione con l'info server tramite la CONNECT() */
      if(connect(sockfd, (struct sockaddr *) &server, 
         sizeof(server)) < 0)
      {
         fprintf(stderr,"Errore di connessione.\n");
         exit(1);
      }

      printf("Connessione effettuata.\n");

      /*Azzero la variabile buffer*/
      bzero(buffer, 1024);
      /*Passo la richiesta staticamente al server tramite un buffer*/
      snprintf(buffer, sizeof(buffer), "2");
      /*Invio all'info server la mia scelta tramite SEND()*/
      n_write = send(sockfd, &buffer, strlen(buffer), 0);

      if(n_write < 0)
      {
         fprintf(stderr, "Errore durante la SEND().\n");
         exit(1);
      }

      buffer[n_write] = 0;
      /* Chiudo il socket della mia connessione */
      
      /*Stampa della lista dei servizi*/
      printf("Lista servizi ottenuta.\n");
      for(i = 0; i < PIANI; i++)
      {
         n_read = recv(sockfd, &label, sizeof(label), 0);

         printf("%d - ", i+1);
         puts(label);
         printf("\n");
      }
      
      printf("\n\n");
      printf("Premere un tasto per continuare.\n");
      getchar();
      bzero(label, 1024);
      printf("\n\n");
      /*printf("Inserire il servizio per il percorso desiderato.\n");
      scanf("%s", label);*/
      printf("\nInserire servizio.\n");
      scanf("%d", &sezione);
      /* Invio la richiesta del percorso del servizio scelto */
      n_write = send(sockfd, &sezione, sizeof(sezione), 0);

      if(n_write  < 0)
      {
         //fprintf(stderr, "Errore durante la SEND().\n");
         perror("send");
         exit(1);
      }

      printf("\nMessaggio inviato.\n");
      
      /* Il client riceve il messaggio che ha calcolato il route 
         dall'info server*/
      n_read = recv(sockfd, &percorso, sizeof(percorso), 0);
      
      if(n_read < 0)
      {
         fprintf(stderr, "Errore durante RECV().\n");
         exit(1);
      }
      
      printf("\n\n");
      printf("\nA partire dal piano 0 al piano %d, cammino è di = %d\n\n", 
              sezione, percorso);

      /* Chiudo il descrittore file per la comunicazione tra client e 
         info server e chiudo la connessione */
      close(sockfd);
      exit(0);
   }
   else
   {
      /* Funzione per il messaggio di errore quando si sbaglia a inserire gli 
         argomenti */
      error_argument();
      exit(1);
   }
   exit(0);
}

/* Corpo della procedura del menù di benvenuto */
void intro_menu()
{
   printf("\n");
   printf("********************************************\n");
   printf("*                                          *\n");
   printf("*                                          *\n");
   printf("*       Benvenuti nel menu' client         *\n");
   printf("*                                          *\n");
   printf("*                                          *\n");
   printf("********************************************\n");

   printf("\n");
   printf("Premere un tasto per continuare.");
   printf("\n");
   getchar();
   system("clear");
   printf("\n");
}

/* Corpo della procedura dell'inizializzazione dell'utente al primo
   utilizzo */
void static inizialized()
{
   printf("************************************************\n");
   printf("* Questo programma è un progetto basato su un  *\n");
   printf("* modello client-server dove uno e pie' client *\n");
   printf("* si collega ad un server concorrente.         *\n");
   printf("* Le istruzioni sono qui sotto come seguito.   *\n");
   printf("************************************************\n");
   printf("\n\n");
   printf("Premere un tasto per continuare.\n");
   getchar();
   system("clear");
   printf("\n");

   printf("************************************************************\n");
   printf("* Per avviare il programma, selezionare l'eseguibile con il*\n");
   printf("* seguente path: ./<nome_eseguibile> <IP> <PATH>           *\n");
   printf("* <IP> --> Indirizzo IP della macchina locale.             *\n");
   printf("* <PATH> --> Switching per la scelta dell'utente.          *\n");
   printf("*-i -> Chiede la lista informazioni dei servizi disponibili*\n");
   printf("*-r -> Richiesta di bypass per il percorso                 *\n");
   printf("************************************************************\n");
}

/* Corpo della procedura ERROR che stampa cosa abbiamo sbagliato durante
    il passaggio degli argomenti passati al main*/
void static error()
{
   fprintf(stderr, "Argomenti passati al main troppi o troppo pochi.\n\n");
   fprintf(stderr, "Esempio: ./<nome_eseguibile> ");
   fprintf(stderr, " <IP_ADDRESS> ");
   fprintf(stderr, "<PATH_SELECTION> ");
}

/* Corpo della procedura ERROR_PATH che stampa un messaggio di errore nel
   caso in cui abbiamo sbagliato il path del secondo argomento passato al
   main*/
void static error_argument()
{
   fprintf(stderr, "Scrittura PATH errata.\n");
   fprintf(stderr, "-i -> Chiede la lista informazioni dei servizi disponibili\n");
   fprintf(stderr, "-r -> Richiesta di bypass per il percorso");
   printf("\n\n\n");
}
