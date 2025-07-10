#include <stdio.h>

void func(int a, int b) {
    printf("%d and %d\n", a, b);
}

int* f() {
    int* x;
    return x;
}

int main() {
    // *f() = 123123;

    // int y;
    int x, y = 1;
    // int x = (y = 123) + 1;

    int a = 3;
    int b = 123;
    func(a, b);
}
