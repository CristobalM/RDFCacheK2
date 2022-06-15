cmake_minimum_required(VERSION 3.14)
project(RDFCacheK2)
set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/Modules)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()

#set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_C_FLAGS "-Wall -Wextra -std=c99 -pedantic -Wmissing-prototypes -Wstrict-prototypes -Wold-style-definition")
set(CMAKE_CXX_FLAGS "-Wall -Wextra -std=c++17 -pedantic -Werror")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fstandalone-debug -fprofile-instr-generate -fcoverage-mapping")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}  -fprofile-arcs -ftest-coverage -lgcov --coverage")
endif ()
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

configure_file(${CMAKE_SOURCE_DIR}/CTestCustom.cmake ${CMAKE_BINARY_DIR})


add_definitions(-DLIGHT_FIELDS)


#set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fsanitize=address")
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address")
#set(CMAKE_LINKER_FLAGS_DEBUG "-fsanitize=address")


set(CORE_SOURCE_FILES
        core/K2Tree.cpp
        core/PredicatesCacheMetadata.cpp
        core/PredicatesIndexFileBuilder.cpp
        core/PredicatesIndexCacheMD.cpp
        core/PredicatesCacheManager.cpp
        core/ResultTable.cpp
        core/EmptyISDManager.cpp
        core/block_serialization.cpp
        core/K2TreeMixed.cpp
        core/PredicateFetchResult.cpp
        core/BandScanner.cpp
        core/BandScanner.hpp
        core/FullScanner.cpp
        core/FullScanner.hpp
        core/K2TreeScanner.hpp
        core/EmptyScanner.cpp
        core/EmptyScanner.hpp
        core/block_stats.cpp
        core/block_stats.hpp
        core/K2QStateWrapper.cpp
        core/K2QStateWrapper.hpp
        core/K2TreeBulkOp.cpp
        core/K2TreeBulkOp.hpp
        core/I_DataManager.hpp
        core/NullScanner.cpp
        core/NullScanner.hpp
        core/I_UpdateLoggerPCM.hpp
        core/NodeId.cpp
        core/NodeId.hpp
        core/InvalidStringSizeForNodeId.hpp
        core/TripleNodeId.cpp
        core/TripleNodeId.hpp
        core/NodesSequence.cpp
        core/NodesSequence.hpp

        core/FullyIndexedPredicate.hpp
        core/FullyIndexedPredicate.cpp
        core/FullyIndexedCache.cpp
        core/FullyIndexedCache.hpp
        core/FullyIndexedCacheResponse.cpp
        core/FullyIndexedCacheResponse.hpp

        core/UpdatesLogger.cpp
        core/UpdatesLogger.hpp
        core/UpdatesLoggerFilesManager.cpp
        core/UpdatesLoggerFilesManager.hpp

        core/K2TreeUpdates.cpp
        core/K2TreeUpdates.hpp


        core/replacement/I_PQTraverse.hpp
        core/replacement/CacheReplacement.cpp
        core/replacement/CacheReplacement.hpp
        core/replacement/CacheReplacementFactory.cpp
        core/replacement/CacheReplacementFactory.hpp
        core/replacement/FrequencyReplacementStrategy.cpp
        core/replacement/FrequencyReplacementStrategy.hpp
        core/replacement/I_CacheReplacement.hpp
        core/replacement/I_ReplacementPriorityQueue.hpp
        core/replacement/LRUReplacementStrategy.cpp
        core/replacement/LRUReplacementStrategy.hpp
        core/replacement/NoCachingReplacement.cpp
        core/replacement/NoCachingReplacement.hpp
        )


set(NETWORK_SOURCE_FILES
        network/CacheServer.cpp
        network/TCPServerConnection.cpp
        network/ServerTask.cpp
        network/CacheServerTaskProcessor.cpp
        network/ServerWorker.cpp
        network/Message.cpp
        network/ReplacementTaskProcessor.cpp
        network/ReplacementTaskProcessor.hpp
        network/ReplacementTask.cpp
        network/ReplacementTask.hpp
        network/TripleMatchesPartStreamer.cpp
        network/TripleMatchesPartStreamer.hpp
        network/I_TRStreamer.hpp
        network/I_TRMatchingStreamer.hpp
        network/TriplePatternMatchingStreamer.cpp
        network/TriplePatternMatchingStreamer.hpp
        network/I_Updater.hpp
        network/UpdaterSession.cpp
        network/UpdaterSession.hpp
        network/WriteDataLock.cpp
        network/WriteDataLock.hpp
        network/I_DataMerger.hpp

        network/I_CachedPredicateSource.hpp
        network/StreamerFromCachedSource.hpp
        network/StreamerFromCachedSource.cpp
        network/CachedSourceScanner.hpp
        network/CachedSubjectObjectScanner.hpp
        network/CachedSubjectObjectScanner.cpp
        network/CachedSubjectScanner.hpp
        network/CachedSubjectScanner.cpp
        network/CachedObjectScanner.hpp
        network/CachedObjectScanner.cpp

        network/networking_utils.cpp
        network/networking_utils.hpp
        network/InMemoryTreeMerger.cpp
        network/InMemoryTreeMerger.hpp
        )


set(PROTO_MESSAGES
        proto/generated/message_type.pb.cc
        proto/generated/request_msg.pb.cc
        proto/generated/response_msg.pb.cc
        proto/generated/sparql_tree.pb.cc
        )

set(MEMORY_SOURCES
        memory/MemoryManager.hpp
        memory/MemoryManager.cpp
        memory/MemoryPool.hpp
        memory/MemorySegment.cpp memory/MemorySegment.hpp memory/OutOfMemoryException.cpp memory/OutOfMemoryException.hpp)

set(CACHE_SOURCES
        cache/Cache.hpp
        cache/Cache.cpp
        cache/CacheArgs.hpp
        )


set(K2TREE_PATH ${CMAKE_SOURCE_DIR}/lib/c-k2tree-dyn)


set(K2TREE_SOURCES
        ${K2TREE_PATH}/src/bitvector.c
        ${K2TREE_PATH}/src/block.c
        ${K2TREE_PATH}/src/block_frontier.c
        ${K2TREE_PATH}/src/block_topology.c
        ${K2TREE_PATH}/src/custom_bv_handling.c
        ${K2TREE_PATH}/src/k2node.c
        ${K2TREE_PATH}/src/morton_code.c
        ${K2TREE_PATH}/src/queries_state.c
        ${K2TREE_PATH}/src/stacks.c
        ${K2TREE_PATH}/src/vectors.c
        )


add_library(k2dyn_custom_mem ${K2TREE_SOURCES})

set(PROTO_MESSAGES_PATH ${CMAKE_SOURCE_DIR}/proto/generated)

set(INCLUDES
        core
        network
        proc
        algorithms
        utils
        memory
        cache
        ${PROTO_MESSAGES_PATH}
        )


set(LIB_EXTERNAL_SORT_PATH lib/external-sort)

set(INCLUDES_LIBS
        ${K2TREE_PATH}/include
        ${LIB_EXTERNAL_SORT_PATH}/include
        )


set(LIB_NTPARSER lib/ntparser)
set(LIB_NTPARSER_INCLUDES
        ${LIB_NTPARSER}/include
        ${LIB_NTPARSER}/gen
        )


add_subdirectory(${LIB_NTPARSER})
# add_subdirectory(${K2TREE_PATH})


include_directories(
        ${INCLUDES}
        ${INCLUDES_LIBS}
        ${LIB_NTPARSER_INCLUDES}
)


add_library(RDFCacheK2 STATIC ${CORE_SOURCE_FILES})
add_library(RDFCacheK2_Network STATIC ${NETWORK_SOURCE_FILES})
add_library(RDFCacheK2_Memory STATIC ${MEMORY_SOURCES})
add_library(RDFCacheK2_Cache STATIC ${CACHE_SOURCES})
add_library(ProtoMessages STATIC ${PROTO_MESSAGES})


add_library(Utils utils/serialization_util.cpp
        utils/hashing.cpp
        utils/I_FileRWHandler.hpp
        utils/FileRWHandler.cpp
        utils/FileRWHandler.hpp
        utils/I_OStream.hpp
        utils/I_IStream.hpp
        utils/I_IOStream.hpp
        utils/FileIStream.cpp
        utils/FileIStream.hpp
        utils/FileOStream.cpp
        utils/FileOStream.hpp

        utils/FileIOStream.cpp utils/FileIOStream.hpp)

# add_dependencies(RDFCacheK2 _k2tree_merged_noalloc)
#add_dependencies(RDFCacheK2 k2dyn Utils)
add_dependencies(RDFCacheK2 k2dyn_custom_mem Utils)
add_dependencies(RDFCacheK2_Cache RDFCacheK2)
add_dependencies(RDFCacheK2_Network RDFCacheK2 RDFCacheK2_Cache)

#add_dependencies(RDFCacheK2_GR RDFCacheK2)


find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})

find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIRS})

find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIR})


#add_executable(start_cache_server_example example/start_cache_server_example.cpp)

set(BASE_LIBS
        RDFCacheK2
        #k2dyn
        k2dyn_custom_mem
        ProtoMessages
        ${Protobuf_LIBRARIES}
        Utils
        ${OPENSSL_LIBRARIES}
        stdc++fs
        pcrecpp
        ${CURL_LIBRARIES}
        RDFCacheK2_Memory
        )

set(PROC_LIBS
        pthread
        Utils
        )

set(DEFAULT_LIBS
        RDFCacheK2_Network
        RDFCacheK2_Cache
        RDFCacheK2
        #k2dyn
        k2dyn_custom_mem
        ProtoMessages
        ${Protobuf_LIBRARIES}
        ${Protobuf_PROTOC_LIBRARIES}
        Utils
        ${OPENSSL_LIBRARIES}
        stdc++fs
        pcrecpp
        ${CURL_LIBRARIES}
        pthread
        RDFCacheK2_Memory
        )


add_executable(external_sort scripts/triple_external_sort.cpp)
target_include_directories(external_sort PUBLIC ${INCLUDES})
target_link_libraries(external_sort pthread Utils stdc++fs)

add_executable(extract_triples_binary scripts/extract_triples_binary.cpp)
target_include_directories(extract_triples_binary PUBLIC ${INCLUDES})
target_link_libraries(extract_triples_binary ${PROC_LIBS} ntparser)


add_executable(build_k2tree_index scripts/build_k2tree_index.cpp)
target_include_directories(build_k2tree_index PUBLIC ${INCLUDES})
target_link_libraries(build_k2tree_index ${DEFAULT_LIBS})

add_executable(build_k2tree_index_debug scripts/build_k2tree_index_debug.cpp)
target_include_directories(build_k2tree_index_debug PUBLIC ${INCLUDES})
target_link_libraries(build_k2tree_index_debug ${DEFAULT_LIBS})

add_executable(print_k2tree_index scripts/print_k2tree_index.cpp)
target_include_directories(print_k2tree_index PUBLIC ${INCLUDES})
target_link_libraries(print_k2tree_index ${DEFAULT_LIBS})


add_executable(cache_server scripts/cache_server.cpp)
target_include_directories(cache_server PUBLIC ${INCLUDES})
target_link_libraries(cache_server ${DEFAULT_LIBS})

add_executable(debug_read_k2trees_metadata scripts/debug_read_k2trees_metadata.cpp)
target_include_directories(debug_read_k2trees_metadata PUBLIC ${INCLUDES})
target_link_libraries(debug_read_k2trees_metadata ${DEFAULT_LIBS})

add_executable(dataset_stats scripts/dataset_stats.cpp)
target_include_directories(dataset_stats PUBLIC ${INCLUDES})
target_link_libraries(dataset_stats ${DEFAULT_LIBS})


add_executable(convert_triples_to_binary scripts/convert_triples_to_binary.cpp)
target_include_directories(convert_triples_to_binary PUBLIC ${INCLUDES})
target_link_libraries(convert_triples_to_binary ${DEFAULT_LIBS})


add_executable(traverse_k2tree_band scripts/traverse_k2tree_band.cpp)
target_include_directories(traverse_k2tree_band PUBLIC ${INCLUDES})
target_link_libraries(traverse_k2tree_band ${DEFAULT_LIBS})


add_executable(k2tree_example example/k2tree_example.cpp)
target_link_libraries(k2tree_example ${BASE_LIBS})

add_executable(measure_serialization_time scripts/measure_serialization_time.cpp)
target_include_directories(measure_serialization_time PUBLIC ${INCLUDES})
target_link_libraries(measure_serialization_time ${DEFAULT_LIBS})

add_executable(node_ids_set_from_triples_set scripts/node_ids_set_from_triples_set.cpp)
target_include_directories(node_ids_set_from_triples_set PUBLIC ${INCLUDES})
target_link_libraries(node_ids_set_from_triples_set ${DEFAULT_LIBS})

add_executable(print_nodeids scripts/print_nodeids.cpp)
target_include_directories(print_nodeids PUBLIC ${INCLUDES})
target_link_libraries(print_nodeids ${DEFAULT_LIBS})

add_executable(translate_triples_to_nseq scripts/translate_triples_to_nseq.cpp)
target_include_directories(translate_triples_to_nseq PUBLIC ${INCLUDES})
target_link_libraries(translate_triples_to_nseq ${DEFAULT_LIBS})

add_executable(trigger_logs_sync scripts/trigger_logs_sync.cpp)
target_include_directories(trigger_logs_sync PUBLIC ${INCLUDES})
target_link_libraries(trigger_logs_sync ${DEFAULT_LIBS})

# Only create test executables if google test is found
find_package(GTest QUIET)
if (GTest_FOUND)
    enable_testing()

    set(MOCK_STRUCTURES
            test/mock_structures/DataMergerMock.hpp
            test/mock_structures/DataMergerMock.cpp
            test/mock_structures/StringIStream.hpp
            test/mock_structures/StringIStream.cpp
            test/mock_structures/StringOStream.hpp
            test/mock_structures/StringOStream.cpp
            test/mock_structures/FHMock.hpp
            test/mock_structures/FHMock.cpp
            test/mock_structures/StringIOStream.cpp test/mock_structures/StringIOStream.hpp)

    add_library(MockStructureLib STATIC ${MOCK_STRUCTURES})
    add_dependencies(MockStructureLib RDFCacheK2 Utils)

    include_directories(${GTEST_INCLUDE_DIRS})
    add_executable(serialization_test test/serialization_tests.cpp)
    target_link_libraries(serialization_test ${DEFAULT_LIBS} ${GTEST_BOTH_LIBRARIES} pthread)

    add_executable(k2tree_serialization_test test/k2tree_serialization_test.cpp)
    target_link_libraries(k2tree_serialization_test ${DEFAULT_LIBS} ${GTEST_BOTH_LIBRARIES} pthread)

    add_executable(k2tree_tests test/k2tree_tests.cpp)
    target_link_libraries(k2tree_tests ${DEFAULT_LIBS} ${GTEST_BOTH_LIBRARIES} pthread)

    add_executable(k2tree_mixed_test test/k2tree_mixed_test.cpp)
    target_link_libraries(k2tree_mixed_test ${DEFAULT_LIBS} ${GTEST_BOTH_LIBRARIES} pthread)

    add_executable(test_external_sort test/test_external_sort.cpp)
    target_link_libraries(test_external_sort ${DEFAULT_LIBS} ${GTEST_BOTH_LIBRARIES} pthread)

    add_executable(predicates_metadata_serialization_test test/predicates_metadata_serialization_test.cpp)
    target_link_libraries(predicates_metadata_serialization_test ${DEFAULT_LIBS} ${GTEST_BOTH_LIBRARIES} pthread MockStructureLib)

    add_executable(k2mixed_from_file test/k2mixed_from_file.cpp)
    target_link_libraries(k2mixed_from_file ${DEFAULT_LIBS} ${GTEST_BOTH_LIBRARIES} pthread)

    add_executable(cache_replacement_test test/cache_replacement_test.cpp test/cache_test_util.cpp)
    target_link_libraries(cache_replacement_test ${DEFAULT_LIBS} ${GTEST_BOTH_LIBRARIES} pthread MockStructureLib)

    add_executable(memory_segment_test test/memory_segment_test.cpp)
    target_link_libraries(memory_segment_test ${DEFAULT_LIBS} ${GTEST_BOTH_LIBRARIES} pthread)


    add_executable(update_log_test test/update_log_test.cpp test/cache_test_util.cpp)
    target_link_libraries(update_log_test ${DEFAULT_LIBS} ${GTEST_BOTH_LIBRARIES} MockStructureLib pthread)


    add_executable(fully_indexed_cache_test test/fully_indexed_cache_test.cpp test/cache_test_util.cpp)
    target_link_libraries(fully_indexed_cache_test ${DEFAULT_LIBS} ${GTEST_BOTH_LIBRARIES} MockStructureLib pthread)

    add_executable(predicates_cache_manager_test test/predicates_cache_manager_test.cpp)
    target_link_libraries(predicates_cache_manager_test ${DEFAULT_LIBS} ${GTEST_BOTH_LIBRARIES} MockStructureLib pthread)

    add_executable(nodes_map_test test/nodes_map_test.cpp)
    target_link_libraries(nodes_map_test ${DEFAULT_LIBS} ${GTEST_BOTH_LIBRARIES} MockStructureLib pthread)


    add_test(NAME serialization_test COMMAND ./serialization_test)
    add_test(NAME k2tree_serialization_test COMMAND ./k2tree_serialization_test)
    add_test(NAME k2tree_tests COMMAND ./k2tree_tests)
    add_test(NAME k2tree_mixed_test COMMAND ./k2tree_mixed_test)
    add_test(NAME test_external_sort COMMAND ./test_external_sort)
    add_test(NAME predicates_metadata_serialization_test COMMAND ./predicates_metadata_serialization_test)
    add_test(NAME cache_replacement_test COMMAND ./cache_replacement_test)
    add_test(NAME memory_segment_test COMMAND ./memory_segment_test)
    add_test(NAME update_log_test COMMAND ./update_log_test)
    add_test(NAME fully_indexed_cache_test COMMAND ./fully_indexed_cache_test)
    add_test(NAME predicates_cache_manager_test COMMAND ./predicates_cache_manager_test)
    add_test(NAME nodes_map_test COMMAND ./nodes_map_test)

    # K2 Tree Tests

    add_executable(block_test ${K2TREE_PATH}/test/block_test.cpp ${K2TREE_PATH}/test/block_wrapper.hpp)
    add_executable(block_leak_test ${K2TREE_PATH}/test/block_leak_test.cpp ${K2TREE_PATH}/test/block_wrapper.hpp)
    add_executable(morton_code_test ${K2TREE_PATH}/test/morton_code_test.cpp)
    add_executable(block_usages_test ${K2TREE_PATH}/test/block_usages_test.cpp)
    add_executable(debug_insertion_out_of_bounds_1_test ${K2TREE_PATH}/test/debug_insertion_out_of_bounds_1_test.cpp)
    add_executable(interactive_report_test ${K2TREE_PATH}/test/interactive_report_test.cpp)
    add_executable(bitvector_test ${K2TREE_PATH}/test/bitvector_test.cpp)
    add_executable(k2node_test ${K2TREE_PATH}/test/k2node_test.cpp)
    add_executable(lazy_scan_test ${K2TREE_PATH}/test/lazy_scan_test.cpp)
    add_executable(block_delete_test ${K2TREE_PATH}/test/block_delete_test.cpp)
    add_executable(k2node_delete_test ${K2TREE_PATH}/test/k2node_delete_test.cpp)

    target_link_libraries(block_test ${GTEST_BOTH_LIBRARIES} pthread k2dyn_custom_mem RDFCacheK2_Memory)
    target_link_libraries(block_leak_test ${GTEST_BOTH_LIBRARIES} pthread k2dyn_custom_mem RDFCacheK2_Memory)
    target_link_libraries(morton_code_test ${GTEST_BOTH_LIBRARIES} pthread k2dyn_custom_mem RDFCacheK2_Memory)
    target_link_libraries(block_usages_test ${GTEST_BOTH_LIBRARIES} pthread k2dyn_custom_mem RDFCacheK2_Memory)
    target_link_libraries(debug_insertion_out_of_bounds_1_test k2dyn_custom_mem ${GTEST_BOTH_LIBRARIES} pthread RDFCacheK2_Memory)
    target_link_libraries(interactive_report_test k2dyn_custom_mem ${GTEST_BOTH_LIBRARIES} pthread RDFCacheK2_Memory)
    target_link_libraries(bitvector_test k2dyn_custom_mem ${GTEST_BOTH_LIBRARIES} pthread RDFCacheK2_Memory)
    target_link_libraries(k2node_test k2dyn_custom_mem ${GTEST_BOTH_LIBRARIES} pthread RDFCacheK2_Memory)
    target_link_libraries(lazy_scan_test k2dyn_custom_mem ${GTEST_BOTH_LIBRARIES} pthread RDFCacheK2_Memory)
    target_link_libraries(block_delete_test k2dyn_custom_mem ${GTEST_BOTH_LIBRARIES} pthread RDFCacheK2_Memory)
    target_link_libraries(k2node_delete_test k2dyn_custom_mem ${GTEST_BOTH_LIBRARIES} pthread RDFCacheK2_Memory)

    add_test(NAME block_test COMMAND ./block_test)
    add_test(NAME block_leak_test COMMAND ./block_leak_test)
    add_test(NAME morton_code_test COMMAND ./morton_code_test)
    add_test(NAME block_usages_test COMMAND ./block_usages_test)
    add_test(NAME interactive_report_test COMMAND ./interactive_report_test)
    add_test(NAME k2node_test COMMAND ./k2node_test)
    add_test(NAME lazy_scan_test COMMAND ./lazy_scan_test)
    add_test(NAME block_delete_test COMMAND ./block_delete_test)
    add_test(NAME k2node_delete_test COMMAND ./k2node_delete_test)

endif ()


add_executable(k2tree_size_benchmark benchmarks/k2tree_size_benchmark.cpp)
target_link_libraries(k2tree_size_benchmark ${BASE_LIBS})

add_executable(k2tree_mixed_benchmark benchmarks/k2tree_mixed_benchmark.cpp)
target_link_libraries(k2tree_mixed_benchmark ${BASE_LIBS})

add_executable(compare_64_32_depths_performance benchmarks/compare_64_32_depths_performance.cpp)
target_include_directories(compare_64_32_depths_performance PUBLIC ${INCLUDES})
target_link_libraries(compare_64_32_depths_performance ${DEFAULT_LIBS})

add_executable(compare_64_32_depths_performance_2 benchmarks/compare_64_32_depths_performance_2.cpp)
target_include_directories(compare_64_32_depths_performance_2 PUBLIC ${INCLUDES})
target_link_libraries(compare_64_32_depths_performance_2 ${DEFAULT_LIBS})

add_executable(compare_64_32_depths_performance_3 benchmarks/compare_64_32_depths_performance_3.cpp)
target_include_directories(compare_64_32_depths_performance_3 PUBLIC ${INCLUDES})
target_link_libraries(compare_64_32_depths_performance_3 ${DEFAULT_LIBS})

add_executable(lazy_scan_benchmark benchmarks/lazy_scan_benchmark.cpp)
target_include_directories(lazy_scan_benchmark PUBLIC ${INCLUDES})
target_link_libraries(lazy_scan_benchmark ${DEFAULT_LIBS})
