#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>  // 変更: gettimeofday から clock() に統一

#define BUF_SIZE 80

struct node {
    int id; int weight; struct node *next;
};
struct node **list;
int *dist, *prev;

/*** Heap functions ***/
int *Heap, *Heap_Index, *Heap_Key, Heap_Size = 0;
bool *visited_pq;

void downheap(int k){
    int v = Heap[k];
    for (;;) {
        int c = 2 * k + 1;
        if (c >= Heap_Size) break;
        if (c + 1 < Heap_Size && Heap_Key[Heap[c]] > Heap_Key[Heap[c + 1]]) c++;
        if (Heap_Key[v] <= Heap_Key[Heap[c]]) break;
        Heap[k] = Heap[c]; Heap_Index[Heap[c]] = k; k = c;
    }
    Heap[k] = v; Heap_Index[v] = k;
}

void upheap(int k){
    int v = Heap[k];
    for (;;) {
        int p = (k - 1) / 2;
        if (k < 1 || Heap_Key[Heap[p]] <= Heap_Key[v]) break;
        Heap[k] = Heap[p]; Heap_Index[Heap[p]] = k; k = p;
    }
    Heap[k] = v; Heap_Index[v] = k;
}

void PQ_Init(int size, int *key) {
    int k; Heap_Size = size; Heap_Key = key;
    Heap = malloc(sizeof(int) * Heap_Size); 
    Heap_Index = malloc(sizeof(int) * Heap_Size); 
    visited_pq = malloc(sizeof(bool) * Heap_Size);
    for (k = 0; k < Heap_Size; k++) { Heap[k] = k; Heap_Index[k] = k; visited_pq[k] = false; }
    for (k = Heap_Size / 2 - 1; k >= 0; k--) downheap(k);
}

void PQ_Clear() { free(Heap); free(Heap_Index); free(visited_pq); }
int PQ_Empty() { return (Heap_Size == 0); }
int PQ_RemoveMin() {
    int min = Heap[0], last = Heap[Heap_Size - 1];
    Heap[0] = last; Heap_Index[last] = 0; Heap_Size--; downheap(0);
    visited_pq[min] = true; return min;
}
void PQ_Update(int v) { if (!visited_pq[v] && Heap_Index[v] != -1) upheap(Heap_Index[v]); }

void Insert(int a, int b, int weight, bool is_directed) {
    struct node *p;
    if ((p = malloc(sizeof(struct node))) == NULL) { printf("Cannot allocate memory \n"); exit(1); }
    p->weight = weight; p->id = b; p->next = list[a]; list[a] = p;
    if (!is_directed) {
        if ((p = malloc(sizeof(struct node))) == NULL) { printf("Cannot allocate memory \n"); exit(1); }
        p->weight = weight; p->id = a; p->next = list[b]; list[b] = p;
    }
}

void DijkstraPQ(int p, int size) {
    for (int i = 0; i < size; i++) { dist[i] = INT_MAX; prev[i] = -1; }
    dist[p] = 0; PQ_Init(size, dist);
    while (!PQ_Empty()) {
        int u = PQ_RemoveMin();
        if (dist[u] == INT_MAX) break;
        for (struct node *n = list[u]; n != NULL; n = n->next) {
            int w = n->weight; int newlen = dist[u] + w;
            if (newlen < dist[n->id]) { dist[n->id] = newlen; prev[n->id] = u; PQ_Update(n->id); }
        }
    }
    PQ_Clear();
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
    clock_t start_time, end_time; // 時間計測用
    double cpu_time_used;

    dist = malloc(sizeof(int) * n); prev = malloc(sizeof(int) * n);
    
    printf("データ読み込み完了。ノード数: %d\n", n);
    printf("優先度キュー版 ダイクストラ法の実行時間を計測します...\n\n");

    // --- Dijkstra (優先度キュー) 時間計測 ---
    start_time = clock();
    DijkstraPQ(s, n);
    end_time = clock();
    
    cpu_time_used = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;
    printf("Dijkstra (PQ) 実行時間     : %f 秒\n", cpu_time_used);

    // メモリ解放
    free(dist); free(prev);
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