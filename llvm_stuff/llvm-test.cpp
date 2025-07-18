// struct huh{float x; char* hum;};
// struct epic{ int a; int b; struct huh var;};
// int myfunc(){return 1231;}
int square(int x);
extern int huh;

int main() { 
    int var = 213123;
    huh = 123123;
    huh = 21;
    return square(123123);
}

int square(int x) { return x * x; }
