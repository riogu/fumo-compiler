DIR="build/"
PROJECT_NAME="${PWD##*/}"
BIN="${DIR}${PROJECT_NAME}"

cd $DIR
# cmake --build . 
# cmake --build . && cd .. && ./$BIN
# ninja && cd .. && ./$BIN
ninja

mv $PROJECT_NAME fumo
