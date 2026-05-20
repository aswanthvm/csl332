server:
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

void resend_message(int msg_num, int c_sock) {
    char buff[60];
    snprintf(buff, sizeof(buff), "server message: %d", msg_num);
    printf("NACK received! Resending: %s\n", buff);
    write(c_sock, buff, sizeof(buff));
    usleep(1000);
}

int main() {
    int s_sock, c_sock;
    struct sockaddr_in server, client;
    socklen_t addr_len = sizeof(client);

    s_sock = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    server.sin_port = htons(9009);
    server.sin_addr.s_addr = INADDR_ANY;

    bind(s_sock, (struct sockaddr *)&server, sizeof(server));
    listen(s_sock, 10);
    printf("Server Up - Selective Repeat ARQ\n");

    c_sock = accept(s_sock, (struct sockaddr *)&client, &addr_len);

    int total_msgs = 9;
    int window_size = 3;
    int sent_msgs = 0;
    char feedback[50];

    while (sent_msgs < total_msgs) {
        // 1. Send the Window (3 messages)
        for (int i = sent_msgs; i < sent_msgs + window_size && i < total_msgs; i++) {
            char buff[50];
            snprintf(buff, sizeof(buff), "server message: %d", i);
            printf("Message sent to client: %s\n", buff);
            write(c_sock, buff, sizeof(buff));
            usleep(1000);
        }

        // 2. Read Feedback for each message in the window
        for (int i = sent_msgs; i < sent_msgs + window_size && i < total_msgs; i++) {
            bzero(feedback, sizeof(feedback));
            read(c_sock, feedback, sizeof(feedback));

            if (feedback[0] == 'n') { // If it starts with 'n' for nack
                int nack_num = feedback[4] - '0'; // Get msg number
                resend_message(nack_num, c_sock);
                i--; // Decrease loop to wait for ACK of resent message
            } else {
                printf("Feedback: %s\n", feedback);
            }
        }
        sent_msgs += window_size; // Move to next window
    }

    close(c_sock);
    close(s_sock);
    return 0;
}
===========================================================================
client:
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

int is_faulty() {
    int d = rand() % 4; // 0, 1, 2, or 3
    return (d > 2);     // If 3, return true (faulty)
}

int main() {
    srand(time(0));
    int sockfd;
    struct sockaddr_in server;
    char buffer[100], response[50];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    server.sin_port = htons(9009);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(sockfd, (struct sockaddr *)&server, sizeof(server));
    printf("Client with Selective Repeat\n");

    int count = 0;
    while (count < 9) {
        bzero(buffer, sizeof(buffer));
        read(sockfd, buffer, sizeof(buffer));

        // Extract message number (the last character)
        int msg_num = buffer[strlen(buffer) - 1] - '0';
        printf("Message received: %s\n", buffer);

        if (is_faulty()) {
            snprintf(response, sizeof(response), "nack%d", msg_num);
            printf("Negative ACK sent for message %d\n", msg_num);
        } else {
            snprintf(response, sizeof(response), "ack%d", msg_num);
            printf("ACK sent for message %d\n", msg_num);
            count++;
        }

        write(sockfd, response, sizeof(response));
        usleep(1000);
    }

    close(sockfd);
    return 0;
}