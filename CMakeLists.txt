cmake_minimum_required(VERSION 2.8)
project(sa CXX)
add_definitions("-std=c++11")

include_directories(/usr/include /usr/local/include include include/common include/sens include/sim)
link_directories(/usr/lib /usr/local/lib)

add_subdirectory(src)


