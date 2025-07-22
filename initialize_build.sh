DIR="build/"
if [ ! -d "$DIR" ]; then
    mkdir ${DIR}
fi
project_name="${PWD##*/}"
EPIC="${project_name}"
cd $DIR
cmake -S .. -B . -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Debug

BIN="${DIR}${project_name}"
cmake --build . && cd .. && ./$BIN
