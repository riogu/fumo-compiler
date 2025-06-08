DIR="build/"
project_name="${PWD##*/}"
BIN="${DIR}${project_name}"

cd $DIR
cmake --build . && cd .. && ./$BIN
