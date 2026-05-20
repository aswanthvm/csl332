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

    FILE *fp;

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

    printf("FTP Server Waiting...\n");

    addrSize = sizeof(clientAddr);

    // Accept client connection
    clientSock = accept(serverSock,
                        (struct sockaddr*)&clientAddr,
                        &addrSize);

    // Receive filename
    recv(clientSock,
         buffer,
         sizeof(buffer),
         0);

    printf("Requested File: %s\n", buffer);

    // Open file
    fp = fopen(buffer, "r");

    // File not found
    if (fp == NULL) {

        strcpy(buffer, "File Not Found");

        send(clientSock,
             buffer,
             strlen(buffer),
             0);
    }
    else {

        // Read and send file line by line
        while (fgets(buffer,
                     sizeof(buffer),
                     fp) != NULL) {

            send(clientSock,
                 buffer,
                 strlen(buffer),
                 0);
        }

        fclose(fp);
    }

    // Close sockets
    close(clientSock);
    close(serverSock);

    return 0;
}

====================================================
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

    serverAddr.sin_addr.s_addr =
        inet_addr("127.0.0.1");

    // Connect to server
    connect(clientSock,
            (struct sockaddr*)&serverAddr,
            sizeof(serverAddr));

    // Input filename
    printf("Enter File Name: ");

    scanf("%s", buffer);

    // Send filename
    send(clientSock,
         buffer,
         strlen(buffer),
         0);

    printf("\nFile Content:\n\n");

    // Receive file data
    while (recv(clientSock,
                buffer,
                sizeof(buffer),
                0) > 0) {

        printf("%s", buffer);

        memset(buffer, 0, sizeof(buffer));
    }

    // Close socket
    close(clientSock);

    return 0;
}
