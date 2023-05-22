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
    int shots[num_of_guns];
    int number;

    if ((number = recv(sock, receivedBuffer, num_of_guns * sizeof (int), 0)) < 0) {
        dieWithError("recv() failed or connection closed prematurely");
    }

    printf("%d\n", number);
    for (;number > 0;) {
        if (receivedBuffer[0] >= 0) {
            printf("Shot into: ");
            for (int i = 0; i < number / 4; ++i) {
                printf("%d ", receivedBuffer[i]);
                if (own_field[receivedBuffer[i]].type == ALIVE_GUN) {
                    own_field[receivedBuffer[i]].type = DEAD_GUN;
                }
                if (own_field[receivedBuffer[i]].type == EMPTY_POINT) {
                    own_field[receivedBuffer[i]].type = USED_POINT;
                }
            }
            printf("\n");
        }

        if (!check_status()) {
            shots[0] = -1;
            printf("War losed\n");
            if (send(sock, shots, 1 * sizeof (int), 0) != 1 * sizeof (int)) {
                dieWithError("send() sent a different number of bytes than expected");
            }
            break;
        }

        printf("Making shots\n");
        int *p = shots;
        number = generate_targets(p);
        printf("Shots made\n");
        if (send(sock, shots, number * sizeof (int), 0) != number * sizeof (int)) {
            dieWithError("send() sent a different number of bytes than expected");
        }

        sleep(1);

        if ((number = recv(sock, receivedBuffer, num_of_guns * sizeof (int), 0)) < 0) {
            dieWithError("recv() failed or connection closed prematurely");
        }
    }

    printf("\n");

    if (check_status()) {
        printf("War wined\n");
    }

    close(sock);
    free(own_field);
    free(foreign_field);

    exit(0);
}