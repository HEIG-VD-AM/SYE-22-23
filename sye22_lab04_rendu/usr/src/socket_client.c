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

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <stdbool.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "socket_sye.h"

/* Variable de "routage" */
msg_idx_t message_to_send = 0;

/* A COMPLETER: Handlers des signaux */

void process_siguser(int sig) {

    switch(sig) {
    case SIGUSR1:
        printf("Handler USR1\n");
        message_to_send = GREETING;
        break;
    case SIGUSR2:
        printf("Handler USR2\n");
        message_to_send = COUNTING;
        break;
    default:
        break;
    }
}

void process_sigint(int sig) {

    printf("Handler INT\n");
    message_to_send = QUITING;
}

int main(int argc, char **argv) {
    /* A COMPLETER: Variables */
    int running = 1;
    bool isHello = true;

    // Variables de connexion
    int sockfd = 0;
    int port = 0;
    struct sockaddr_in serv_addr; 
    memset(&serv_addr, 0, sizeof(serv_addr));

    // Tampons et lecture
    char receiveBuff[BUFFER_LEN];
    char sendBuff[BUFFER_LEN];
    memset(receiveBuff, 0, sizeof(receiveBuff));
    memset(sendBuff, 0, sizeof(sendBuff));

    /* A COMPLETER: Traitement des arguments */
    if(argc != 3) {
        printf("\nUsage : %s <IP Address> <Port>\n", argv[0]);
        return EXIT_FAILURE;
    }

    if((port = atoi(argv[2])) < 0) {
        printf("\nERROR : Bad port");
        return EXIT_FAILURE;
    }

    /* A COMPLETER: Liens entre signaux et handlers */
    signal(SIGUSR1, process_siguser);
    signal(SIGUSR2, process_siguser);
    signal(SIGINT, process_sigint);
    
    printf("Connection to server at %s:%s...", argv[1], argv[2]);

    /* A COMPLETER: Création du socket et connection au serveur */
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\nERROR : Socket cannot be created");
        return EXIT_FAILURE;
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port); 

    if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0)
    {
        printf("FAILED : inet_pton error occured\n");
        return EXIT_FAILURE;
    } 

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
       printf("FAILED : Connect Failed \n");
       return EXIT_FAILURE;
    } 

    printf("SUCCESS\n");


    while (running) {

        /* A COMPLETER: Préparation en envoi du message */
        switch (message_to_send) {
            case GREETING:
                if(isHello) {
                    snprintf(sendBuff, sizeof(sendBuff), "%s", BONJOUR);
                } else {
                    snprintf(sendBuff, sizeof(sendBuff), "%s", AUREVOIR);
                }   
                isHello = !isHello;
                break;
            case COUNTING:
                snprintf(sendBuff, sizeof(sendBuff), "%s", COMPTEUR);
                break;
            case QUITING:
                snprintf(sendBuff, sizeof(sendBuff), "%s", QUITTER);
                running = 0;
                break;
            default:
                continue;
        }

        write(sockfd, sendBuff, sizeof(sendBuff));
        
        /* A COMPLETER: Réception, si nécessaire de la réponse */
        switch (message_to_send) {
            case GREETING:
            case COUNTING:
                read(sockfd, receiveBuff, sizeof(receiveBuff));                
                printf("%s\n", receiveBuff);
                break;
            case QUITING:
			    running = 0;
			    break;
        }

        message_to_send = 0;
    }

    /* A COMPLETER: Nettoyage des ressources */
    close(sockfd);

    return 0;
}
