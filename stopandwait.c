// ================= STOP AND WAIT SERVER =================

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {

    int serverSock;
    int expected = 0;
    int frame, ack;

    struct sockaddr_in serverAddr,
                       clientAddr;
    socklen_t addrSize;

    // Create UDP socket
    serverSock = socket(AF_INET,
                        SOCK_DGRAM,
                        0);

    // Configure server
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(9002);
    serverAddr.sin_addr.s_addr =
            INADDR_ANY;

    // Bind socket
    bind(serverSock,
         (struct sockaddr*)&serverAddr,
         sizeof(serverAddr));

    // Random generator
    srand(time(NULL));
    printf("Receiver Waiting...\n");
    addrSize = sizeof(clientAddr);

    // Receive frames continuously
    while(1) {
        // Receive frame
        recvfrom(serverSock,
                 &frame,
                 sizeof(frame),
                 0,
                 (struct sockaddr*)&clientAddr,
                 &addrSize);
        // Simulate frame loss
        if(rand() % 10 < 2) {
            printf("Frame %d Lost\n",
                    frame);
            continue;
        }

        // Correct frame received
        if(frame == expected) {

            printf("Received Frame %d\n",
                    frame);

            expected++;
        }

        // Duplicate frame
        else {

            printf("Duplicate Frame %d\n",
                    frame);
        }

        // Send ACK
        ack = expected - 1;

        sendto(serverSock,
               &ack,
               sizeof(ack),
               0,
               (struct sockaddr*)&clientAddr,
               addrSize);

        printf("ACK %d Sent\n\n",
                ack);
    }

    // Close socket
    close(serverSock);

    return 0;
}
========================================================================
// ================= STOP AND WAIT CLIENT =================

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>

int main() {

    int clientSock;

    int totalFrames;

    int frame = 0;

    int ack;

    struct sockaddr_in serverAddr;

    socklen_t addrSize;

    struct timeval tv;

    // Input total frames
    printf("Enter Total Frames: ");

    scanf("%d", &totalFrames);

    // Create UDP socket
    clientSock = socket(AF_INET,
                        SOCK_DGRAM,
                        0);

    // Configure server
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(9002);
    serverAddr.sin_addr.s_addr =
            inet_addr("127.0.0.1");
    addrSize = sizeof(serverAddr);

    // Set timeout = 2 seconds
    tv.tv_sec = 2;

    tv.tv_usec = 0;

    setsockopt(clientSock,
               SOL_SOCKET,
               SO_RCVTIMEO,
               &tv,
               sizeof(tv));
    printf("\nStarting Transmission...\n");
    // Send frames
    while(frame < totalFrames) {

        // Send frame
        printf("Sending Frame %d\n",
                frame);
        sendto(clientSock,
               &frame,
               sizeof(frame),
               0,
               (struct sockaddr*)&serverAddr,
               addrSize);
        // Wait for ACK
        if(recvfrom(clientSock,
                    &ack,
                    sizeof(ack),
                    0,
                    (struct sockaddr*)&serverAddr,
                    &addrSize) > 0) {
            // Correct ACK received
            if(ack == frame) {
                printf("ACK %d Received\n\n",
                        ack);
                frame++;
            }
        }
        // Timeout occurs
        else {
            printf("Timeout\n");
            printf("Resending Frame %d\n\n",
                    frame);
        }
    }
    printf("All Frames Sent Successfully\n");
    // Close socket
    close(clientSock);

    return 0;
}
