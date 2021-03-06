#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_PROCESS 128
#define MAXN 9000000

/* SAMPLE TEST CASE:
5
-1  2  6 -1 -1
 2 -1  4  1 -1
 6  4 -1 -1  1
-1  1 -1 -1  1
-1 -1  1  1 -1
*/

// ssh -i ./43 kel43@34.239.177.66

typedef struct Edge_struct{
    long a, b;
    long w;
} Edge;

long n, m;
Edge *edges;
Edge *tree_edges;
long *par, *sz;
int world_size;
int world_rank;
int parent_rank[MAX_PROCESS];
long long ans = 0;
long id = 0;
// long *data;
// Edge *temp;

int compare_by_weight(Edge* a, Edge* b){
    if (a->w != b->w) return a->w < b->w;
    if (a->a != b->a) return a->a < b->a;
    return a->b < b->b;
}
int compare_lexicographically(Edge* a, Edge* b){
    if (a->a != b->a) return a->a < b->a;
    return a->b < b->b;
}
void sort_edges(Edge* ar, long sz, int (*comparator)(Edge*, Edge*), int mx_rank, int send_to_parent){
    // threshold
    if (sz > 1){
        long m = sz / 2;
        int ch_rank = world_rank + (mx_rank - world_rank + 1) / 2;

        sort_edges(ar, m, comparator, ch_rank - 1, 0);
        if (ch_rank > world_rank){
            MPI_Send(&mx_rank, 1, MPI_INT, ch_rank, 0, MPI_COMM_WORLD);
            long *data = (long*) malloc((sz - m) * 3 * sizeof(long));
            for (long i=m;i<sz;i++){
                data[(i - m) * 3 + 0] = ar[i].a;
                data[(i - m) * 3 + 1] = ar[i].b;
                data[(i - m) * 3 + 2] = ar[i].w;
            }
            MPI_Send(data, (sz - m) * 3, MPI_LONG, ch_rank, 1, MPI_COMM_WORLD);
            MPI_Recv(data, (sz - m) * 3, MPI_LONG, ch_rank, 99, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for (long i=m;i<sz;i++){
                ar[i].a = data[(i - m) * 3 + 0];
                ar[i].b = data[(i - m) * 3 + 1];
                ar[i].w = data[(i - m) * 3 + 2];
            }
            free(data);
        } else{
            sort_edges(ar + m, sz - m, comparator, mx_rank, 0);
        }
        Edge *temp = (Edge*) malloc(sz * sizeof(Edge));
        long cur = 0;
        long lcur = 0, rcur = m;
        while (lcur < m && rcur < sz){
            if (comparator(&ar[lcur], &ar[rcur])){
                temp[cur++] = ar[lcur++];
            } else{
                temp[cur++] = ar[rcur++];
            }
        }
        while (lcur < m){
            temp[cur++] = ar[lcur++];
        }
        while (rcur < sz){
            temp[cur++] = ar[rcur++];
        }
        for (long i=0;i<sz;i++){
            ar[i] = temp[i];
        }
        free(temp);
    }
    if (send_to_parent){
        long *data = (long*) malloc(sz * 3 * sizeof(long));
        for (long i=0;i<sz;i++){
            data[i * 3 + 0] = ar[i].a;
            data[i * 3 + 1] = ar[i].b;
            data[i * 3 + 2] = ar[i].w;
        }
        MPI_Send(data, sz * 3, MPI_LONG, parent_rank[world_rank], 99, MPI_COMM_WORLD);
        free(data);
    }
}
void init(){
    par = (long*) malloc(n * sizeof(long));
    sz = (long*) malloc(n * sizeof(long));
    edges = (Edge*) malloc(n * n * sizeof(Edge));
    tree_edges = (Edge*) malloc(n * sizeof(Edge));

    for (long i=0;i<n;i++){
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
void init_parent_rank(int cur, int p, int mx){
    if (p != cur) parent_rank[cur] = p;
    if (cur == mx) return;
    int ch_rank = cur + (mx - cur + 1) / 2;
    init_parent_rank(cur, cur, ch_rank - 1);
    init_parent_rank(ch_rank, cur, mx);
}

int main(int argc, char** argv) {
    clock_t begin_time = clock();
    // Initialize the MPI environment
    MPI_Init(NULL, NULL);

    // Get the number of processes
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    init_parent_rank(0, -1, world_size - 1);
    if (world_rank == 0){
        scanf("%ld", &n);
        m = 0;
        init();
        for (long i=0;i<n;i++){
            for (long j=0;j<n;j++){
                long w;
                scanf("%ld", &w);
                if (j > i && w != -1){
                    edges[m].a = i;
                    edges[m].b = j;
                    edges[m++].w = w;
                }
            }
        }
        sort_edges(edges, m, &compare_by_weight, world_size - 1, 0);
        for (long i=0;i<m;i++){
            if (find_set(edges[i].a) == find_set(edges[i].b)) continue;
            make_set(edges[i].a, edges[i].b);
            ans += edges[i].w;
            tree_edges[id++] = edges[i];
        }
        sort_edges(tree_edges, id, &compare_lexicographically, world_size - 1, 0);
        free(par);
        free(sz);
        free(edges);
        free(tree_edges);
    } else{
        // sort by weight
        int mx_rank = 0;
        MPI_Recv(&mx_rank, 1, MPI_INT, parent_rank[world_rank], 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        if (mx_rank >= world_rank){
            MPI_Status status;
            MPI_Probe(parent_rank[world_rank], 1, MPI_COMM_WORLD, &status);
            int sz;
            MPI_Get_count(&status, MPI_LONG, &sz);
            long *data = (long*) malloc(sz * sizeof(long));
            Edge *temp = (Edge*) malloc((sz / 3) * sizeof(Edge));
            MPI_Recv(data, sz, MPI_LONG, parent_rank[world_rank], 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            fflush(stdout);
            for (int i=0;i<sz / 3;i++){
                fflush(stdout);
                temp[i].a = data[i * 3 + 0];
                temp[i].b = data[i * 3 + 1];
                temp[i].w = data[i * 3 + 2];
            }
            sort_edges(temp, sz / 3, &compare_by_weight, mx_rank, 1);
            free(data);
            free(temp);
        }
        // sort lexicographically
        mx_rank = 0;
        MPI_Recv(&mx_rank, 1, MPI_INT, parent_rank[world_rank], 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        if (mx_rank >= world_rank){
            MPI_Status status;
            MPI_Probe(parent_rank[world_rank], 1, MPI_COMM_WORLD, &status);
            int sz;
            MPI_Get_count(&status, MPI_LONG, &sz);
            long *data = (long*) malloc(sz * sizeof(long));
            Edge *temp = (Edge*) malloc((sz / 3) * sizeof(Edge));
            MPI_Recv(data, sz, MPI_LONG, parent_rank[world_rank], 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for (int i=0;i<sz / 3;i++){
                temp[i].a = data[i * 3 + 0];
                temp[i].b = data[i * 3 + 1];
                temp[i].w = data[i * 3 + 2];
            }
            sort_edges(temp, sz / 3, &compare_lexicographically, mx_rank, 1);
            free(data);
            free(temp);
        }
    }
    for (int i=0;i<world_size;i++){
        int freeing = -1;
        if (parent_rank[i] == world_rank){
            MPI_Send(&freeing, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&freeing, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    }
    MPI_Finalize();
    if (world_rank == 0){
        clock_t end_time = clock();
        double time_spent = (end_time - begin_time) / (0.001 * CLOCKS_PER_SEC);

        printf("%lld\n", ans);
        for (long i=0;i<id;i++){
            printf("%ld-%ld\n", tree_edges[i].a, tree_edges[i].b);
        }
        printf("Waktu Eksekusi: %.2f ms\n", time_spent);
    }
}
