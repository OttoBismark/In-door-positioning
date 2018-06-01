#include <sys/types.h>
#include <unistd.h> 
#include <arpa/inet.h> 
#include <sys/socket.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "wrapper_project.h"
#define MAXSIZE 1024
#define PIANI 5
#define INTERNI 3

int main()
{
   /*Descrittore file per la SOCKET()*/
   int list_fd;
   /*Descrittore file per la ACCEPT()*/
   int conn_fd;
   /*Descrittore file per la SOCKET verso il ROUTE_SERVER*/
   int bp_fd;
   /*Variabili SERVER e CLIENT per inserire i valori fondamentali quali
     DOMINIO, PORTA e tipo di PROTOCOLLO*/
   struct sockaddr_in server, client, route;
   /*Variabile per inizializzare la grandezza della varabile CLIENT*/
   socklen_t size;
   /*Variabile per avviare la procedura di FORK()*/
   pid_t pid;
   /* Variabile per scrivere/ricevere un messaggio */
   char buffer[MAXSIZE + 1];
   /*Variabile che utilizzo per calcolare il tempo secondo la macchina UNIX */
   time_t time_up;
   /*Descrittori per i messaggi ricevuti e inviati*/
   int n_read, n_write;
   char label[MAXSIZE + 1];
   int i;
   struct EDIFICIO lista_servizi[PIANI];
   int recive, scelta_client, salva_piano = 0, salva_interno = 0;
   int cammino;
   
   system("clear");

   /*Prendo il TIMER settato secondo il sistema UNIX*/
   time(&time_up);
   
   /*Procedura come messaggio di avvio*/
   open_server();

   build_list(lista_servizi);

   printf("Server connesso alla porta:");
   printf("%d\n", server.sin_port);
   /*Stampa time di connessione*/
   printf("\nOrario UNIX: %s\n",ctime(&time_up));

   /* Condizione + funzione SOCKET() per creare il socket del server, in
      modo da avviare la comunicazione*/
   if((list_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
   {
      fprintf(stderr,"Errore in SOCKET().\n");
      exit(1);
   }

   /*Creazione del DOMINIO, PORTA e INDIRIZZO del server */
   server.sin_addr.s_addr = htonl(INADDR_ANY);
   server.sin_family = AF_INET;
   server.sin_port = htons(1024);
   
   route.sin_family = AF_INET;
   route.sin_port = htons(3600);

   /* Condizione + funzione BIND() per l'assegnazione
      di un indirizzo al server */
   if(bind(list_fd, (struct sockaddr *) &server, sizeof(server)) < 0)
   {
      fprintf(stderr,"Errore in BIND().\n");
      exit(1);
   }

   /*Preparazione del server, pronto all'ascolto dei client */
   if(listen(list_fd, 1024) < 0)
   {
      fprintf(stdout,"Errore in LISTEN().\n");
      exit(1);
   }

   /* Ciclo infinito */
   while(1)
   {
      /* Inizializzo la grandezza del client */ 
      size = sizeof(client);
      /* Accetto la connessione con il client tramite ACCEPT() */
      conn_fd = accept(list_fd, (struct sockaddr *) &client, &size);
      
      /* In caso di errore, la connessione non viene effettuata */
      if(conn_fd < 0)
      {
         fprintf(stderr,"Errore in ACCEPT.\n");
         exit(1);
      }
      
      /*Condizione + Funzione FORK()*/
      if((pid = fork()) < 0)
      {
         fprintf(stderr,"Errore in FORK().\n");
         exit(1);
      }

      /*Processo figlio*/
      if(pid == 0)
      {
         /*Chiusura del descrittore per l'ascolto*/
         close(list_fd);
         /*Conversione della stringa in un indirizzo (quello del client)*/
         inet_ntop(AF_INET, &client.sin_addr,buffer, sizeof(buffer));
         /*Stampa dell'indirizzo del client e della porta*/
         printf("Client connesso: %s\n", inet_ntoa(client.sin_addr));
         printf("Client connesso alla porta: %d\n", client.sin_port);
         printf("\n\n\b");

         /* Legge il buffer del descrittore file passato dal CLIENT fin quando 
            N (byte del buffer) sono maggiori di zero */
         n_read = recv(conn_fd, &buffer, strlen(buffer), 0);
         /* Se i byte passati dal CLIENT al SERVER sono minori di zero, allora
            c'è stato un errore durante il passaggio del buffer tra i due 
            elementi */
         if(n_read < 0)
         {
            fprintf(stderr, "Errore in lettura di RECV().\n");
            exit(1);
         }
         
         buffer[n_read] = 0;

         /* Verifica di switching */
         /* Se la stringa passata dal client al server è uguale a 1 (valore in 
            risposta dell'utente tramite client) il server manda la lista dei
            servizi al client.
            Se la stringa ricevuta è uguale a 2, allora il server deve calcolare
            il percorso tramite il ROUTE SERVER */
         if(strcmp(buffer, "1") == 0)
         {
            printf("\nScelta effettuata =  %s.\n", buffer);
            printf("Richiesta lista servizi.\n");
            bzero(buffer, 1024);

            /*Invio singolarmente al client la lista informazioni*/            
            for(i = 0; i < PIANI; i++)
            {
               bzero(label, 1024);
               snprintf(label, MAXSIZE, "%s", lista_servizi[i].servizio);
               n_write = send(conn_fd, &label, sizeof(label), 0);
               label[n_write] = 0;
               bzero(label,1024);
            }

            /* L'info server riceve la scelta del client */
            n_read = recv(conn_fd, &scelta_client, sizeof(scelta_client), 0);
            
            if(n_read < 0)
            {
               fprintf(stderr, "Errore duante RECV().\n");
               exit(1);
            }    

            buffer[n_read] = 0;
            
            /* Verifica del messaggio ricevuto tramite la lista dei servizi */
            if(scelta_client == 1)
            {
               printf("Messaggio: %d", scelta_client);
               printf("\n");
               salva_piano = lista_servizi[0].piano;
               salva_interno = lista_servizi[0].interno;

               n_write = send(conn_fd, &salva_piano, sizeof(salva_piano), 0);

               if(n_write < 0)
               {
                  fprintf(stderr, " Errore durante la SEND().\n");
                  exit(1);
               }

               n_write = send(conn_fd, &salva_interno, sizeof(salva_interno), 0);

               if(n_write < 0)
               {
                  fprintf(stderr, "Errore durante la SEND().\n");
                  exit(1);
               }
            }
            else if(scelta_client == 2)
            {
               printf("Messaggio: %d ", scelta_client);
               printf("\n");

               salva_piano = lista_servizi[1].piano;
               salva_interno = lista_servizi[1].interno;
               n_write = send(conn_fd, &salva_piano, sizeof(salva_piano), 0);

               if(n_write < 0)
               {
                  fprintf(stderr, " Errore durante la SEND().\n");
                  exit(1);
               }

               if(n_write = send(conn_fd, &salva_interno, 
                   sizeof(salva_interno), 0) < 0)
               {
                  fprintf(stderr, "Errore durante la SEND().\n");
                  exit(1);
               }
            }
            else if(scelta_client == 3)
            {
               printf("Messaggio: %d ", scelta_client);
               printf("\n");

               salva_piano = lista_servizi[2].piano;
               salva_interno = lista_servizi[2].interno;
               n_write = send(conn_fd, &salva_piano, sizeof(salva_piano), 0);

               if(n_write < 0)
               {
                  fprintf(stderr, " Errore durante la SEND().\n");
                  exit(1);
               }
    
               printf("\n");

               n_write = send(conn_fd, &salva_interno, sizeof(salva_interno), 0);
            }
            else if(scelta_client == 4)
            {
               printf("Messaggio %d", scelta_client);
               printf("\n");

               salva_piano = lista_servizi[3].piano;
               salva_interno = lista_servizi[3].interno;
               n_write = send(conn_fd, &salva_piano, sizeof(salva_piano), 0);
               
               if(n_write < 0)
               {
                  fprintf(stderr, " Errore durante la SEND().\n");
                  exit(1);
               }

               printf("\n");

               n_write = send(conn_fd, &salva_interno, sizeof(salva_interno), 0);
            }
            else if(scelta_client == 5)
            {
               printf("Messaggio %d", scelta_client);
               printf("\n");

               salva_piano = lista_servizi[4].piano;
               salva_interno = lista_servizi[4].interno;
               n_write = send(conn_fd, &salva_piano, sizeof(salva_piano), 0);
               
               if(n_write < 0)
               {
                  fprintf(stderr, " Errore durante la SEND().\n");
                  exit(1);
               }
    
               printf("\n");

               n_write = send(conn_fd, &salva_interno, sizeof(salva_interno), 0);
            }
            else
            {
               printf("Comando errato!!\n");
               exit(1);
            }

            close(conn_fd);
            exit(0);
         }
         /* Se la richiesta dell'utente è la seconda, allora il server 
            richiederà al ROUTE SERVER il percorso dentro l'edificio */
         else if(strcmp(buffer, "2") == 0)
         {
            printf("\nScelta effettuata = %s ", buffer);
            bzero(buffer,1024);
            bzero(label, 1024);

            /* Invio la lista dei servizi al client */
            for(i = 0; i < PIANI; i++)
            {
               bzero(label, 1024);
               snprintf(label, MAXSIZE, "%s", lista_servizi[i].servizio);
               n_write = send(conn_fd, &label, sizeof(label), 0);
               label[n_write] = 0;
               bzero(label,1024);
            }
            
            bzero(buffer, 1024);
            printf("\n\n");
            /* L'info server riceve il messaggio, ossia il numero del servizio
               che si vuole sapere il percorso */
            n_read = recv(conn_fd, &recive, sizeof(recive), 0);

            if(n_read < 0)
            {
               perror("recv");
               exit(1);
            }

            printf("Messaggio ricevuto : %d ",  recive);

            /* Apro una connessione chiamando SOCKET e CONNECT per comunicare
               con il route server */
            if((bp_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
            {
               fprintf(stderr, "Errore durante la SOCKET().\n");
               exit(1);
            }

            printf("\nSocket lato SERVER-ROUTE SERVER creato.\n");

            /* Richiesta connessione dall'INFO SERVER al ROUTE SERVER */
            if(connect(bp_fd, (struct sockaddr *) &route, sizeof(route)) < 0)
            {
               fprintf(stderr, "Errore di connessione\n");
               exit(1);
            }

            printf("\nConnessione con il ROUTE SERVER effettuata.\n");
            printf("\nRichiesta percorso servizi.\n");

            /* L'info server invia il messaggio del client inoltrandolo come un
               bypass fino al route server */
            n_write = send(bp_fd, &recive, sizeof(recive), 0);

            if(n_write < 0)
            {
               fprintf(stderr, "Errore in SEND().\n");
               exit(1);
            }
            
            /*	L'info server riceve il messaggio dal route server */
            n_read = recv(bp_fd, &cammino, sizeof(cammino), 0);
            
            if(n_read < 0)
            {
               fprintf(stderr, "Errore in RECV().\n");
               exit(1);
            }

            printf("\nPercorso calcolato\n");
            
            /*Infine, l'info server invia il percorso calcolato al client */
            n_write = send(conn_fd, &cammino, sizeof(cammino), 0);
            /* Chiudo il descrittore file che collega l'info server e il route
               server*/
            close(bp_fd);
          }
          /*Chiudo il descrittore file che collega l'info server con il client*/
          close(conn_fd);
          exit(0);
       }
       else
       {
          close(conn_fd);
       }
   }
   exit(0);
}

/* Corpo della procedura che stampa un messaggio al primo avvio del server */
static void open_server()
{
   printf("Server avviato.\n");
   printf("\n");
}

/*Funzione per l'inizializzazione manuale della struttura della lista servizi*/
void build_list(struct EDIFICIO lista_servizi[])
{
   strcpy(lista_servizi[0].servizio, "Reception");
   lista_servizi[0].piano = 1;
   lista_servizi[0].interno = 3;

   strcpy(lista_servizi[1].servizio, "Reparto vendite");
   lista_servizi[1].piano = 2;
   lista_servizi[1].interno = 3;

   strcpy(lista_servizi[2].servizio, "Risorse umane");
   lista_servizi[2].piano = 2;
   lista_servizi[2].interno = 1;
   
   strcpy(lista_servizi[3].servizio, "Uffici amministrativi");
   lista_servizi[3].piano = 3;
   lista_servizi[3].interno = 2;

   strcpy(lista_servizi[4].servizio, "Direzione");
   lista_servizi[4].piano = 3;
   lista_servizi[4].interno = 3;
}
