#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifdef _OPENMP
    #include <omp.h>
#endif

int threads, processors, max_threads;
int threshold = 20;

typedef struct Edge_struct{
    int a, b;
    long w;
} Edge;

int compare_by_weight(Edge* a, Edge* b){
    if (a->w != b->w) return a->w < b->w;
    if (a->a != b->a) return a->a < b->a;
    return a->b < b->b;
}
int compare_lexicographically(Edge* a, Edge* b){
    if (a->a != b->a) return a->a < b->a;
    return a->b < b->b;
}

void merge(Edge* ar, int l, int r, int (*comparator)(Edge*, Edge*), Edge* temp) {
    // Edge *temp = malloc((r - l + 1) * sizeof(Edge));
    // Edge temp[r - l + 1];
    int cur = l;
    int m = (l + r) / 2;
    int lcur = l, rcur = m + 1;
    while (lcur <= m && rcur <= r){
        if (comparator(&ar[lcur], &ar[rcur])){
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
        ar[i] = temp[i];
    }
    // free (temp);
}

void insertion_sort(Edge* ar, int l, int r, int (*comparator)(Edge*, Edge*)) {
    for (int i = l; i <= r; i++) {
        int j;
        Edge v = ar[i];
        for (j = i - 1; j >= l; j--) {
            if (comparator(&ar[j], &v)) break;
            ar[j + 1] = ar[j];
        }
        ar[j + 1] = v;
    }
}

void merge_sort_serial(Edge* ar, int l, int r, int (*comparator)(Edge*, Edge*), Edge* temp) {
    if (l >= r) return;
    if (r - l + 1 <= threshold) {
        insertion_sort(ar, l, r, comparator);
        return;
    }
    int m = (l + r) / 2;
    merge_sort_serial(ar, l, m, comparator, temp);
    merge_sort_serial(ar, m + 1, r, comparator, temp);
    merge(ar, l, r, comparator, temp);
}

void merge_sort_parallel(Edge* ar, int l, int r, int (*comparator)(Edge*, Edge*), int threads, Edge* temp){
    if (l >= r) return;
    if (threads == 1) {
        merge_sort_serial(ar, l, r, comparator, temp);    
        return;
    }
    int m = (l + r) / 2;
    #pragma omp parallel sections
    {   
        #pragma omp section
            merge_sort_parallel(ar, l, m, comparator, threads / 2, temp);
        #pragma omp section
            merge_sort_parallel(ar, m + 1, r, comparator, threads - threads / 2, temp);
    }
    merge(ar, l, r, comparator, temp);
}

int n, m;
Edge *edges, *tmp;
Edge *tree_edges;
int *par, *sz;


void init(){
	par = (int*) malloc(n * sizeof(int));
    sz = (int*) malloc(n * sizeof(int));
    edges = (Edge*) malloc(n * n * sizeof(Edge));
    tree_edges = (Edge*) malloc(n * sizeof(Edge));
    tmp = (Edge*) malloc(n * n * sizeof(Edge));

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

int main(int argc, char** argv){

    // FILE *input, *output; 
    
    // get input filename
    // char* filename = argv[1];
    
    // input = freopen(filename, "r", stdin); 

    scanf("%d", &n);
    init();
    m = 0;
    for (int i=0;i<n;i++){
        for (int j=0;j<n;j++){
            long w;
            scanf("%ld", &w);
            if (w != -1 && i < j) {
                edges[m].a = i;
                edges[m].b = j;
                edges[m++].w = w;
            }
        }
    }

    threads = 8;
    //processors = omp_get_num_procs();
    //max_threads = omp_get_max_threads();

    //printf("Using %d threads\n", threads);
    //printf("Num of available processors: %d\n", processors);
    //printf("Max num of threads: %d\n", max_threads);

    clock_t start = clock();

    omp_set_nested(1);
    merge_sort_parallel(edges, 0, m - 1, &compare_by_weight, threads, tmp);
    // merge_sort_serial(edges, 0, m - 1, &compare_by_weight);

    long ans = 0;
    int id = 0;
    for (int i=0;i<m;i++){
        if (find_set(edges[i].a) == find_set(edges[i].b)) continue;
        make_set(edges[i].a, edges[i].b);
        ans += edges[i].w;
        tree_edges[id++] = edges[i];
    }

    merge_sort_parallel(tree_edges, 0, id - 1, &compare_lexicographically, threads, tmp);
    // merge_sort_serial(tree_edges, 0, id - 1, &compare_lexicographically);

    clock_t end = clock();

    double exec_time = (end - start) / (0.001 * CLOCKS_PER_SEC);

    // output = freopen("output.out", "w", stdout); 
    printf("%ld\n", ans);
    for (int i=0;i<id;i++){
        printf("%d-%d\n", tree_edges[i].a, tree_edges[i].b);
    }
    printf("Waktu eksekusi: %.3f ms", exec_time);

    // fclose(input);
    // fclose(output);

    // free(par);
    // free(sz);
    // free(edges);
    // free(tree_edges);
}
