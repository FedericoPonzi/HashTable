cmake_minimum_required(VERSION 3.22)
project(hashtable C)

set(CMAKE_C_STANDARD 99)
add_library(hashtable hash_table.c prime.c hash_table.h prime.h)
