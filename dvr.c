#include <stdio.h>

#define INFINITY 9999
#define MAX 10

int cost[MAX][MAX];
int dist[MAX][MAX];
int next_hop[MAX][MAX];

int nodes;


// Function to initialize routing tables
void initialize() {

    for(int i = 0; i < nodes; i++) {

        for(int j = 0; j < nodes; j++) {

            // Copy cost matrix to distance matrix
            dist[i][j] = cost[i][j];

            // Initially next hop is destination itself
            next_hop[i][j] = j;
        }
    }
}


// Function to update shortest paths
void updateRoutes() {

    int updated;

    do {

        updated = 0;

        // Check all routers
        for(int i = 0; i < nodes; i++) {

            for(int j = 0; j < nodes; j++) {

                for(int k = 0; k < nodes; k++) {

                    // Check for shorter path through router k
                    if(dist[i][j] >
                       dist[i][k] + dist[k][j]) {

                        // Update shortest distance
                        dist[i][j] =
                        dist[i][k] + dist[k][j];

                        // Update next hop
                        next_hop[i][j] =
                        next_hop[i][k];

                        // Mark table updated
                        updated = 1;
                    }
                }
            }
        }

    } while(updated); // Repeat until no changes
}


// Function to display routing tables
void display() {

    for(int i = 0; i < nodes; i++) {

        printf("\nRouter %d Routing Table\n",
                i + 1);

        printf("Destination\tCost\tNext Hop\n");

        for(int j = 0; j < nodes; j++) {

            printf("%d\t\t%d\t%d\n",
                    j + 1,
                    dist[i][j],
                    next_hop[i][j] + 1);
        }
    }
}


int main() {

    // Input number of routers
    printf("Enter Number of Routers: ");

    scanf("%d", &nodes);

    // Input cost matrix
    printf("Enter Cost Matrix:\n");

    for(int i = 0; i < nodes; i++) {

        for(int j = 0; j < nodes; j++) {

            scanf("%d", &cost[i][j]);

            // Distance to itself is always 0
            if(i == j) {

                cost[i][j] = 0;
            }
        }
    }

    // Initialize routing tables
    initialize();

    // Update shortest paths
    updateRoutes();

    // Display final routing tables
    display();

    return 0;
}