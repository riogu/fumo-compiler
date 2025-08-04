DIR="build/"
project_name="${PWD##*/}"
BIN="${DIR}${project_name}"

cd $DIR
# cmake --build . 
# cmake --build . && cd .. && ./$BIN
# ninja && cd .. && ./$BIN
ninja
