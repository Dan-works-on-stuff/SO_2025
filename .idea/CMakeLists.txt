cmake_minimum_required(VERSION 3.30)
project(_idea)

set(CMAKE_CXX_STANDARD 14)

add_executable(_idea
    editor.xml
    modules.xml
    SO_2025.iml
    vcs.xml
#        ../Lab8/signal_processing.c
        ../lab11/signal_proc2.c
#        ../lab10.c++
        ../Lab10/mmap1.c
        ../Lab10/mmap2.c
        ../Lab10/mmap4.c
        ../Lab10/myTr.c
        ../Lab10/myHead.c
        ../lab11/fork_ex.c
        ../lab11/wait_ex1.c
        ../lab11/tree_of_proceses.c)
