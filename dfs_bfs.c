#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>  // 時間計測用

#define BUF_SIZE 80

struct node {
    int id;
    int weight;
    struct node *next;
};

struct node **list;

void Insert(int a, int b, int weight, bool is_directed) {
    struct node *p;
    if ((p = malloc(sizeof(struct node))) == NULL) { printf("Cannot allocate memory \n"); exit(1); }
    p->weight = weight; p->id = b; p->next = list[a]; list[a] = p;
    if (!is_directed) {
        if ((p = malloc(sizeof(struct node))) == NULL) { printf("Cannot allocate memory \n"); exit(1); }
        p->weight = weight; p->id = a; p->next = list[b]; list[b] = p;
    }
}

// --- BFS (幅優先探索) ---
void BFS_Search(int start, int size) {
    bool *bfs_visited = calloc(size, sizeof(bool));
    int *bfs_prev = malloc(sizeof(int) * size);
    for (int i = 0; i < size; i++) bfs_prev[i] = -1;
    int *queue = malloc(sizeof(int) * size), head = 0, tail = 0;
    
    bfs_visited[start] = true; queue[tail++] = start;
    while (head < tail) {
        int u = queue[head++];
        for (struct node *n = list[u]; n != NULL; n = n->next) {
            if (!bfs_visited[n->id]) { 
                bfs_visited[n->id] = true; 
                bfs_prev[n->id] = u; 
                queue[tail++] = n->id; 
            }
        }
    }
    free(bfs_visited); free(bfs_prev); free(queue);
}

// --- DFS (深さ優先探索) ---
bool *dfs_visited; int *dfs_prev;
void DFS_Rec(int u) {
    dfs_visited[u] = true;
    for (struct node *n = list[u]; n != NULL; n = n->next) {
        if (!dfs_visited[n->id]) { 
            dfs_prev[n->id] = u; 
            DFS_Rec(n->id); 
        }
    }
}

void DFS_Search(int start, int size) {
    dfs_visited = calloc(size, sizeof(bool));
    dfs_prev = malloc(sizeof(int) * size);
    for (int i = 0; i < size; i++) dfs_prev[i] = -1;
    
    DFS_Rec(start);
    free(dfs_visited); free(dfs_prev);
}

int main(int argc, char *argv[]) {
    FILE *fp; int i, n; char buf[BUF_SIZE]; bool is_directed = false;
    if (argc != 2) { printf("Usage: %s <filename>\n", argv[0]); exit(1); }
    if ((fp = fopen(argv[1], "r")) == NULL) { printf("Cannot open file \n"); exit(1); }
    fgets(buf, BUF_SIZE, fp); sscanf(buf, "%d", &n);
    list = malloc(sizeof(struct node*) * n);
    for (i = 0; i < n; i++) list[i] = NULL;
    if (strstr(argv[1], "twitter") != NULL) is_directed = true;

    while (fgets(buf, BUF_SIZE, fp) != NULL) {
        int a, b, c; 
        if (sscanf(buf, "%d %d %d", &a, &b, &c) == 3) {
            Insert(a, b, c, is_directed);
        }
    }
    fclose(fp);

    int s = 0; // 始点を0に固定
    clock_t start_time, end_time; // 時間計測用の変数
    double cpu_time_used;

    printf("データ読み込み完了。ノード数: %d\n", n);
    printf("各アルゴリズムの実行時間を計測します...\n\n");

    // --- DFS 時間計測 ---
    start_time = clock();
    DFS_Search(s, n);
    end_time = clock();
    cpu_time_used = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;
    printf("DFS 実行時間        : %f 秒\n", cpu_time_used);

    // --- BFS 時間計測 ---
    start_time = clock();
    BFS_Search(s, n);
    end_time = clock();
    cpu_time_used = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;
    printf("BFS 実行時間        : %f 秒\n", cpu_time_used);

    // メモリ解放
    for (i = 0; i < n; i++) {
        struct node *curr = list[i];
        while (curr != NULL) {
            struct node *temp = curr;
            curr = curr->next;
            free(temp);
        }
    }
    free(list);

    return 0;
}