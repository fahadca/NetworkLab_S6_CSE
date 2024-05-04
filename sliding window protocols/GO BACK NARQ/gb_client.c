#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_BUFFER_SIZE 80
#define PORT_NUMBER 8080
#define SA struct sockaddr

// Function to handle communication with the server
void sendData(int sockfd, int totalFrames, int windowSize) {
    char buffer[MAX_BUFFER_SIZE];
    int ack, i = 0, j, n, k, windowStart = 0, windowEnd = windowSize - 1, flag = 0;
    struct timeval timeout;

    //timeout.tv_sec = 3;
    //timeout.tv_usec = 0;

    // Set timeout for receiving acknowledgment
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout)) < 0) {
        perror("setsockopt(SO_RCVTIMEO) failed");
    }

    // Sending frames
    for (i = 0; i < totalFrames && i <= windowEnd; i++) {
        bzero(buffer, sizeof(buffer));
        snprintf(buffer, sizeof(buffer), "%d", i);
        k = send(sockfd, buffer, sizeof(buffer), 0);
        printf("Frame %d sent\n", i);
    }

    // Receiving acknowledgments
    while (1) {
        // If there's space in the window and frames left to send
        if (windowEnd - windowStart != windowSize - 1 && flag == 0 && i != totalFrames) {
            bzero(buffer, sizeof(buffer));
            snprintf(buffer, sizeof(buffer), "%d", i);
            k = send(sockfd, buffer, sizeof(buffer), 0); // This line sends the frame
            printf("Frame %d sent -\n", i);
            windowEnd++;
            i++;
        }
        flag = 0;
        bzero(buffer, sizeof(buffer));
        n = recv(sockfd, buffer, MAX_BUFFER_SIZE, 0);
        ack = atoi(buffer);

        if (n > 0) {
            if (ack + 1 == totalFrames) {
                printf("Acknowledgement received:%d\nExit\n", ack);
                bzero(buffer, sizeof(buffer));
                strcpy(buffer, "Exit");
                k = send(sockfd, buffer, sizeof(buffer), 0);
                break;
            }
            if (ack == windowStart) {
                windowStart++;
                printf("Acknowledgement received:%d\n", ack);
            }
        } else {
            printf("Acknowledgement not received for %d\nResending frames\n", windowStart);
            for (j = windowStart; j < totalFrames && j < windowStart + windowSize; j++) {
                bzero(buffer, sizeof(buffer));
                snprintf(buffer, sizeof(buffer), "%d", j);
                k = send(sockfd, buffer, sizeof(buffer), 0);
                printf("Frame %d sent\n", j);
            }
            flag = 1;
        }
    }
}

int main() {
    int sockfd, totalFrames, windowSize;

    struct sockaddr_in serverAddress;

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("Socket creation failed\n");
        exit(0);
    }
    printf("Socket successfully created\n");

    // Initialize server address
    bzero(&serverAddress, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddress.sin_port = htons(PORT_NUMBER);

    // Connect to the server
    if (connect(sockfd, (SA *)&serverAddress, sizeof(serverAddress)) != 0) {
        printf("Connection with the server failed\n");
        exit(0);
    }
    printf("Connected to the server\n");

    // Input total frames and window size
    printf("Enter the number of frames: ");
    scanf("%d", &totalFrames);
    printf("Enter the window size: ");
    scanf("%d", &windowSize);

    // Send data using sliding window protocol
    sendData(sockfd, totalFrames, windowSize);

    // Close socket
    close(sockfd);

    return 0;
}

