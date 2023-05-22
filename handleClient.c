//
// Created by milena on 22.05.2023.
//

#include "helper.h"

#define RECEIVED_BUFFER_SIZE 50 * sizeof (int)/* Size of receive buffer */

void handleClient(int clientSocket) {
    char buffer[RECEIVED_BUFFER_SIZE];
    int receivedMessageSize;

    if (send(clientSocket, buffer, receivedMessageSize, 0) != receivedMessageSize) {
        dieWithError("send() failed");
    }

    /* Receive message from client */
    if ((receivedMessageSize = recv(clientSocket, buffer, RECEIVED_BUFFER_SIZE, 0)) < 0) {
        dieWithError("recv() failed");
    }

    shared

    /* Send received string and receive again until end of transmission */
    while (receivedMessageSize > 0) { /* zero indicates end of transmission */
        /* Echo message back to client */

        /* See if there is more data to receive */
        if ((receivedMessageSize = recv(clientSocket, buffer, RECEIVED_BUFFER_SIZE, 0)) < 0) {
            dieWithError("recv() failed");
        }
    }
}
