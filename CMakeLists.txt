    cmake_minimum_required(VERSION 3.10)
    project(MyProject)

    add_executable(my_app main.cpp)
    target_link_libraries(my_app sqlite3)
