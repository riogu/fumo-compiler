// struct huh{float x; char* hum;};
// struct epic{ int a; int b; struct huh var;};
// int myfunc(){return 1231;}
#include <array>
#include <iostream>

int square(int x) { return x * x; }

int main() {
    int var;
    std::cin >> var;
    return square(var);
}
