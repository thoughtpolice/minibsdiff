cmake_minimum_required (VERSION 2.8.9)
project (minibsdiff)

add_executable(minibsdiff bsdiff.c bspatch.c minibsdiff.c)
