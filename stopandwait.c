server:
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {

    int serverSock;

    char buffer[1024];

    struct sockaddr_in serverAddr,
                       clientAddr;

    socklen_t addrSize;

    int k = 5, m = 0;

    // Create UDP socket
    serverSock = socket(AF_INET,
                        SOCK_DGRAM,
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

    printf("Receiver Waiting...\n");

    addrSize = sizeof(clientAddr);

    // Receive frames
    while(k != 0) {

        // Receive frame
        recvfrom(serverSock,
                 buffer,
                 sizeof(buffer),
                 0,
                 (struct sockaddr*)&clientAddr,
                 &addrSize);

        printf("Received: %s\n", buffer);

        // Send ACK normally
        if(m % 2 == 0) {

            strcpy(buffer, "ACK");

            sendto(serverSock,
                   buffer,
                   strlen(buffer),
                   0,
                   (struct sockaddr*)&clientAddr,
                   addrSize);

            printf("ACK Sent\n");
        }

        // Simulate ACK loss
        else {

            printf("ACK Lost\n");

            sleep(3);

            strcpy(buffer, "ACK");

            sendto(serverSock,
                   buffer,
                   strlen(buffer),
                   0,
                   (struct sockaddr*)&clientAddr,
                   addrSize);

            printf("ACK Retransmitted\n");
        }

        k--;
        m++;
    }

    // Close socket
    close(serverSock);

    return 0;
}
========================================================================
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

    int k = 5, m = 0;

    // Create UDP socket
    clientSock = socket(AF_INET,
                        SOCK_DGRAM,
                        0);

    // Configure server
    serverAddr.sin_family = AF_INET;

    serverAddr.sin_port = htons(6265);

    serverAddr.sin_addr.s_addr =
            inet_addr("127.0.0.1");

    addrSize = sizeof(serverAddr);

    // Send frames
    while(k != 0) {

        // Send frame normally
        if(m % 2 == 0) {

            strcpy(buffer, "FRAME");

            sendto(clientSock,
                   buffer,
                   strlen(buffer),
                   0,
                   (struct sockaddr*)&serverAddr,
                   addrSize);

            printf("Frame Sent\n");
        }

        // Simulate frame loss
        else {

            printf("Frame Lost\n");

            sleep(3);

            strcpy(buffer, "FRAME");

            sendto(clientSock,
                   buffer,
                   strlen(buffer),
                   0,
                   (struct sockaddr*)&serverAddr,
                   addrSize);

            printf("Frame Retransmitted\n");
        }

        // Receive ACK
        recvfrom(clientSock,
                 buffer,
                 sizeof(buffer),
                 0,
                 (struct sockaddr*)&serverAddr,
                 &addrSize);

        printf("Received: %s\n", buffer);

        k--;
        m++;
    }

    // Close socket
    close(clientSock);

    return 0;
}