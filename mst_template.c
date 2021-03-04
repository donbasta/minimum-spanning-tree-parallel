#include <stdio.h>

#define N 100010

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
Edge edges[N * 2];
Edge tree_edges[N * 2];
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
    scanf("%d %d", &n, &m);
    init();
    for (int i=0;i<m;i++){
        int a, b;
        long w;
        scanf("%d %d %ld", &a, &b, &w);
        if (a > b){
            int c = a;
            a = b;
            b = c;
        }
        edges[i].a = a;
        edges[i].b = b;
        edges[i].w = w;
    }
    sort_edges(edges, 0, m - 1, &compare_by_weight);
    long ans = 0;
    int id = 0;
    for (int i=0;i<m;i++){
        if (find_set(edges[i].a) == find_set(edges[i].b)) continue;
        make_set(edges[i].a, edges[i].b);
        ans += edges[i].w;
        tree_edges[id++] = edges[i];
    }
    sort_edges(tree_edges, 0, id - 1, &compare_lexicographically);
    printf("%ld\n", ans);
    for (int i=0;i<id;i++){
        printf("%d-%d\n", tree_edges[i].a, tree_edges[i].b);
    }
}