DIR="build/"
project_name="${PWD##*/}"
BIN="${DIR}${project_name}"

cd $DIR
# cmake --build . && cd .. && ./$BIN
# cmake --build . 
cmake . -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=On 
cmake . -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Debug
