cmake_minimum_required(VERSION 3.8)
project(mygos C ASM)
set(CMAKE_C_FLAGS "-mcmodel=medany")
set(CMAKE_EXE_LINKER_FLAGS "-T ${CMAKE_SOURCE_DIR}/link_script_riscv.ld -nostdlib  -mcmodel=medany")
set(SRC_LIST src/algs/kstring.c
        src/algs/kstring.h
        src/io/kput.S
        src/io/io.h
        src/io/plic.c
        src/io/printk.c
        src/io/trap_handler.c
        src/io/uart.c
        src/task/sched.c
        src/task/task.c
        src/task/trap_handler.c
        src/tasks/count.c
        src/tasks/tasks.c
        src/tasks/timer.c
        src/timer/timer.c
        src/timer/trap_handler.c
        src/trap/trap_handler.c
        src/trap/entry.S
        src/trap/trap.c
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
        src/io/plic.h
        src/lock/spin_lock.c
        src/lock/lock.S
        src/main.S
)

add_executable(mygos ${SRC_LIST})

target_include_directories(mygos PUBLIC
        third_party/opensbi/include
        src/algs
        src/io
        src/mem
        src/device_tree
        include/
        src/tasks
        src/task
        src/timer
        src/trap
        src/memanagement
)
