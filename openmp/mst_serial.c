#include <stdio.h>
#include <time.h>
#ifdef _OPENMP
    #include <omp.h>
#endif

#define N 2010

typedef struct Edge_struct{
    int a, b;
    long w;
} Edge;

int compare_by_weight(Edge* a, Edge* b){
    return a->w < b->w;
}
int compare_lexicographically(Edge* a, Edge* b){
    if (a->a != b->a) return a->a < b->a;
    return a->b < b->b;
}

void sort_edges(Edge* ar, int l, int r, int (*comparator)(Edge*, Edge*)){
    if (l == r) return;
    int m = (l + r) / 2;

    // #pragma omp parallel num_threads(2)
    sort_edges(ar, l, m, comparator);
    sort_edges(ar, m + 1, r, comparator);

    Edge temp[r - l + 1];
    int cur = 0;
    int lcur = l, rcur = m + 1;
    while (lcur <= m && rcur <= r){
        if (comparator(&ar[lcur], &ar[rcur])){//ar[lcur].w < ar[rcur].w){
            temp[cur++] = ar[lcur++];
        } else{
            temp[cur++] = ar[rcur++];
        }
    }
    while (lcur <= m){
        temp[cur++] = ar[lcur++];
    }
    while (rcur <= r){
        temp[cur++] = ar[rcur++];
    }
    for (int i=l;i<=r;i++){
        ar[i] = temp[i - l];
    }
}

int n, m;
Edge edges[N * N];
Edge tree_edges[N * N];
int par[N], sz[N];
void init(){
    for (int i=1;i<=n;i++){
        par[i] = i;
        sz[i] = 1;
    }
}
int find_set(int a){
    if (par[a] == a) return a;
    par[a] = find_set(par[a]);
    return par[a];
}
void make_set(int a, int b){
    a = find_set(a);
    b = find_set(b);
    if (a == b) return;
    if (sz[a] < sz[b]){
        int c = a;
        a = b;
        b = c;
    }
    par[b] = a;
    sz[a] += sz[b];
}

int main(){

    FILE *input, *output; 
    input = freopen("input1.txt", "r", stdin); 
    

    fscanf(input, "%d", &n);
    init();
    int itr = 0;
    for (int i=1;i<=n;i++){
        for (int j=1;j<=n;j++){
            int w;
            fscanf(input, "%d", &w);
            if (w != -1 && i < j) {
                edges[itr].a = i;
                edges[itr].b = j;
                edges[itr].w = w;
                itr++;
            }
        }
    }
    printf("%d %d %d\n", edges[0].a, edges[0].b, edges[0].w);
    printf("%d\n", itr);
    // printf("debug1");

    clock_t start = clock();

    // printf("debug2");

    sort_edges(edges, 0, itr - 1, &compare_by_weight);

    // printf("debug3");
    long ans = 0;
    int id = 0;
    for (int i=0;i<itr;i++){
        if (find_set(edges[i].a) == find_set(edges[i].b)) continue;
        make_set(edges[i].a, edges[i].b);
        ans += edges[i].w;
        tree_edges[id++] = edges[i];
    }

    // printf("debug4");
    sort_edges(tree_edges, 0, id - 1, &compare_lexicographically);

    // printf("debug5");

    clock_t interval = clock() - start;
    int exec_time = interval * 1000 / CLOCKS_PER_SEC;

    //  printf("Waktu eksekusi: %d ms\n", exec_time);

    output = freopen("output.out", "w", stdout); 
    printf("Waktu eksekusi: %d ms\n", exec_time);
    printf("%ld\n", ans);
    for (int i=0;i<id;i++){
        printf("%d-%d\n", tree_edges[i].a, tree_edges[i].b);
    }

    fclose(input);
    fclose(output);
}