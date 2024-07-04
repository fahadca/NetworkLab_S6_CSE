#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <sys/time.h>

#define BUFFER_SIZE 80
#define SERVER_PORT 8080
#define SOCKET_ADDRESS struct sockaddr

void sendData(int socketDescriptor, int numberOfFrames, int windowSize) {
    char buffer[BUFFER_SIZE];
    int acknowledgement, i = 0, bytesSent, startWindow = 0, endWindow = windowSize - 1, j, timeoutFlag = 0;
    struct timeval timeout;
    timeout.tv_sec = 3;
    timeout.tv_usec = 0;
    
    if(setsockopt(socketDescriptor, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)) < 0)
        perror("setsockopt(SO_RCVTIMEO) failed");

    for(i = 0; i < numberOfFrames && i <= endWindow; i++) {
        snprintf(buffer, sizeof(buffer), "%d", i);
        bytesSent = send(socketDescriptor, buffer, sizeof(buffer), 0);
        printf("Frame %d sent\n", i);
    }
    
    while(1) {
        if(endWindow - startWindow != windowSize - 1 && timeoutFlag == 0 && i != numberOfFrames) {
            snprintf(buffer, sizeof(buffer), "%d", i);
            bytesSent = send(socketDescriptor, buffer, sizeof(buffer), 0);
            printf("Frame %d sent\n", i);
            endWindow++;
            i++;
        }
        
        timeoutFlag = 0;
        int bytesReceived = recv(socketDescriptor, buffer, BUFFER_SIZE, 0);
        acknowledgement = atoi(buffer);
        
        if(bytesReceived > 0) {
            if(acknowledgement + 1 == numberOfFrames) {
                printf("Ack received %d\nEXIT\n", acknowledgement);
                strcpy(buffer, "EXIT");
                bytesSent = send(socketDescriptor, buffer, sizeof(buffer), 0);
                return;
            }
            
            if(acknowledgement == startWindow) {
                startWindow++;
                printf("Ack received %d\n", acknowledgement);
            }
        } else {
            printf("Ack not received for %d\nRESENDING ...\n", startWindow);
            
            for(j = startWindow; j < numberOfFrames && j < startWindow + windowSize; j++) {
                snprintf(buffer, sizeof(buffer), "%d", j);
                bytesSent = send(socketDescriptor, buffer, sizeof(buffer), 0);
                printf("Frame %d sent\n", j);
            }
            timeoutFlag = 1;
        }
    }
}

int main() {
    int socketDescriptor, numberOfFrames, windowSize;
    struct sockaddr_in serverAddress;
    
    socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    
    if(socketDescriptor < 0) {
        printf("Socket creation failed\n");
        exit(0);
    }
    
    printf("Socket created\n");
    
    memset(&serverAddress, 0, sizeof(serverAddress));
    
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(SERVER_PORT);
    
    if(connect(socketDescriptor, (SOCKET_ADDRESS*)&serverAddress, sizeof(serverAddress)) != 0) {
        printf("Connection failed\n");
        exit(0);
    }
    
    printf("Connected successfully\n");
    
    printf("Enter the number of frames: ");
    scanf("%d", &numberOfFrames);
    
    printf("Enter the window size: ");
    scanf("%d", &windowSize);
    
    sendData(socketDescriptor, numberOfFrames, windowSize);
    
    close(socketDescriptor);
    
    return 0;
}
