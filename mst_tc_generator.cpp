#include <bits/stdc++.h>

using namespace std;

const int N = 1e4 + 10;

int randomInt(int l, int r){
    return rand() % (r - l + 1) + l;
}

int n;
int mat[N][N];

int main(){
    srand(time(0));

    n = randomInt(500, 1000);
    for (int i=0;i<n;i++){
        mat[i][i] = -1;
        for (int j=i + 1;j<n;j++){
            if (randomInt(1, 2) == 1){
                mat[i][j] = mat[j][i] = randomInt(1, 1000);
            } else{
                mat[i][j] = mat[j][i] = -1;
            }
        }
    }
    cout << n << '\n';
    for (int i=0;i<n;i++){
        for (int j=0;j<n;j++){
            cout << mat[i][j] << ' ';
        }
        cout << '\n';
    }

    return 0;
}