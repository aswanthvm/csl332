server:
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {

    int serverSock, clientSock;

    int num, fact = 1, i;

    struct sockaddr_in serverAddr;

    // Create TCP socket
    serverSock = socket(AF_INET,
                        SOCK_STREAM,
                        0);

    // Configure server
    serverAddr.sin_family = AF_INET;

    serverAddr.sin_port = htons(6265);

    serverAddr.sin_addr.s_addr =
            INADDR_ANY;

    // Bind socket
    bind(serverSock,
         (struct sockaddr*)&serverAddr,
         sizeof(serverAddr));

    // Listen for client
    listen(serverSock, 5);

    printf("Factorial Server Waiting...\n");

    // Accept client
    clientSock = accept(serverSock,
                        NULL,
                        NULL);

    // Receive number
    recv(clientSock,
         &num,
         sizeof(num),
         0);

    printf("Number Received: %d\n", num);

    // Calculate factorial
    for(i = 1; i <= num; i++) {

        fact = fact * i;
    }

    printf("Factorial = %d\n", fact);

    // Send factorial
    send(clientSock,
         &fact,
         sizeof(fact),
         0);

    // Close sockets
    close(clientSock);
    close(serverSock);

    return 0;
}
======================================================================================
client:
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {

    int clientSock;

    int num, fact;

    struct sockaddr_in serverAddr;

    // Create TCP socket
    clientSock = socket(AF_INET,
                        SOCK_STREAM,
                        0);

    // Configure server
    serverAddr.sin_family = AF_INET;

    serverAddr.sin_port = htons(6265);

    serverAddr.sin_addr.s_addr =
            inet_addr("127.0.0.1");

    // Connect to server
    connect(clientSock,
            (struct sockaddr*)&serverAddr,
            sizeof(serverAddr));

    // Input number
    printf("Enter Number: ");

    scanf("%d", &num);

    // Send number
    send(clientSock,
         &num,
         sizeof(num),
         0);

    // Receive factorial
    recv(clientSock,
         &fact,
         sizeof(fact),
         0);

    printf("Factorial = %d\n", fact);

    // Close socket
    close(clientSock);

    return 0;
}