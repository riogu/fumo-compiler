#include <stdio.h>

void func(int a, int b) {
    printf("%d and %d\n", a, b);
}

int main() {
    int x = 10;
    int* z;
    int y;
    z[0] = 1 + (x = 2, 2);
}
