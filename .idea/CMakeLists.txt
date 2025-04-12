cmake_minimum_required(VERSION 3.30)
project(_idea)

set(CMAKE_CXX_STANDARD 14)

add_executable(_idea
    editor.xml
    modules.xml
    SO_2025.iml
    vcs.xml)
