//
// Created by milena on 22.05.2023.
//

#ifndef UNTITLED2_HELPER_H
#define UNTITLED2_HELPER_H

#include <stdio.h>  /* for perror(), printf(), fprintf() */
#include <stdlib.h> /* for exit(), atoi() */
#include <string.h> /* for memset() */
#include <unistd.h> /* for close() */

#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */

#define MAX_PENDING 5 /* Maximum outstanding connection requests */

void dieWithError(char *message);

int createSocket();

void setServerAddressWithIP(struct sockaddr_in* serverAddress, char* serverIP, unsigned short serverPort);
void setServerAddress(struct sockaddr_in* serverAddress, unsigned short serverPort);

void connectServer(int sock, struct sockaddr_in* serverAddress);
void bindServer(int sock, struct sockaddr_in* serverAddress);
void listenServer(int sock);

#endif//UNTITLED2_HELPER_H