#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#define BUF_SIZE 80

int **matrix;
int *dist, *prev, *visited;

//隣接行列版ダイクストラ法
void DijkstraMatrix(int p, int size) {
    int i;
    for (i = 0; i < size; i++) {
        dist[i] = INT_MAX; prev[i] = -1; visited[i] = 0;
    }
    dist[p] = 0;
    while (1) {
        int u = -1;
        int min = INT_MAX;
        for (i = 0; i < size; i++) {
            if (visited[i] == 0 && dist[i] < min) {
                min = dist[i]; u = i;
            }
        }
        if (u == -1) break;
        visited[u] = 1;
        for (i = 0; i < size; i++) {
            if (matrix[u][i] > 0) {
                int w = matrix[u][i];         
                int newlen = dist[u] + w;     
                if (newlen < dist[i]) {      
                    dist[i] = newlen;         
                    prev[i] = u;             
                } 
            }
        }
    }
}

int main(int argc, char *argv[]) {
    FILE *fp; int i, j, n; char buf[BUF_SIZE]; bool is_directed = false;
    if (argc != 2) { printf("Usage: %s <filename>\n", argv[0]); exit(1); }
    if ((fp = fopen(argv[1], "r")) == NULL) { printf("Cannot open file \n"); exit(1); }
    fgets(buf, BUF_SIZE, fp); sscanf(buf, "%d", &n);
    if (n > 10000) {
        printf("ノード数が大きすぎる\n");
        fclose(fp); return 1;
    }

    matrix = malloc(sizeof(int*) * n);
    for (i = 0; i < n; i++) matrix[i] = malloc(sizeof(int) * n);
    for (i = 0; i < n; i++) for (j = 0; j < n; j++) matrix[i][j] = 0;
    if (strstr(argv[1], "twitter") != NULL) is_directed = true;

    while (fgets(buf, BUF_SIZE, fp) != NULL) {
        int a, b, c; 
        if (sscanf(buf, "%d %d %d", &a, &b, &c) == 3) {
            matrix[a][b] = c;
            if (!is_directed) matrix[b][a] = c;
        }
    }
    fclose(fp);

    int s = 0;
    clock_t start_time, end_time;
    double cpu_time_used;

    dist = malloc(sizeof(int) * n); 
    prev = malloc(sizeof(int) * n); 
    visited = malloc(sizeof(int) * n);

    //Dijkstra (隣接行列) 時間計測
    start_time = clock();
    DijkstraMatrix(s, n);
    end_time = clock();
    
    cpu_time_used = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;
    printf("Dijkstra (Matrix) 実行時間 : %f 秒\n", cpu_time_used);

    //メモリ解放
    free(dist); free(prev); free(visited);
    for (i = 0; i < n; i++) free(matrix[i]);
    free(matrix);
    
    return 0;
}
