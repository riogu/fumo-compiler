DIR="build/"
if [ ! -d "$DIR" ]; then
    mkdir ${DIR}
fi
PROJECT_NAME="${PWD##*/}"
="${PROJECT_NAME}"
cd $DIR
# cmake -S .. -B . -DCMAKE_BUILD_TYPE=Debug
cmake -S .. -B . -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++

# BIN="${DIR}${project_name}"
# cmake --build . && cd .. && ./$BIN
# ninja && cd .. && ./$BIN
ninja

mv $PROJECT_NAME fumo
