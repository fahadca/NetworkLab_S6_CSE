#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>

#define BUFFER_SIZE 80
#define SERVER_PORT 8080
#define SOCKET_ADDRESS struct sockaddr

void sendAcknowledgement(int socketDescriptor, int acknowledgement) {
    char buffer[BUFFER_SIZE];
    sprintf(buffer, "%d", acknowledgement);
    send(socketDescriptor, buffer, strlen(buffer), 0);
}

int main() {
    int socketDescriptor, connectionDescriptor, clientLength;
    struct sockaddr_in serverAddress, clientAddress;

    // Create socket
    if((socketDescriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Socket creation failed\n");
        exit(0);
    }
    else
        printf("Socket created successfully\n");

    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(SERVER_PORT);

    if (bind(socketDescriptor, (SOCKET_ADDRESS *) &serverAddress, sizeof(serverAddress)) < 0) {
        perror("Bind failed. Error");
        return 1;
    }
    printf("Bind done\n");

    listen(socketDescriptor, 5);
    printf("Server listening .. \n");

    clientLength = sizeof(clientAddress);
    connectionDescriptor = accept(socketDescriptor, (SOCKET_ADDRESS *) &clientAddress, &clientLength);

    char buffer[BUFFER_SIZE];
    int frameNumber, randomChoice, acknowledgement, expectedFrame = 0;

    while(1) {
        sleep(1);
        memset(buffer, 0, BUFFER_SIZE);
        recv(connectionDescriptor, buffer, BUFFER_SIZE, 0);

        if(strcmp("EXIT", buffer) == 0) {
            printf("Exiting\n");
            exit(0);
        }

        frameNumber = atoi(buffer);

        if(frameNumber != expectedFrame) {
            printf("Frame %d discarded\nAcknowledgement sent: %d\n", frameNumber, acknowledgement);
            sendAcknowledgement(connectionDescriptor, acknowledgement);
            continue;
        }

        randomChoice = rand() % 3;

        switch(randomChoice) {
            case 0:
                printf("Frame %d not received\n", frameNumber);
                break;
            case 1:
                acknowledgement = frameNumber;
                sleep(2);
                printf("Frame %d received\nAcknowledgement sent: %d\n", frameNumber, acknowledgement);
                sendAcknowledgement(connectionDescriptor, acknowledgement);
                expectedFrame = acknowledgement + 1;
                break;
            case 2:
                acknowledgement = frameNumber;
                printf("Frame %d received\nAcknowledgement sent: %d\n", frameNumber, acknowledgement);
                sendAcknowledgement(connectionDescriptor, acknowledgement);
                expectedFrame = acknowledgement + 1;
                break;
        }
    }

    close(socketDescriptor);

    return 0;
}
