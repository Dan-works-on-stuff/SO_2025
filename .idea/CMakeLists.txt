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
        ../lab11/tree_of_proceses.c
        ../Lab12/before_exec.c
        ../Lab12/after_exec.c
        ../Lab12/exec_rec.c
        ../Lab12/com-0.c
        ../Lab12/redirect.c
        ../Lab12/exec_ls_rm.c
        ../Lab12/myCall_System.c
        ../Lab13/pipe_ex1.c
        ../Lab13/pipe_ex2.c
        ../Lab13/mkfifo_ex.c
        ../Lab13/fifo_retention_test.c
        ../Lab13/pipe_commands1.c
        ../Lab13/pipe_commands2.c
        ../Lab13/pipe_commands3.c
        ../TP2.c
        ../TP2/worker1.c
        ../TP2/worker2+3.c
        ../TP2/worker3.c)
