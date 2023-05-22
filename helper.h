//
// Created by milena on 22.05.2023.
//

#ifndef OS3_HELPER_H
#define OS3_HELPER_H

#include <arpa/inet.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_PENDING 5

void dieWithError(char *message);

int createSocket();
void setServerAddressWithIP(struct sockaddr_in *serverAddress, char *serverIP,
                            unsigned short serverPort);
void setServerAddress(struct sockaddr_in *serverAddress,
                      unsigned short serverPort);
void connectServer(int sock, struct sockaddr_in *serverAddress);
void bindServer(int sock, struct sockaddr_in *serverAddress);
void listenServer(int sock);

#endif//OS3_HELPER_H