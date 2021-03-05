#include <stdio.h>
#include <stdlib.h>

#define N 1000

int ar[N+5][N+5];

void init() {
    for (int i = 1; i <= N; i++) {
        for (int j = 1; j <= N; j++) {
            ar[i][j] = -1;
        }
    }
}

void generate_tree() {
    //1-2-....-N/2
    // N/4 - N/2+1 - ... - 3*N/4
    // N/3 - 3*N/4+1 - ... N
    for (int i = 1; i < N/2; i++) {
        ar[i][i+1] = ar[i+1][i] = 2;
    }

    ar[N/4][N/2 + 1] = ar[N/2 + 1][N/4] = 3;
    for (int i = N/2+1; i < 3*N/4; i++) {
        ar[i][i+1] = ar[i+1][i] = 4;
    }
    ar[N/3][3*N/4 + 1] = ar[3*N/4 + 1][N] = 5;
    for (int i = 3*N/4+1; i < N; i++) {
        ar[i][i+1] = ar[i+1][i] = 6;
    }
}

void generate_random_edges() {
    int cnt = 0;
    for (int i = 1; i <= N; i++) {
        for (int j = 1; j <= N; j++) {
            if (cnt >= 100*N) break;
            if (ar[i][j] != -1) continue;
            int random = rand() % 10;
            if (random >= 3) {
                ar[i][j] = 7; 
                ar[j][i] = 7;
                cnt++;
            }
        }
    }
}

int main () {
    freopen("input1.txt", "w", stdout);
    printf("%d\n", N);
    init();
    generate_tree();
    generate_random_edges();
    for (int i = 1; i <= N; i++) {
        for (int j = 1; j <= N; j++) {
            printf("%d", ar[i][j]);
            if (j == N) {
                printf("\n");
            } else {
                printf(" ");
            }
        }
    }
}