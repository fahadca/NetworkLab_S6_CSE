#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main(void) {
    // Initialize variables
    int socket_desc, timer, frame;
    char server_message[2000], client_message[2000];

    // Clean buffers
    memset(server_message, '\0', sizeof(server_message));
    memset(client_message, '\0', sizeof(client_message));

    // Create socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc < 0) {
        printf("Unable to create socket\n");
        return -1;
    }
    printf("Socket created successfully\n");

    // Set up server address
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Connect to server
    if (connect(socket_desc, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0) {
        printf("Unable to connect\n");
        return -1;
    }
    printf("Connected with server successfully\n");

    // Input total frames
    printf("Enter the total frames: ");
    scanf("%d", &frame);

    // Frame transmission loop
    while (frame > 0) {
        // Get frame message from user
        printf("\nEnter frame message: ");
        scanf("%s", client_message);

        // Send frame message to server
    x:
        timer = 10; // Reset timer for each frame
        if (send(socket_desc, client_message, strlen(client_message), 0) < 0) {
            printf("Unable to send message\n");
            return -1;
        }

        // Receive acknowledgment from server
    y:
        // Wait for acknowledgment or timeout
        if (timer > 0) {
            if (recv(socket_desc, server_message, sizeof(server_message), 0) < 0) {
                printf("Error while receiving server's msg\n");
                timer--;
                goto y; // Retry receiving acknowledgment
            }
            if (strcmp(server_message, "Y") == 0 || strcmp(server_message, "y") == 0) {
                printf("\nServer acknowledged");
                timer = 0; // Reset timer since acknowledgment received
                frame--; // Decrement frame count
            } else if (strcmp(server_message, "N") == 0 || strcmp(server_message, "n") == 0) {
                printf("\nWaiting...");
                timer--; // Decrement timer for waiting
                goto y; // Retry receiving acknowledgment
            }
        } else {
            printf("\nServer did not acknowledge\nResending....");
            goto x; // Resend frame if timeout
        }
    }

    // Close socket
    close(socket_desc);
    return 0;
}

