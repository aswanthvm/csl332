#include <stdio.h>

// Maximum number of routers
#define MAX 10

// Infinity value
#define INF 999

int main()
{
    // Variable declarations
    int n, cost[MAX][MAX], dist[MAX], visited[MAX];
    int i, j, min, next, src;

    // Input number of routers
    printf("Enter number of routers: ");
    scanf("%d", &n);

    // Input cost adjacency matrix
    printf("Enter the cost matrix:\n");

    for(i = 0; i < n; i++)
    {
        for(j = 0; j < n; j++)
        {
            scanf("%d", &cost[i][j]);

            // Replace 0 with INF except diagonal elements
            if(cost[i][j] == 0 && i != j)
                cost[i][j] = INF;
        }
    }

    // Input source router
    printf("Enter source router: ");
    scanf("%d", &src);

    // Initialize distance and visited arrays
    for(i = 0; i < n; i++)
    {
        // Distance from source to each router
        dist[i] = cost[src][i];

        // Initially no router is visited
        visited[i] = 0;
    }

    // Distance from source to itself is 0
    dist[src] = 0;

    // Mark source as visited
    visited[src] = 1;

    // Dijkstra Algorithm
    for(i = 0; i < n - 1; i++)
    {
        // Assume minimum distance is INF
        min = INF;

        // Find nearest unvisited router
        for(j = 0; j < n; j++)
        {
            if(!visited[j] && dist[j] < min)
            {
                min = dist[j];
                next = j;
            }
        }

        // Mark selected router as visited
        visited[next] = 1;

        // Update distances of adjacent routers
        for(j = 0; j < n; j++)
        {
            if(!visited[j] &&
               (min + cost[next][j] < dist[j]))
            {
                dist[j] = min + cost[next][j];
            }
        }
    }

    // Display shortest distances
    printf("\nShortest distance from router %d:\n", src);

    for(i = 0; i < n; i++)
    {
        printf("Router %d -> Router %d = %d\n",
               src, i, dist[i]);
    }

    return 0;
}