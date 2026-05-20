#include <stdio.h>

int main() {
    int bucket_size, n, outgoing, incoming, store = 0;

    printf("Enter Bucket Size: ");
    scanf("%d", &bucket_size);

    printf("Enter Outgoing Rate: ");
    scanf("%d", &outgoing);

    printf("Enter No of Inputs: ");
    scanf("%d", &n);

    while (n != 0) {
        printf("\nEnter Incoming Packet Size: ");
        scanf("%d", &incoming);

        printf("Incoming Packet Size: %d\n", incoming);

        // Check if the incoming packet fits in the remaining bucket space
        if (incoming <= (bucket_size - store)) {
            store += incoming;
            printf("Bucket Buffer Size %d out of %d\n", store, bucket_size);
        }
        else {
            // Calculate and display dropped packets, then fill the bucket to capacity
            printf("Dropped %d packets\n", incoming - (bucket_size - store));
            store = bucket_size;
            printf("Bucket Buffer Size %d out of %d\n", store, bucket_size);
        }

        // Leak/leakage phase: Remove packets at the constant outgoing rate
        store = store - outgoing;

        // Reset store to 0 if the outgoing rate exceeds the current buffer content
        if (store < 0)
            store = 0;

        printf("After Outgoing: %d packets left out of %d in buffer\n", store, bucket_size);

        n--;
    }

    return 0;
}