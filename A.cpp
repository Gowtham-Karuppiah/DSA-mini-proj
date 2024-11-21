#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <limits.h>
#include <time.h>

#define MAX_WIDTH 50
#define MAX_HEIGHT 20

// Directions for moving in the maze (left, right, up, down)
int dx[] = {0, 0, -1, 1};
int dy[] = {-1, 1, 0, 0};

// Maze representation
char maze[MAX_HEIGHT][MAX_WIDTH + 1]; // Maze array
int parent[MAX_HEIGHT][MAX_WIDTH][2]; // Parent array for path reconstruction

// Priority queue node
typedef struct Node {
    int x, y;
    int g; // Cost from start
    int h; // Heuristic cost to goal
    int f; // Total cost
} Node;

// Min-heap priority queue implementation
typedef struct {
    Node nodes[MAX_HEIGHT * MAX_WIDTH];
    int size;
} PriorityQueue;

void initQueue(PriorityQueue* pq) {
    pq->size = 0;
}

bool isEmpty(PriorityQueue* pq) {
    return pq->size == 0;
}

void push(PriorityQueue* pq, Node n) {
    pq->nodes[pq->size++] = n;
    int i = pq->size - 1;
    while (i > 0 && pq->nodes[i].f < pq->nodes[(i - 1) / 2].f) {
        Node temp = pq->nodes[i];
        pq->nodes[i] = pq->nodes[(i - 1) / 2];
        pq->nodes[(i - 1) / 2] = temp;
        i = (i - 1) / 2;
    }
}

Node pop(PriorityQueue* pq) {
    Node top = pq->nodes[0];
    pq->nodes[0] = pq->nodes[--pq->size];
    int i = 0;
    while (true) {
        int smallest = i;
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        if (left < pq->size && pq->nodes[left].f < pq->nodes[smallest].f) {
            smallest = left;
        }
        if (right < pq->size && pq->nodes[right].f < pq->nodes[smallest].f) {
            smallest = right;
        }
        if (smallest == i) break;
        Node temp = pq->nodes[i];
        pq->nodes[i] = pq->nodes[smallest];
        pq->nodes[smallest] = temp;
        i = smallest;
    }
    return top;
}

// Heuristic function: Manhattan distance
int heuristic(int x1, int y1, int x2, int y2) {
    return abs(x1 - x2) + abs(y1 - y2);
}

// A* algorithm for pathfinding
bool aStar(int startX, int startY, int endX, int endY, int height, int width) {
    PriorityQueue pq;
    initQueue(&pq);

    int g[MAX_HEIGHT][MAX_WIDTH];
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            g[i][j] = INT_MAX;
            parent[i][j][0] = -1;
            parent[i][j][1] = -1;
        }
    }

    g[startX][startY] = 0;
    push(&pq, (Node){startX, startY, 0, heuristic(startX, startY, endX, endY), 0});

    while (!isEmpty(&pq)) {
        Node current = pop(&pq);

        if (current.x == endX && current.y == endY) {
            return true;
        }

        for (int i = 0; i < 4; i++) {
            int nx = current.x + dx[i];
            int ny = current.y + dy[i];

            if (nx >= 0 && nx < height && ny >= 0 && ny < width && maze[nx][ny] == ' ') {
                int newG = g[current.x][current.y] + 1;

                if (newG < g[nx][ny]) {
                    g[nx][ny] = newG;
                    parent[nx][ny][0] = current.x;
                    parent[nx][ny][1] = current.y;
                    int h = heuristic(nx, ny, endX, endY);
                    push(&pq, (Node){nx, ny, newG, h, newG + h});
                }
            }
        }
    }

    return false; // No path found
}

// Function to generate a maze with borders
void generateMazeWithBorders(int height, int width) {
    // Create borders of walls ('*')
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (i == 0 || i == height - 1 || j == 0 || j == width - 1) {
                maze[i][j] = '.';
            } else {
                maze[i][j] = (rand() % 3 == 0) ? '.' : ' ';
            }
        }
        maze[i][width] = '\0';
    }
}

// Function to print the maze with the solution path
void printMazeWithPath(int height, int width, int startX, int startY, int endX, int endY) {
    char displayMaze[MAX_HEIGHT][MAX_WIDTH + 1];
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            displayMaze[i][j] = maze[i][j];
        }
        displayMaze[i][width] = '\0';
    }

    int x = endX, y = endY;
    while (!(x == startX && y == startY)) {
        displayMaze[x][y] = '~'; // Mark path with '+'
        int tempX = parent[x][y][0];
        int tempY = parent[x][y][1];
        x = tempX;
        y = tempY;
    }

    displayMaze[startX][startY] = 'S'; // Mark the start with 'S'
    displayMaze[endX][endY] = 'E';    // Mark the end with 'E'

    for (int i = 0; i < height; i++) {
        printf("%s\n", displayMaze[i]);
    }
}

// Function to get a random valid position in the maze
void getRandomPosition(int* x, int* y, int height, int width) {
    do {
        *x = rand() % (height - 2) + 1; // Ensure it's not on the border
        *y = rand() % (width - 2) + 1;  // Ensure it's not on the border
    } while (maze[*x][*y] != ' '); // Ensure it's an open space
}

int main() {
    srand(time(NULL));

    int height, width;
    printf("Enter maze height (max %d): ", MAX_HEIGHT);
    scanf("%d", &height);
    printf("Enter maze width (max %d): ", MAX_WIDTH);
    scanf("%d", &width);

    // Ensure valid dimensions
    if (height < 5 || width < 5) {
        printf("Height and width must be at least 5.\n");
        return 1;
    }

    generateMazeWithBorders(height, width);

    int startX, startY, endX, endY;
    getRandomPosition(&startX, &startY, height, width);
    getRandomPosition(&endX, &endY, height, width);

    printf("Generated Maze:\n");
    for (int i = 0; i < height; i++) {
        printf("%s\n", maze[i]);
    }

    printf("Start: (%d, %d), End: (%d, %d)\n", startX, startY, endX, endY);

    if (aStar(startX, startY, endX, endY, height, width)) {
        printf("\nMaze with Shortest Path:\n");
        printMazeWithPath(height, width, startX, startY, endX, endY);
    } else {
        printf("\nNo Path Found.\n");
    }

    return 0;
}

