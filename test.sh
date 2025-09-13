clang++ -std=c++23 -Wno-ignored-attributes src/compiler_test.cpp -o test-fumo && ./test-fumo "$@" && rm test-fumo
