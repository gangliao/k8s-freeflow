mkdir -p build && cd build
cmake -DWITH_PYTHON=OFF ..
make -j8
