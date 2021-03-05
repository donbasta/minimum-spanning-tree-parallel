#include <stdio.h>

#define N 100010

typedef struct Edge_struct{
    long a, b;
    long w;
} Edge;

int compare_by_weight(Edge* a, Edge* b){
    return a->w < b->w;
}
int compare_lexicographically(Edge* a, Edge* b){
    if (a->a != b->a) return a->a < b->a;
    return a->b < b->b;
}

void sort_edges(Edge* ar, long l, long r, int (*comparator)(Edge*, Edge*)){
    if (l == r) return;
    long m = (l + r) / 2;
    sort_edges(ar, l, m, comparator);
    sort_edges(ar, m + 1, r, comparator);

    Edge temp[r - l + 1];
    long cur = 0;
    long lcur = l, rcur = m + 1;
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
    for (long i=l;i<=r;i++){
        ar[i] = temp[i - l];
    }
}

long n, m;
Edge edges[N * 2];
Edge tree_edges[N * 2];
long par[N], sz[N];
void init(){
    for (long i=1;i<=n;i++){
        par[i] = i;
        sz[i] = 1;
    }
}
long find_set(long a){
    if (par[a] == a) return a;
    par[a] = find_set(par[a]);
    return par[a];
}
void make_set(long a, long b){
    a = find_set(a);
    b = find_set(b);
    if (a == b) return;
    if (sz[a] < sz[b]){
        long c = a;
        a = b;
        b = c;
    }
    par[b] = a;
    sz[a] += sz[b];
}

int main(){
    scanf("%ld %ld", &n, &m);
    init();
    for (long i=0;i<m;i++){
        long a, b;
        long w;
        scanf("%ld %ld %ld", &a, &b, &w);
        if (a > b){
            long c = a;
            a = b;
            b = c;
        }
        edges[i].a = a;
        edges[i].b = b;
        edges[i].w = w;
    }
    sort_edges(edges, 0, m - 1, &compare_by_weight);
    long ans = 0;
    long id = 0;
    for (long i=0;i<m;i++){
        if (find_set(edges[i].a) == find_set(edges[i].b)) continue;
        make_set(edges[i].a, edges[i].b);
        ans += edges[i].w;
        tree_edges[id++] = edges[i];
    }
    sort_edges(tree_edges, 0, id - 1, &compare_lexicographically);
    printf("%ld\n", ans);
    for (long i=0;i<id;i++){
        printf("%ld-%ld\n", tree_edges[i].a, tree_edges[i].b);
    }
}