server:
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {

    int serverSock, clientSock;
    char buffer[1024];

    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrSize;

    // Create TCP socket
    serverSock = socket(AF_INET, SOCK_STREAM, 0);

    // Configure server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind socket
    bind(serverSock,
         (struct sockaddr*)&serverAddr,
         sizeof(serverAddr));

    // Listen for client
    listen(serverSock, 5);

    printf("TCP Server Waiting...\n");

    addrSize = sizeof(clientAddr);

    // Accept client connection
    clientSock = accept(serverSock,
                        (struct sockaddr*)&clientAddr,
                        &addrSize);

    // Receive message from client
    recv(clientSock,
         buffer,
         sizeof(buffer),
         0);

    printf("Client: %s\n", buffer);

    // Send reply
    strcpy(buffer, "Hello Client");

    send(clientSock,
         buffer,
         strlen(buffer),
         0);

    // Close sockets
    close(clientSock);
    close(serverSock);

    return 0;
}
======================================================================
client:
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {

    int clientSock;
    char buffer[1024];

    struct sockaddr_in serverAddr;

    // Create TCP socket
    clientSock = socket(AF_INET, SOCK_STREAM, 0);

    // Configure server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);

    serverAddr.sin_addr.s_addr =
        inet_addr("127.0.0.1");

    // Connect to server
    connect(clientSock,
            (struct sockaddr*)&serverAddr,
            sizeof(serverAddr));

    // Send message to server
    strcpy(buffer, "Hello Server");

    send(clientSock,
         buffer,
         strlen(buffer),
         0);

    // Receive reply from server
    recv(clientSock,
         buffer,
         sizeof(buffer),
         0);

    printf("Server: %s\n", buffer);

    // Close socket
    close(clientSock);

    return 0;
}
