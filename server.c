//
// Created by milena on 22.05.2023.
//

#define MAX_NUM_OF_GUNS 50
#define RECEIVED_BUFFER_SIZE MAX_NUM_OF_GUNS * sizeof (int)

#include "helper.h"

char *memory_name = "shared memory";
int *shared;
int shmid;

sem_t *first;
sem_t *second;


void destroy() {
    sem_destroy(first);
    sem_destroy(second);

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
        fprintf(stderr, "<Server Port>\n");
        exit(1);
    }

    unsigned short serverPort = atoi(argv[1]);
    int serverSocket = createSocket();

    struct sockaddr_in serverAddress;
    setServerAddress(&serverAddress, serverPort);
    bindServer(serverSocket, &serverAddress);

    int clientSocket;
    struct sockaddr_in clientAddress;
    unsigned int clientLen;

    printf("Server IP address = %s. Wait...\n", inet_ntoa(clientAddress.sin_addr));

    listenServer(serverSocket);

    if ((shmid = shm_open(memory_name, O_CREAT | O_RDWR, S_IRWXU)) == -1) {
        perror("shm_open");
        dieWithError("Memory opening error");
    } else {
        printf("Memory opened: name = %s, id = 0x%x\n", memory_name, shmid);
    }

    if (ftruncate(shmid, sizeof(int) * (MAX_NUM_OF_GUNS + 1) + 2 * sizeof(sem_t)) == -1) {
        perror("ftruncate");
        dieWithError("Setting memory size error");
    } else {
        printf("Memory size set - %ld\n", sizeof(int) * (MAX_NUM_OF_GUNS + 1) + 2 * sizeof(sem_t));
    }

    void *pointer = mmap(0, sizeof(int) * (MAX_NUM_OF_GUNS + 1) + 2 * sizeof(sem_t),
                         PROT_WRITE | PROT_READ, MAP_SHARED, shmid, 0);
    shared = pointer;
    first = pointer + sizeof(int) * (MAX_NUM_OF_GUNS + 1);
    second = first + sizeof(sem_t);
    shared[0] = 1;
    shared[1] = -1;

    if (sem_init(first, 1, 0) == -1) {
        dieWithError("sem_init(first) failed");
    }

    if (sem_init(second, 1, 0) == -1) {
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
    printf("Handling client %s\n", inet_ntoa(clientAddress.sin_addr));

    if (processID == 0) {
        sem_post(first);
        sem_wait(second);
        sem_post(first);
    } else {
        sem_post(second);
        sem_wait(first);
    }
    printf("Begin war\n");
    for (;;) {
        if (processID == 0) {
            sem_wait(first);
        } else {
            sem_wait(second);
        }

        int receivedMessageSize = shared[0];
        if (receivedMessageSize == -1) {
            break;
        }

        if (send(clientSocket, &shared[1], receivedMessageSize * sizeof(int), 0) != receivedMessageSize * sizeof(int)) {
            dieWithError("send() failed");
        }

        sleep(1);
        /* Receive message from client */
        if ((receivedMessageSize = recv(clientSocket, &shared[1], RECEIVED_BUFFER_SIZE, 0)) < 0) {
            dieWithError("recv() failed");
        }
        shared[0] = receivedMessageSize / sizeof(int);
        if (shared[1] == -1) {
            shared[0] = -1;
        }

        if (processID == 0) {
            sem_post(second);
        } else {
            sem_post(first);
        }
        if (shared[0] == -1) {
            break;
        }
    }

    printf("End of war\n");
    close(clientSocket);
    destroy();
}