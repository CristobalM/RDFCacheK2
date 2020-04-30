cmake_minimum_required(VERSION 3.14)
project(RDFCacheK2)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES memory/MemoryManager.cpp)

set(HEADER_FILES memory/MemoryManager.hpp memory/MemoryPool.hpp)

set(INCLUDES memory)

set(K2TREE_PATH lib/c-k2tree-dyn)

set(INCLUDES_LIBS
        ${K2TREE_PATH}/include
        ${K2TREE_PATH}/lib/c-bitvector/include
        ${K2TREE_PATH}/lib/c-queue/include
        ${K2TREE_PATH}/lib/c-vector/include
        )

set(LINK_LIBS ${K2TREE_PATH}/bin)

include_directories(${INCLUDES} ${INCLUDES_LIBS})
link_directories(${LINK_LIBS})

add_library(RDFCacheK2 STATIC ${SOURCE_FILES})

add_library(pooled_memory_impl STATIC memory/pooled_memory_impl.cpp)

add_executable(main main.cpp)