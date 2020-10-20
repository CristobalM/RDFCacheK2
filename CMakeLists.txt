cmake_minimum_required(VERSION 3.14)
project(RDFCacheK2)
set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_MODULE_PATH  ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/Modules)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")


set(CORE_SOURCE_FILES
        core/K2Tree.cpp
        core/PredicatesIndexCache.cpp
        core/EntitiesMapping.cpp
        core/serialization_util.cpp
        core/PredicatesCacheManager.cpp
        core/NaiveDynamicStringDictionary.cpp
        core/ResultTable.cpp
        core/EmptyISDManager.cpp
        )

set(PROC_SOURCE_FILES
        proc/QueueIteratorDictString.cpp
        proc/SDFeed.cpp
)

set(GRAPH_RESULT_CACHING_FILE
        gresults-caching/GraphResult.cpp
        gresults-caching/Cache.cpp
        gresults-caching/cache_result/ExistingCacheResult.cpp
        gresults-caching/cache_result/NonExistingCacheResult.cpp
)

        
set(NETWORK_SOURCE_FILES
        network/CacheServer.cpp
        network/TCPServerConnection.cpp
        #network/ServerTask.cpp
        network/CacheServerTaskProcessor.cpp
        network/ServerWorker.cpp
        network/Message.cpp
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

set(INCLUDES memory core network proc ${PROTO_MESSAGES_PATH} ${CMAKE_SOURCE_DIR}/gresults-caching)

set(K2TREE_PATH ${CMAKE_SOURCE_DIR}/lib/c-k2tree-dyn)

set(INCLUDES_LIBS
        ${K2TREE_PATH}/include
        ${K2TREE_PATH}/lib/c-bitvector/include
        ${K2TREE_PATH}/lib/c-queue/include
        ${K2TREE_PATH}/lib/c-vector/include
        )


set(LIB_CSD_BPATH lib/libCSD)
set(LIB_CSD_INCLUDES
        ${LIB_CSD_BPATH}
        ${LIB_CSD_BPATH}/libcds/includes
        )


set(LINK_LIBS ${K2TREE_PATH}/bin)

add_subdirectory(${LIB_CSD_BPATH})

set(LIB_NTPARSER lib/ntparser)
set(LIB_NTPARSER_INCLUDES 
        ${LIB_NTPARSER}/include
        ${LIB_NTPARSER}/gen
)

add_subdirectory(${LIB_NTPARSER})


add_subdirectory(${K2TREE_PATH})


include_directories(${INCLUDES} ${INCLUDES_LIBS} ${LIB_CSD_INCLUDES} ${LIB_NTPARSER_INCLUDES})
link_directories(${LINK_LIBS})


add_library(MemoryManager STATIC
        memory/MemoryManager.cpp
        )

add_library(RDFCacheK2 STATIC ${CORE_SOURCE_FILES})
#add_library(RDFCacheK2_Network STATIC ${NETWORK_SOURCE_FILES})
#add_library(RDFCacheK2_GR STATIC ${GRAPH_RESULT_CACHING_FILE})

add_library(ProtoMessages STATIC ${PROTO_MESSAGES})


add_library(Proc STATIC ${PROC_SOURCE_FILES})

# add_dependencies(RDFCacheK2 _k2tree_merged_noalloc)
add_dependencies(RDFCacheK2 k2dyn)

find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})

set(RAX_PATH plib/rax-31-may-2020-1927550)
add_library(Rax ${RAX_PATH}/rax.c )
include_directories(${RAX_PATH})

#add_executable(start_cache_server_example example/start_cache_server_example.cpp)

set(BASE_LIBS
        RDFCacheK2
        k2dyn
        MemoryManager
        ProtoMessages
        ${Protobuf_LIBRARIES}
        CSD
        cds
        Rax
        )
        
set(PROC_LIBS
        Proc
        CSD
        cds
        pthread
        )

set(DEFAULT_LIBS
        RDFCacheK2
        #RDFCacheK2_Network
        k2dyn
        MemoryManager
        pthread
        ProtoMessages
        ${Protobuf_LIBRARIES}
        CSD
        cds
        Rax
        )



set(LIB_CPPBASE64_PATH lib/cpp-base64)
add_library(CppBase64 ${LIB_CPPBASE64_PATH}/base64.cpp ${LIB_CPPBASE64_PATH}/base64.h)


# target_link_libraries(start_cache_server_example RDFCacheK2_GR ${DEFAULT_LIBS} )


add_executable(sample_set scripts/sample_set.cpp)
target_link_libraries(sample_set ${BASE_LIBS})

add_executable(validate_sd_from_nt scripts/validate_sd_from_nt.cpp)
target_include_directories(validate_sd_from_nt PUBLIC ${LIB_CPPBASE64_PATH})
target_link_libraries(validate_sd_from_nt ${BASE_LIBS} CppBase64 ntparser stdc++fs)


add_executable(index_by_k2tree_with_sd scripts/index_by_k2tree_with_sd.cpp)
target_link_libraries(index_by_k2tree_with_sd ${BASE_LIBS} ntparser )


add_executable(print_k2trees scripts/print_k2trees.cpp)
target_link_libraries(print_k2trees ${BASE_LIBS} pthread)


add_executable(split_nt_triples scripts/split_nt_triples.cpp)
target_include_directories(split_nt_triples PUBLIC ${LIB_CPPBASE64_PATH})
target_link_libraries(split_nt_triples ntparser CppBase64 )


add_executable(sd_from_fileset_readfull scripts/sd_from_fileset_readfull.cpp)
target_include_directories(sd_from_fileset_readfull PUBLIC ${LIB_CPPBASE64_PATH})
target_link_libraries(sd_from_fileset_readfull ${PROC_LIBS} CppBase64)

add_executable(print_sd scripts/print_sd.cpp)
target_include_directories(print_sd PUBLIC ${LIB_CPPBASE64_PATH})
target_link_libraries(print_sd ${PROC_LIBS} CppBase64)


add_executable(base64_transform scripts/base64_transform.cpp)
target_include_directories(base64_transform PUBLIC ${LIB_CPPBASE64_PATH})
target_link_libraries(base64_transform ${PROC_LIBS} CppBase64)


add_executable(four_sided_queue_example example/four_sided_queue_example.cpp)

# Only create test executables if google test is found
find_package(GTest QUIET)
if(GTest_FOUND)
enable_testing()
include_directories(${GTEST_INCLUDE_DIRS})
add_executable(serialization_test  test/serialization_tests.cpp)
target_link_libraries(serialization_test ${DEFAULT_LIBS} ${GTEST_BOTH_LIBRARIES})

add_executable(k2tree_serialization_test  test/k2tree_serialization_test.cpp)
target_link_libraries(k2tree_serialization_test ${DEFAULT_LIBS} ${GTEST_BOTH_LIBRARIES})

add_executable(predicates_cache_manager_test  test/predicates_cache_manager_test.cpp)
target_link_libraries(predicates_cache_manager_test ${DEFAULT_LIBS} ${GTEST_BOTH_LIBRARIES})

add_executable(index_file_test test/index_file_test.cpp)
target_link_libraries(index_file_test ${BASE_LIBS} ${GTEST_BOTH_LIBRARIES})

add_executable(four_sided_queue_test test/four_sided_queue_test.cpp)
target_link_libraries(four_sided_queue_test ${GTEST_BOTH_LIBRARIES} pthread)

add_executable(k2tree_tests test/k2tree_tests.cpp)
target_link_libraries(k2tree_tests  ${DEFAULT_LIBS}  ${GTEST_BOTH_LIBRARIES} pthread)

add_executable(result_table_test test/result_table_test.cpp)
target_link_libraries(result_table_test  ${DEFAULT_LIBS}  ${GTEST_BOTH_LIBRARIES} pthread)

add_test(NAME serialization_test COMMAND ./serialization_test)
add_test(NAME k2tree_serialization_test COMMAND ./k2tree_serialization_test)
add_test(NAME predicates_cache_manager_test COMMAND ./predicates_cache_manager_test)
add_test(NAME index_file_test COMMAND ./index_file_test)
add_test(NAME four_sided_queue_test COMMAND ./four_sided_queue_test)
add_test(NAME k2tree_tests COMMAND ./k2tree_tests)
add_test(NAME result_table_test COMMAND ./result_table_test)
endif()


add_executable(k2tree_size_benchmark benchmarks/k2tree_size_benchmark.cpp)
target_link_libraries(k2tree_size_benchmark ${BASE_LIBS})
