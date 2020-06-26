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
        core/NTParser.cpp
        core/serialization_util.cpp
        core/PredicatesCacheManager.cpp
        core/raptor_util.cpp
        core/QueueIteratorDictString.cpp
        core/SDFeed.cpp
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
        network/ServerTask.cpp
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

set(INCLUDES memory core network ${PROTO_MESSAGES_PATH} ${CMAKE_SOURCE_DIR}/gresults-caching)

set(K2TREE_PATH ${CMAKE_SOURCE_DIR}/lib/c-k2tree-dyn)


set(K2TREE_LIBFILE libk2tree_merged_noalloc.a)

add_custom_command(OUTPUT ${K2TREE_LIBFILE}
        COMMAND make
        COMMAND cp bin/libk2tree_merged_noalloc.a ${CMAKE_BINARY_DIR}
        WORKING_DIRECTORY ${K2TREE_PATH}
        )
add_custom_target(_k2tree_target ALL DEPENDS ${K2TREE_LIBFILE})
add_library(_k2tree_merged_noalloc STATIC IMPORTED GLOBAL)
add_dependencies(_k2tree_merged_noalloc _k2tree_target)
set_target_properties(_k2tree_merged_noalloc
        PROPERTIES
        IMPORTED_LOCATION ${K2TREE_PATH}/libk2tree_merged_noalloc.a
        INTERFACE_INCLUDE_DIRECTORIES ${K2TREE_LIB_LOCATION}/include
        )



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

include_directories(${INCLUDES} ${INCLUDES_LIBS} ${LIB_CSD_INCLUDES})
link_directories(${LINK_LIBS})


add_library(MemoryManager STATIC
        memory/pooled_memory_impl.cpp
        memory/MemoryManager.cpp
        )

add_library(RDFCacheK2 STATIC ${CORE_SOURCE_FILES})
add_library(RDFCacheK2_Network STATIC ${NETWORK_SOURCE_FILES})
add_library(RDFCacheK2_GR STATIC ${GRAPH_RESULT_CACHING_FILE})

add_library(ProtoMessages STATIC ${PROTO_MESSAGES})

add_dependencies(RDFCacheK2 _k2tree_merged_noalloc)

find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})

# include(3rdparty-libs.cmake)

set(RAX_PATH plib/rax-31-may-2020-1927550)
add_library(Rax ${RAX_PATH}/rax.c )
include_directories(${RAX_PATH})

find_package(LibXml2 REQUIRED)
include_directories(${LibXml2_INCLUDE_DIRS})
find_package(Raptor2 REQUIRED)
include_directories(${RAPTOR2_INCLUDE_DIR})


add_executable(start_cache_server_example example/start_cache_server_example.cpp)

set(BASE_LIBS
        RDFCacheK2
        k2tree_merged_noalloc
        MemoryManager
        ProtoMessages
        ${Protobuf_LIBRARIES}
        CSD
        cds
        Rax
        raptor2
        xml2
        )

set(DEFAULT_LIBS
        RDFCacheK2
        RDFCacheK2_Network
        k2tree_merged_noalloc
        MemoryManager
        pthread
        ProtoMessages
        ${Protobuf_LIBRARIES}
        CSD
        cds
        Rax
        raptor2
        xml2
        )



set(LIB_CPPBASE64_PATH lib/cpp-base64)
add_library(CppBase64 ${LIB_CPPBASE64_PATH}/base64.cpp ${LIB_CPPBASE64_PATH}/base64.h)




target_link_libraries(start_cache_server_example RDFCacheK2_GR ${DEFAULT_LIBS} )


# index by k2tree script


add_executable(index_by_k2tree
        scripts/index_by_k2tree.cpp
        )

if(NOT ${THIRD_PARTY_DEPS_SZ} EQUAL 0)
    add_dependencies(index_by_k2tree ${THIRD_PARTY_DEPS})
endif()



target_link_libraries(index_by_k2tree ${BASE_LIBS})



add_executable(radix_set_from_ntfile scripts/radix_set_from_ntfile.cpp)
target_link_libraries(radix_set_from_ntfile ${BASE_LIBS})


add_executable(sample_set scripts/sample_set.cpp)
target_link_libraries(sample_set ${BASE_LIBS})

add_executable(dicts_build_from_sets scripts/dicts_build_from_sets.cpp)
target_link_libraries(dicts_build_from_sets ${BASE_LIBS})

add_executable(validate_radix_sets_from_nt scripts/validate_radix_sets_from_nt.cpp)
target_link_libraries(validate_radix_sets_from_nt ${BASE_LIBS})

add_executable(validate_sd_from_nt scripts/validate_sd_from_nt.cpp)
target_include_directories(validate_sd_from_nt PUBLIC ${LIB_CPPBASE64_PATH})
target_link_libraries(validate_sd_from_nt ${BASE_LIBS} CppBase64)


add_executable(index_by_k2tree_with_sd scripts/index_by_k2tree_with_sd.cpp)
target_link_libraries(index_by_k2tree_with_sd ${BASE_LIBS})


add_executable(sd_from_ntfile scripts/sd_from_ntfile.cpp)
target_link_libraries(sd_from_ntfile ${BASE_LIBS})



add_executable(split_nt_triples scripts/split_nt_triples.cpp)
target_include_directories(split_nt_triples PUBLIC ${LIB_CPPBASE64_PATH})
target_link_libraries(split_nt_triples ${BASE_LIBS} CppBase64)


add_executable(sd_from_fileset scripts/sd_from_fileset.cpp)
target_link_libraries(sd_from_fileset ${BASE_LIBS})


add_executable(sd_from_fileset_readfull scripts/sd_from_fileset_readfull.cpp)
target_link_libraries(sd_from_fileset_readfull ${BASE_LIBS})

add_executable(print_sd scripts/print_sd.cpp)
target_link_libraries(print_sd ${BASE_LIBS})







# Only create test executables if google test is found
find_package(GTest QUIET)
if(GTest_FOUND)
include_directories(${GTEST_INCLUDE_DIRS})
add_executable(serialization_test  test/serialization_tests.cpp)
target_link_libraries(serialization_test ${DEFAULT_LIBS} ${GTEST_BOTH_LIBRARIES})

add_executable(index_file_test test/index_file_test.cpp)
target_link_libraries(index_file_test ${BASE_LIBS} ${GTEST_BOTH_LIBRARIES})
endif()