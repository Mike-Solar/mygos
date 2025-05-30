cmake_minimum_required(VERSION 3.8)
project(mygos)

set(CMAKE_EXE_LINKER_FLAGS -T link_script_riscv.ld -nostdlib)
set(SRC_LIST src/algs/kstring.c
        src/algs/kstring.h
        src/io/kput.S
        src/io/io.h
        src/mem/kmemory.c
        src/mem/kmemory.h
        src/io/io.c
        src/main.c
        include/endian.h
        src/device_tree/device_tree_parser.c
        src/device_tree/device_tree_parser.h
        include/typedefs.h
        src/mem/page.c
        src/mem/page.h
        include/list.h
        src/algs/math.c
        src/algs/kmath.h
)

add_executable(mygos ${SRC_LIST})

target_include_directories(mygos PUBLIC
        third_party/opensbi/include
        src/algs
        src/io
        src/mem
        src/device_tree
        include/
)
