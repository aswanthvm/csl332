// ================= SELECTIVE REPEAT SERVER =================

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int serverSock;
    int frame;
    int expected = 0;
    int received[100] = {0};
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

    srand(time(NULL));
    printf("Selective Repeat Receiver Waiting...\n");
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
        // New frame
        if(received[frame] == 0) {

            received[frame] = 1;

            printf("Received Frame %d\n",
                    frame);
        }

        // Duplicate frame
        else {

            printf("Duplicate Frame %d\n",
                    frame);
        }

        // Send ACK for that frame
        sendto(serverSock,
               &frame,
               sizeof(frame),
               0,
               (struct sockaddr*)&clientAddr,
               addrSize);

        printf("ACK %d Sent\n",
                frame);

        // Slide receiver window
        if(frame == expected) {

            while(received[expected] == 1) {

                expected++;
            }

            printf("Next Expected Frame %d\n\n",
                    expected);
        }
    }

    // Close socket
    close(serverSock);

    return 0;
}
===========================================================================
// ================= SELECTIVE REPEAT CLIENT =================

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

    int ackReceived[100] = {0};

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
    printf("\nStarting Selective Repeat Transmission...\n");
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

            printf("ACK %d Received\n",
                    ack);
            // Mark ACK received
            ackReceived[ack] = 1;
            // Slide sender window
            if(ack == base) {

                while(ackReceived[base] == 1) {
                    base++;
                }
            }
        }
        // Timeout occurs
        else {
            printf("\nTimeout Occurred\n");
            // Retransmit only lost frames
            for(int i = base; i < next; i++) {
                if(ackReceived[i] == 0) {
                    printf("Resending Frame %d\n",
                            i);
                    sendto(clientSock,
                           &i,
                           sizeof(i),
                           0,
                           (struct sockaddr*)&serverAddr,
                           addrSize);
                }
            }
            printf("\n");
        }
    }
    printf("All Frames Sent Successfully\n");
    // Close socket
    close(clientSock);

    return 0;
}
