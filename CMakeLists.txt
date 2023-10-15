cmake_minimum_required(VERSION 3.25)
project(simbats_master)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(simbats_master
        BatchDec.cpp
        BatchEnc.cpp
        FiniteField.cpp
        simBats.cpp )

add_executable(generate generateDistributionWithWassersteinDistance.cpp fileinout.h BatchDegree.h)

add_executable(nextdegree Thenextdegree.cpp fileinout.h)

add_executable(packetsSlection fileinout.h packets_selection.cpp)
