//
// Created by milena on 22.05.2023.
//

#include "helper.h"
#include "war.h"

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "<Server IP> <Server Port> <Field size> <Num of guns>\n");
        exit(1);
    }
    srand(time(NULL));

    int sock = createSocket();
    char *serverIP = argv[1];
    unsigned short serverPort = atoi(argv[2]);

    struct sockaddr_in serverAddress;
    setServerAddressWithIP(&serverAddress, serverIP, serverPort);
    connectServer(sock, &serverAddress);

    field_size = atoi(argv[3]);
    num_of_guns = atoi(argv[4]);
    printf("Field size = %d\nNumber of guns = %d\n", field_size, num_of_guns);

    own_field = malloc(field_size * field_size * sizeof (point_t));
    foreign_field = malloc(field_size * field_size * sizeof (point_t));
    fill_field(own_field);
    simple_fill_field(foreign_field);

    int receivedBuffer[num_of_guns];



    for (;;) {
        if (!check_status()) {
            break;
        }

        int shots[num_of_guns];
        int *p = shots;
        generate_targets(&p);

        if (send(sock, shots, num_of_guns * sizeof (int), 0) != num_of_guns * sizeof (int)) {
            dieWithError("send() sent a different number of bytes than expected");
        }

        sleep(2);

        if (recv(sock, receivedBuffer, num_of_guns * sizeof (int), 0) <= 0) {
            dieWithError("recv() failed or connection closed prematurely");
        }

        for (int i = 0; i < num_of_guns; ++i) {
            if (receivedBuffer[i] > 0) {
                if (own_field[receivedBuffer[i]].type == ALIVE_GUN) {
                    own_field[receivedBuffer[i]].type = DEAD_GUN;
                }
                if (own_field[receivedBuffer[i]].type == EMPTY_POINT) {
                    own_field[receivedBuffer[i]].type = USED_POINT;
                }
            }
        }

        sleep(1);
    }

    printf("\n"); /* Print a final linefeed */

    close(sock);
    free(own_field);
    free(foreign_field);

    exit(0);
}

/*
int mainn(int argc, char **argv) {
    // open memory
    if ((shmid = shm_open(memory_name, O_CREAT | O_RDWR, S_IRWXU)) == -1) {
        perror("shm_open");
        system_error("Memory opening error");
    } else {
        printf("Memory opened: name = %s, id = 0x%x\n", memory_name, shmid);
    }

    // set memory size
    if (ftruncate(shmid, (int) sizeof(point_t) * field_size * field_size * 2) == -1) {
        perror("ftruncate");
        system_error("Setting memory size error");
    } else {
        printf("Memory size set - %d\n", (int) sizeof(point_t) * field_size * field_size * 2);
    }

    first_field = mmap(0, sizeof(point_t) * field_size * field_size * 2,
                       PROT_WRITE | PROT_READ, MAP_SHARED, shmid, 0);
    second_field = first_field + field_size * field_size;

    fill_field(first_field);
    fill_field(second_field);

    sem_t *gun_semaphores_first[field_size * field_size];
    sem_t *gun_semaphores_second[field_size * field_size];
    for (int i = 0; i < field_size * field_size; ++i) {
        char* semaphore_name = get_semaphore_name(i, 1, gun_semaphore_name);
        gun_semaphores_first[i] = sem_open(semaphore_name, O_CREAT, 0666, 0);
        free(semaphore_name);

        semaphore_name = get_semaphore_name(i, 2, gun_semaphore_name);
        gun_semaphores_second[i] = sem_open(semaphore_name, O_CREAT, 0666, 0);
        free(semaphore_name);
    }
    gun_semaphores_pointer_first = gun_semaphores_first;
    gun_semaphores_pointer_second = gun_semaphores_second;

    sem_t *manager_semaphores_first[field_size * field_size];
    sem_t *manager_semaphores_second[field_size * field_size];
    for (int i = 0; i < field_size * field_size; ++i) {
        char* semaphore_name = get_semaphore_name(i, 1, manager_semaphore_name);
        manager_semaphores_first[i] = sem_open(semaphore_name, O_CREAT, 0666, 0);
        free(semaphore_name);

        semaphore_name = get_semaphore_name(i, 2, manager_semaphore_name);
        manager_semaphores_second[i] = sem_open(semaphore_name, O_CREAT, 0666, 0);
        free(semaphore_name);
    }

    manager_semaphores_pointer_first = manager_semaphores_first;
    manager_semaphores_pointer_second = manager_semaphores_second;
    printf("All semaphores are inited\n\n");

    // creating all guns
    for (int i = 0; i < field_size * field_size; ++i) {
        if (first_field[i].type == ALIVE_GUN) {
            if (fork() == 0) {
                signal(SIGINT, prev);
                printf("Gun from first side in coordinate: %d\n", i);
                gun(i, 1, gun_semaphores_first[i], manager_semaphores_first[i]);
                exit(0);
            }
        }
        if (second_field[i].type == ALIVE_GUN) {
            if (fork() == 0) {
                signal(SIGINT, prev);
                printf("Gun from second side in coordinate: %d\n", i);
                gun(i, 2, gun_semaphores_second[i], manager_semaphores_second[i]);
                exit(0);
            }
        }
    }

    manager(gun_semaphores_first, gun_semaphores_second, manager_semaphores_first, manager_semaphores_second);
    destroy();
    return 0;
}
 */