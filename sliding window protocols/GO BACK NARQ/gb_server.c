#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_BUFFER_SIZE 80
#define PORT_NUMBER 8080
#define SOCKET_ADDRESS struct sockaddr

// Function to handle communication with the client
void handleCommunication(int connectionSocket) {
    char buffer[MAX_BUFFER_SIZE]; // Buffer for receiving and sending messages
    int frame, choice, ack, nextExpectedFrame = 0; // Variables for frame, choice, acknowledgement, and next expected frame

    // Loop to continuously receive and process frames
    while (1) {
        sleep(1); // Simulating processing time

        // Clear buffer
        bzero(buffer, MAX_BUFFER_SIZE);

        // Receive frame from client
        recv(connectionSocket, buffer, MAX_BUFFER_SIZE, 0);

        // Check if received frame is "Exit"
        if (strcmp("Exit", buffer) == 0) {
            printf("Exit\n");
            exit(0); // Exit the program
        }

        frame = atoi(buffer);

        // Check if received frame is the expected next frame
        if (frame != nextExpectedFrame) {
            printf("Frame %d discarded\nAcknowledgement sent:%d\n", frame, ack);
            bzero(buffer, MAX_BUFFER_SIZE);
            snprintf(buffer, sizeof(buffer), "%d", ack);
            send(connectionSocket, buffer, sizeof(buffer), 0);
            continue;
        }

        choice = rand() % 3;

        switch (choice) {
            case 0:
                printf("Frame %d not received\n", frame);
                break;
            case 1:
                ack = frame;
                sleep(2);
                printf("Frame %d received\nAcknowledgement sent:%d\n", frame, ack);
                bzero(buffer, MAX_BUFFER_SIZE);
                snprintf(buffer, sizeof(buffer), "%d", ack);
                send(connectionSocket, buffer, sizeof(buffer), 0);
                nextExpectedFrame = ack + 1;
                break;
            case 2:
                ack = frame;
                sleep(2);
                printf("Frame %d received\nAcknowledgement sent:%d\n", frame, ack);
                bzero(buffer, MAX_BUFFER_SIZE);
                snprintf(buffer, sizeof(buffer), "%d", ack);
                send(connectionSocket, buffer, sizeof(buffer), 0);
                nextExpectedFrame = ack + 1;
                break;
        }
    }
}

int main() {
    int serverSocket, connectionSocket, addressLength;
    struct sockaddr_in serverAddress, clientAddress;

    // Create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        printf("Socket creation failed\n");
        exit(0);
    }
    printf("Socket successfully created\n");

    // Initialize server address
    bzero(&serverAddress, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(PORT_NUMBER);

    // Bind the socket
    if (bind(serverSocket, (SOCKET_ADDRESS *)&serverAddress, sizeof(serverAddress)) != 0) {
        printf("Socket bind failed\n");
        exit(0);
    }
    printf("Socket successfully binded\n");

    // Listen for connections
    if ((listen(serverSocket, 5)) != 0) {
        printf("Listen failed\n");
        exit(0);
    }
    printf("Server listening\n");

    addressLength = sizeof(clientAddress);
    // Accept connection from client
    connectionSocket = accept(serverSocket, (SOCKET_ADDRESS *)&clientAddress, &addressLength);
    if (connectionSocket < 0) {
        printf("Server accept failed\n");
        exit(0);
    }
    printf("Server accepted the client\n");

    // Handle communication with client
    handleCommunication(connectionSocket);

    // Close sockets
    close(serverSocket);
    close(connectionSocket);

    return 0;
}

