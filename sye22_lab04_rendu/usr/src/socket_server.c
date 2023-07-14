/*
 * Copyright (C) 2021 David Truan <david.truan@heig-vd.ch>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */
 #include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "socket_sye.h"

int main(int argc, char *argv[]) {

    /* A COMPLETER: déclarations des variables */
    int running = 1;
    int counter = 0;

    // Variables tampons et lecture
    char receiveBuff[BUFFER_LEN];
    char sendBuff[BUFFER_LEN];
    memset(receiveBuff, 0, sizeof(receiveBuff)); 
    memset(sendBuff, 0, sizeof(sendBuff)); 

    // Variables de connexion
    struct sockaddr_in serv_addr;
    struct sockaddr_in client_addr; 
    const int PORT = 5000;
	const int MAX_CONNECTIONS = 10;
    int listenfd = 0, connfd = 0;
    char clientAdr[INET_ADDRSTRLEN];
    memset(&serv_addr, 0, sizeof(serv_addr));
    memset(&client_addr, 0, sizeof(client_addr));

    /* A COMPLETER: Mise en place du socket et connexion avec le client */
    if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\nERROR : Socket cannot be created");
        return EXIT_FAILURE;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT); 

    if(bind(listenfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))) {
        printf("\nERRROR : Bad binding");
        return EXIT_FAILURE;
    }

    if(listen(listenfd, MAX_CONNECTIONS)) {
        printf("\nERROR : Bad listening");
        return EXIT_FAILURE;
    }

    printf("Waiting for clients...\n");

    // Taille de l'adresse IP
    int client_addr_size = sizeof(client_addr);

    if((connfd = accept(listenfd, (struct sockaddr*) &client_addr, (socklen_t*) &client_addr_size)) < 0) {
        printf("ERRROR : Accept failed");
        return EXIT_FAILURE;
    }
    
    // C'est ici que l'on récupère l'adresse du client qui se connecte
    if(inet_ntop( AF_INET, &client_addr.sin_addr, clientAdr, INET_ADDRSTRLEN ) <= 0) {
        printf("\nERROR: inet_ntop error occured\n");
        return EXIT_FAILURE;
    } 

    // A partir d'ici, la connexion a réussie
    printf("Client connected!\n");

    /* Boucle de traitement principal. C'est ici que la reception des message 
        et l'envoi des réponses se fait. */
    while (running) {
        /* A COMPLETER: Reception et traitement des messages */

        // Lecture du message
        if(read(connfd, receiveBuff, sizeof(receiveBuff)) < 0) {
            printf("\nERROR : Bad reading\n");
            break;
        }

        printf("The client said: %s\n", receiveBuff);

        if(!strcmp(receiveBuff, BONJOUR) || !strcmp(receiveBuff, AUREVOIR)) {

            snprintf(sendBuff, BUFFER_LEN, "%s client %s", receiveBuff, clientAdr);

        } else if(!strcmp(receiveBuff, COMPTEUR)) {

            snprintf(sendBuff, BUFFER_LEN, "Valeur compteur %d", ++counter);

        } else if(!strcmp(receiveBuff, QUITTER)) {

            printf("The client asked for a disconnection, now quitting...\n");
            running = 0;
        }
        
        /* A COMPLETER: Envoi de la réponse */
        write(connfd, sendBuff, BUFFER_LEN);
    }


    /* A COMPLETER: Nettoyage des ressources*/
    close(listenfd);
	close(connfd);

    return 0;


}