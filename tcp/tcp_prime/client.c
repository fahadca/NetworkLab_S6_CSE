#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main(void) {
    int socket_desc;
    struct sockaddr_in server_addr;
    int client_number;
    char server_message[2000];
    
    memset(server_message, '\0', sizeof(server_message));
    
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    
    if(socket_desc < 0) {
        printf("Unable to create socket\n");
        return -1;
    }
    
    printf("Socket created successfully\n");
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(2000);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    // Send connection request to server:
    if(connect(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("Unable to connect\n");
        return -1;
    }
    
    printf("Connected with server successfully\n");

    printf("Enter a number: ");
    scanf("%d", &client_number);

    if(send(socket_desc, &client_number, sizeof(client_number), 0) < 0) {
        printf("Unable to send message\n");
        return -1;
    }

    if(recv(socket_desc, server_message, sizeof(server_message), 0) < 0) {
        printf("Error while receiving server's message\n");
        return -1;
    }
    
    printf("Server's response: %s\n", server_message);

    close(socket_desc);
    return 0;
}

