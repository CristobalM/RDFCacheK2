cmake_minimum_required(VERSION 3.14)
project(RDFCacheK2)
set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_STANDARD 17)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(CORE_SOURCE_FILES
        core/K2Tree.cpp
        core/Cache.cpp
        core/GraphResult.cpp
        core/cache_result/ExistingCacheResult.cpp
        core/cache_result/NonExistingCacheResult.cpp
        core/FeedData.cpp
        )

set(CORE_HEADER_FILES
        core/K2Tree.hpp
        core/Cache.hpp
        core/GraphResult.hpp
        core/RDFTriple.hpp
        core/cache_result/ExistingCacheResult.hpp
        core/cache_result/CacheResult.hpp
        core/cache_result/NonExistingCacheResult.hpp
        core/FeedData.hpp
        )

set(NETWORK_SOURCE_FILES
        network/CacheServer.cpp
        network/TCPServerConnection.cpp
        network/ServerTask.cpp
        network/CacheServerTaskProcessor.cpp
        network/ServerWorker.cpp
        network/Message.cpp
        network/responses/CacheCheckResponseMessage.cpp
        network/responses/CacheFeedResponseMessage.cpp
        network/responses/CacheRetrieveResponseMessage.cpp
        )

set(NETWORK_HEADER_FILES
        network/CacheServer.hpp
        network/TCPServerConnection.hpp
        network/ServerTask.hpp
        network/CacheServerTaskProcessor.hpp
        network/ServerWorker.hpp
        network/Message.hpp
        network/responses/ResponseMessage.hpp
        network/responses/CacheCheckResponseMessage.hpp
        network/responses/CacheFeedResponseMessage.hpp
        network/responses/CacheRetrieveResponseMessage.hpp
        )

set(INCLUDES memory core network)

set(K2TREE_PATH ${CMAKE_SOURCE_DIR}/lib/c-k2tree-dyn)

set(INCLUDES_LIBS
        ${K2TREE_PATH}/include
        ${K2TREE_PATH}/lib/c-bitvector/include
        ${K2TREE_PATH}/lib/c-queue/include
        ${K2TREE_PATH}/lib/c-vector/include
        )

set(LINK_LIBS ${K2TREE_PATH}/bin)

include_directories(${INCLUDES} ${INCLUDES_LIBS})
link_directories(${LINK_LIBS})


add_library(MemoryManager STATIC
        memory/pooled_memory_impl.cpp
        memory/MemoryManager.cpp
        )

add_library(RDFCacheK2 STATIC ${CORE_SOURCE_FILES})
add_library(RDFCacheK2_Network STATIC ${NETWORK_SOURCE_FILES})

add_executable(main main.cpp )
add_executable(cache_example cache_example.cpp)

add_executable(example1 example/example1.cpp)

target_link_libraries(main RDFCacheK2 k2tree_merged_noalloc MemoryManager)
target_link_libraries(cache_example RDFCacheK2 k2tree_merged_noalloc MemoryManager)
target_link_libraries(example1 RDFCacheK2 RDFCacheK2_Network k2tree_merged_noalloc MemoryManager pthread)
