server:
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {

    int serverSock;
    char buffer[1024];

    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrSize;

    // Create UDP socket
    serverSock = socket(AF_INET, SOCK_DGRAM, 0);

    // Configure server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(6265);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind socket
    bind(serverSock,
         (struct sockaddr*)&serverAddr,
         sizeof(serverAddr));

    printf("UDP Server Waiting...\n");

    // Receive message from client
    addrSize = sizeof(clientAddr);

    recvfrom(serverSock,
             buffer,
             sizeof(buffer),
             0,
             (struct sockaddr*)&clientAddr,
             &addrSize);

    printf("Client: %s\n", buffer);

    // Send reply to client
    strcpy(buffer, "Hello Client");

    sendto(serverSock,
           buffer,
           strlen(buffer),
           0,
           (struct sockaddr*)&clientAddr,
           addrSize);

    // Close socket
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
    socklen_t addrSize;

    // Create UDP socket
    clientSock = socket(AF_INET, SOCK_DGRAM, 0);

    // Configure server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(6265);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Read message
    printf("Enter message: ");
    fgets(buffer, sizeof(buffer), stdin);

    // Send message to server
    sendto(clientSock,
           buffer,
           strlen(buffer),
           0,
           (struct sockaddr*)&serverAddr,
           sizeof(serverAddr));

    // Receive reply from server
    addrSize = sizeof(serverAddr);

    recvfrom(clientSock,
             buffer,
             sizeof(buffer),
             0,
             (struct sockaddr*)&serverAddr,
             &addrSize);

    printf("Server: %s\n", buffer);

    // Close socket
    close(clientSock);

    return 0;
}