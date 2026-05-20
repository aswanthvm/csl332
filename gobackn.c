server:
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>

int main() {
    int s_sock, c_sock;
    struct sockaddr_in server;
    socklen_t len = sizeof(struct sockaddr_in);

    // 1. Setup Socket (TCP)
    s_sock = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    server.sin_port = htons(9009);
    server.sin_addr.s_addr = INADDR_ANY;

    bind(s_sock, (struct sockaddr *)&server, sizeof(server));
    listen(s_sock, 5);
    printf("Server UP - Go-Back-N Protocol\n");

    c_sock = accept(s_sock, NULL, &len);

    int base = 0;      // The oldest unacknowledged frame
    int nextSeq = 0;   // The next frame to be sent
    char msg[50], ack[50];
    fd_set readfds;    // Set of file descriptors for select()
    struct timeval tv; // Timer structure

    while (base < 10) {
        // STEP A: Send window of frames (N = 3)
        // Send frames as long as we haven't exceeded the window size
        while (nextSeq < base + 3 && nextSeq < 10) {
            sprintf(msg, "Frame %d", nextSeq);
            write(c_sock, msg, strlen(msg) + 1);
            printf("Sent: %s\n", msg);
            nextSeq++;
        }

        // STEP B: Start the Timer
        FD_ZERO(&readfds);
        FD_SET(c_sock, &readfds);
        tv.tv_sec = 2;   // 2 second timeout
        tv.tv_usec = 0;

        // STEP C: Wait for ACK or Timeout
        int rv = select(c_sock + 1, &readfds, NULL, NULL, &tv);

        if (rv == 0) {
            // TIMEOUT: No ACK received within 2 seconds
            printf("Timeout! Retransmitting from frame %d\n\n", base);
            nextSeq = base; // GO BACK: Reset nextSeq to the last un-ACKed frame
        } 
        else {
            // ACK RECEIVED: Move the window forward
            read(c_sock, ack, sizeof(ack));
            printf("Received: %s\n\n", ack);
            base++; // Increment base to slide the window
        }
    }

    close(c_sock);
    close(s_sock);
    return 0;
}

=================================================================================
client:
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int c_sock;
    struct sockaddr_in server;

    c_sock = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    server.sin_port = htons(9009);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(c_sock, (struct sockaddr *)&server, sizeof(server));
    printf("Client Active - Receiving Frames\n\n");

    char buff[50], ack[50];
    int expected = 0;        // The sequence number we are waiting for
    int loss_simulated = 0;  // Flag to simulate a one-time network error

    while (expected < 10) {
        read(c_sock, buff, sizeof(buff));

        // Extract the sequence number from the string "Frame X"
        int seq = buff[strlen(buff) - 1] - '0';

        // STEP 1: Check if frame is out-of-order
        if (seq != expected) {
            printf("Discarded out-of-order frame %d\n", seq);
            continue; // Skip sending ACK, wait for server timeout
        }

        // STEP 2: Simulate a one-time ACK loss for Frame 8
        if (seq == 8 && loss_simulated == 0) {
            printf("Simulating loss of ACK for frame 8\n\n");
            loss_simulated = 1;
            continue; // Don't send ACK; forces Server to "Go Back"
        }

        // STEP 3: Successful Frame Receipt
        printf("Received: %s\n", buff);
        sprintf(ack, "ACK %d", seq);
        write(c_sock, ack, strlen(ack) + 1);
        printf("Sent: %s\n\n", ack);

        expected++; // Increment the number we expect next
    }

    close(c_sock);
    return 0;
}