#include <stdio.h>
#include <stdlib.h>

#define N 100

int main(){
    int a[N], b[N];
    int b, i;
    #pragma scop
    for(i = 0; i <= N; i++)
        b += a[i];
    #pragma endscop
    return 0;
}