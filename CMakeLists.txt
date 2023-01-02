cmake_minimum_required(VERSION 3.0)
project(nuclear)

include_directories("Include")
file(GLOB_RECURSE SOURCES "Source/*.cpp")

add_executable(nuclear ${SOURCES})
set_property(TARGET nuclear PROPERTY CXX_STANDARD 23)