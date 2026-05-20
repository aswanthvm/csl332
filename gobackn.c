// ================= GO BACK N SERVER =================

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
    printf("Go-Back-N Receiver Waiting...\n");
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
        // Correct frame
        if(frame == expected) {

            printf("Received Frame %d\n",
                    frame);

            expected++;
        }
        // Out of order frame
        else {

            printf("Expected %d But Received %d\n",
                    expected,
                    frame);
        }

        // Send cumulative ACK
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
=================================================================================
// ================= GO BACK N CLIENT =================

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>

int main() {

    int clientSock;
    int totalFrames;
    int windowSize;
    int base = 0;
    int next = 0;
    int ack;

    struct sockaddr_in serverAddr;

    socklen_t addrSize;

    struct timeval tv;

    // Input total frames
    printf("Enter Total Frames: ");
    scanf("%d", &totalFrames);
    // Input window size
    printf("Enter Window Size: ");
    scanf("%d", &windowSize);
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

    printf("\nStarting Go-Back-N Transmission...\n");

    // Send frames
    while(base < totalFrames) {

        // Send frames inside window
        while(next < base + windowSize &&
              next < totalFrames) {

            printf("Sending Frame %d\n",
                    next);

            sendto(clientSock,
                   &next,
                   sizeof(next),
                   0,
                   (struct sockaddr*)&serverAddr,
                   addrSize);
            next++;
        }
        // Wait for ACK
        if(recvfrom(clientSock,
                    &ack,
                    sizeof(ack),
                    0,
                    (struct sockaddr*)&serverAddr,
                    &addrSize) > 0) {
            // Valid ACK received
            if(ack >= base) {
                printf("ACK %d Received\n",
                        ack);
                // Slide window
                base = ack + 1;
            }
        }
        // Timeout occurs
        else {
            printf("\nTimeout Occurred\n");

            printf("Going Back To Frame %d\n\n",
                    base);
            // Retransmit from base
            next = base;
        }
    }
    printf("All Frames Sent Successfully\n");
    // Close socket
    close(clientSock);
    return 0;
}
