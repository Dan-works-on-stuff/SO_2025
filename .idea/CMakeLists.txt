cmake_minimum_required(VERSION 3.30)
project(_idea)

set(CMAKE_CXX_STANDARD 14)

add_executable(_idea
    editor.xml
    modules.xml
    SO_2025.iml
    vcs.xml
        ../examen/script.c
        ../examen/script_prototype.c
        ../lab11/cut.c
        ../Lab8/signal_processing.c
        ../lab11/signal_proc2.c
        ../lab10.c++
        ../Lab10/mmap1.c
        ../Lab10/mmap2.c
        ../Lab10/mmap4.c
        ../Lab10/myTr.c
        ../Lab10/myHead.c)
