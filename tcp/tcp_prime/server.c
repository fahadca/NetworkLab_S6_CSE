#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

// Function to check if a number is prime
int isPrime(int num) {
    if (num <= 1) return 0;  // 0 and 1 are not prime
    for (int i = 2; i * i <= num; i++) {
        if (num % i == 0) return 0;  // If divisible by any number other than 1 and itself
    }
    return 1;  // If not divisible by any number other than 1 and itself
}

int main(void) {
    int socket_desc, client_sock, client_size;
    struct sockaddr_in server_addr, client_addr;
    int client_number;
    char server_message[2000];

    memset(server_message, '\0', sizeof(server_message));
    
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_desc < 0) {
        printf("Error while creating socket\n");
        return -1;
    }
    
    printf("Socket created successfully\n");

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(2000);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if(bind(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("Couldn't bind to the port\n");
        return -1;
    }

    printf("Done with binding\n");

    if(listen(socket_desc, 1) < 0) {
        printf("Error while listening\n");
        return -1;
    }

    printf("\nListening for incoming connections.....\n");

    client_size = sizeof(client_addr);
    client_sock = accept(socket_desc, (struct sockaddr*)&client_addr, &client_size);
    if(client_sock < 0) {
        printf("Can't accept\n");
        return -1;
    }

    printf("Client connected at IP: %s and port: %i\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    if(recv(client_sock, &client_number, sizeof(client_number), 0) < 0) {
        printf("Couldn't receive\n");
        return -1;
    }

    printf("Number from client: %d\n", client_number);

    // Check if the number is prime
    int prime = isPrime(client_number);

    if(prime) {
        strcpy(server_message, "The number is prime.");
    } else {
        strcpy(server_message, "The number is not prime.");
    }

    if(send(client_sock, server_message, strlen(server_message), 0) < 0) {
        printf("Can't send\n");
        return -1;
    }

    close(client_sock);
    close(socket_desc);

    return 0;
}

