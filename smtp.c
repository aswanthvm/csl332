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
    serverSock = socket(AF_INET,
                        SOCK_STREAM,
                        0);

    // Configure server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(6265);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind socket
    bind(serverSock,
         (struct sockaddr*)&serverAddr,
         sizeof(serverAddr));

    // Listen for client
    listen(serverSock, 5);

    printf("SMTP Server Waiting...\n");

    addrSize = sizeof(clientAddr);

    // Accept client connection
    clientSock = accept(serverSock,
                        (struct sockaddr*)&clientAddr,
                        &addrSize);

    // Receive HELO
    recv(clientSock,
         buffer,
         sizeof(buffer),
         0);

    printf("Client: %s\n", buffer);

    // Send reply
    strcpy(buffer, "250 OK");

    send(clientSock,
         buffer,
         strlen(buffer),
         0);

    // Receive MAIL FROM
    recv(clientSock,
         buffer,
         sizeof(buffer),
         0);

    printf("Client: %s\n", buffer);

    send(clientSock,
         "250 OK",
         6,
         0);

    // Receive RCPT TO
    recv(clientSock,
         buffer,
         sizeof(buffer),
         0);

    printf("Client: %s\n", buffer);

    send(clientSock,
         "250 OK",
         6,
         0);

    // Receive DATA
    recv(clientSock,
         buffer,
         sizeof(buffer),
         0);

    printf("Client: %s\n", buffer);

    send(clientSock,
         "354 Start Mail",
         15,
         0);

    // Receive Mail Body
    recv(clientSock,
         buffer,
         sizeof(buffer),
         0);

    printf("Mail: %s\n", buffer);

    // Receive QUIT
    recv(clientSock,
         buffer,
         sizeof(buffer),
         0);

    printf("Client: %s\n", buffer);

    // Send closing message
    send(clientSock,
         "221 Bye",
         8,
         0);

    // Close sockets
    close(clientSock);
    close(serverSock);

    return 0;
}
=====================================
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
    clientSock = socket(AF_INET,
                        SOCK_STREAM,
                        0);

    // Configure server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(6265);
    serverAddr.sin_addr.s_addr =inet_addr("127.0.0.1");

    // Connect to server
    connect(clientSock,
            (struct sockaddr*)&serverAddr,
            sizeof(serverAddr));

    // Send HELO
    strcpy(buffer, "HELO");

    send(clientSock,
         buffer,
         strlen(buffer),
         0);

    recv(clientSock,
         buffer,
         sizeof(buffer),
         0);

    printf("Server: %s\n", buffer);

    // Send MAIL FROM
    strcpy(buffer,
           "MAIL FROM:user@gmail.com");

    send(clientSock,
         buffer,
         strlen(buffer),
         0);

    recv(clientSock,
         buffer,
         sizeof(buffer),
         0);

    printf("Server: %s\n", buffer);

    // Send RCPT TO
    strcpy(buffer,
           "RCPT TO:test@gmail.com");

    send(clientSock,
         buffer,
         strlen(buffer),
         0);

    recv(clientSock,
         buffer,
         sizeof(buffer),
         0);

    printf("Server: %s\n", buffer);

    // Send DATA
    strcpy(buffer, "DATA");

    send(clientSock,
         buffer,
         strlen(buffer),
         0);

    recv(clientSock,
         buffer,
         sizeof(buffer),
         0);

    printf("Server: %s\n", buffer);

    // Send Mail Body
    strcpy(buffer,
           "Hello This is SMTP Mail");

    send(clientSock,
         buffer,
         strlen(buffer),
         0);

    // Send QUIT
    strcpy(buffer, "QUIT");

    send(clientSock,
         buffer,
         strlen(buffer),
         0);

    recv(clientSock,
         buffer,
         sizeof(buffer),
         0);

    printf("Server: %s\n", buffer);

    // Close socket
    close(clientSock);

    return 0;
}