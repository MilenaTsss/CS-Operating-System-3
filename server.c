//
// Created by milena on 22.05.2023.
//

#define MAX_NUM_OF_GUNS 50

#include "helper.h"

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

void handleClient(int clientSocket);

char *memory_name = "shared memory";
int *shared;
int shmid;

sem_t first;
sem_t second;


void destroy() {
    sem_destroy(&first);
    sem_destroy(&second);

    printf("Closed all semaphores\n");

    if ((shmid = shm_open(memory_name, O_CREAT | O_RDWR, S_IRWXU)) == -1) {
        if (shm_unlink(memory_name) == -1) {
            perror("shm_unlink");
            dieWithError("Error getting pointer to shared memory");
        }
    }
    printf("Shared memory is unlinked\n");
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage:  %s <Server Port>\n", argv[0]);
        exit(1);
    }

    unsigned short serverPort = atoi(argv[1]);
    int serverSocket = createSocket();

    struct sockaddr_in serverAddress;
    setServerAddress(&serverAddress, serverPort);
    bindServer(serverSocket, &serverAddress);

    int clientSocket;
    struct sockaddr_in clientAddress;
    unsigned int clientLen; /* Length of client address data structure */

    printf("Server IP address = %s. Wait...\n", inet_ntoa(clientAddress.sin_addr));

    listenServer(serverSocket);

    if ((shmid = shm_open(memory_name, O_CREAT | O_RDWR, S_IRWXU)) == -1) {
        perror("shm_open");
        dieWithError("Memory opening error");
    } else {
        printf("Memory opened: name = %s, id = 0x%x\n", memory_name, shmid);
    }

    // set memory size
    if (ftruncate(shmid, (int) sizeof(int) * MAX_NUM_OF_GUNS) == -1) {
        perror("ftruncate");
        dieWithError("Setting memory size error");
    } else {
        printf("Memory size set - %d\n", (int) sizeof(int) * MAX_NUM_OF_GUNS);
    }

    shared = mmap(0, sizeof(int) * MAX_NUM_OF_GUNS,
                  PROT_WRITE | PROT_READ, MAP_SHARED, shmid, 0);

    if (sem_init(&first, 1, 0) == -1) {
        dieWithError("sem_init(first) failed");
    }
    if (sem_init(&second, 1, 0) == -1) {
        dieWithError("sem_init(second) failed");
    }

    printf("All semaphores are inited\n\n");

    pid_t processID;

    if ((processID = fork()) < 0) {
        dieWithError("fork() failed");
    }

    clientLen = sizeof(clientAddress);

    /* Wait for a client to connect */
    if ((clientSocket = accept(serverSocket, (struct sockaddr *) &clientAddress, &clientLen)) < 0) {
        dieWithError("accept() failed");
    }

    for (;;) {
        if (processID == 0) {
            sem_wait(&first);
        } else {
            sem_wait(&second);
        }
        printf("Handling client %s\n", inet_ntoa(clientAddress.sin_addr));
        handleClient(clientSocket);

        if (processID == 0) {
            sem_post(&second);
        } else {
            sem_post(&first);

        }
    }

    destroy();
}