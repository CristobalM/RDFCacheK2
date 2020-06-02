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

        core/PredicatesIndexCache.cpp
        )

set(CORE_HEADER_FILES
        core/K2Tree.hpp
        core/Cache.hpp
        core/GraphResult.hpp
        core/RDFTriple.hpp
        core/cache_result/ExistingCacheResult.hpp
        core/cache_result/CacheResult.hpp
        core/cache_result/NonExistingCacheResult.hpp

        core/PredicatesIndexCache.hpp
        )

set(NETWORK_SOURCE_FILES
        network/CacheServer.cpp
        network/TCPServerConnection.cpp
        network/ServerTask.cpp
        network/CacheServerTaskProcessor.cpp
        network/ServerWorker.cpp
        network/Message.cpp
        )

set(NETWORK_HEADER_FILES
        network/CacheServer.hpp
        network/TCPServerConnection.hpp
        network/ServerTask.hpp
        network/CacheServerTaskProcessor.hpp
        network/ServerWorker.hpp
        network/Message.hpp
        )

set(PROTO_MESSAGES
        proto/generated/entities_mapping.pb.cc
        proto/generated/graph_result.pb.cc
        proto/generated/k2tree.pb.cc
        proto/generated/message_type.pb.cc
        proto/generated/predicates_index_cache.pb.cc
        proto/generated/request_msg.pb.cc
        proto/generated/response_msg.pb.cc
        proto/generated/radix_tree.pb.cc

        )

set(PROTO_MESSAGES_PATH ${CMAKE_SOURCE_DIR}/proto/generated)

set(INCLUDES memory core network ${PROTO_MESSAGES_PATH})

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

add_library(ProtoMessages STATIC ${PROTO_MESSAGES})


find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})




#add_executable(main main.cpp )
#add_executable(cache_example cache_example.cpp)

add_executable(example1 example/example1.cpp)

set(BASE_LIBS
        RDFCacheK2
        k2tree_merged_noalloc
        MemoryManager
        ProtoMessages
        ${Protobuf_LIBRARIES}
        )

set(DEFAULT_LIBS
        RDFCacheK2
        RDFCacheK2_Network
        k2tree_merged_noalloc
        MemoryManager
        pthread
        ProtoMessages
        ${Protobuf_LIBRARIES})

target_link_libraries(example1 ${DEFAULT_LIBS})


# index by k2tree script




set(RAPTOR_PATH lib/raptor2-2.0.15)
set(LIBXML_PATH lib/libxml2-2.9.10)
set(RAX_PATH plib/rax-31-may-2020-1927550)

add_library(Rax ${RAX_PATH}/rax.c )

link_directories(${RAPTOR_PATH}/src/.libs/ ${LIBXML_PATH}/.libs/)
include_directories(${RAPTOR_PATH}/src ${LIBXML_PATH}/include ${RAX_PATH} scripts)


add_executable(index_by_k2tree
        scripts/index_by_k2tree.cpp
        scripts/NTParser.cpp
        scripts/EntitiesMapping.cpp

        )

set(INDEX_FILE_LIBS ${BASE_LIBS}
        raptor2
        xml2
        Rax
        ProtoMessages
        ${Protobuf_LIBRARIES})

target_link_libraries(index_by_k2tree ${INDEX_FILE_LIBS})











### GOOGLE TEST CONFIG BEGIN
configure_file(test/CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
    message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
    message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
        ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
        EXCLUDE_FROM_ALL)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
    include_directories("${gtest_SOURCE_DIR}/include")
endif()

# Now simply link against gtest or gtest_main as needed. Eg
#add_executable(example example.cpp)
#target_link_libraries(example gtest_main)
#add_test(NAME example_test COMMAND example)
### GOOGLE TEST CONFIG END


add_executable(serialization_test  test/serialization_tests.cpp)
target_link_libraries(serialization_test ${DEFAULT_LIBS} gtest_main)

add_executable(index_file_test test/index_file_test.cpp
        scripts/EntitiesMapping.cpp)
target_link_libraries(index_file_test ${INDEX_FILE_LIBS} gtest_main)