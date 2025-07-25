cmake_minimum_required(VERSION 3.14)
project(RDFCacheK2)
#set(CMAKE_VERBOSE_MAKEFILE ON)

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

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_link_options("-rdynamic")
    add_compile_definitions(CACHE_DEBUG)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "-O3")

configure_file(${CMAKE_SOURCE_DIR}/CTestCustom.cmake ${CMAKE_BINARY_DIR})


add_definitions(-DLIGHT_FIELDS)


#set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fsanitize=address")
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address")
#set(CMAKE_LINKER_FLAGS_DEBUG "-fsanitize=address")


set(CORE_SOURCE_FILES
        core/util_algorithms/fisher_yates.hpp

        core/builder/PredicatesIndexFileBuilder.cpp
        core/builder/PredicatesIndexFileBuilder.hpp
        core/builder/TriplesFeedSortedByPredicate.hpp
        core/builder/TriplesFeedSBPFromIstream.cpp
        core/builder/TriplesFeedSBPFromIstream.hpp
        core/builder/K2TreesFeed.hpp
        core/builder/K2TreesFeedFromSortedTriplesFeed.cpp
        core/builder/K2TreesFeedFromSortedTriplesFeed.hpp

        core/fic/CacheDataManager.cpp
        core/fic/CacheDataManager.hpp
        core/fic/FICFactory.cpp
        core/fic/FICFactory.hpp
        core/fic/FullyIndexedCache.hpp
        core/fic/FullyIndexedCacheImpl.cpp
        core/fic/FullyIndexedCacheImpl.hpp
        core/fic/FullyIndexedCacheResponse.cpp
        core/fic/FullyIndexedCacheResponse.hpp
        core/fic/FullyIndexedPredicate.cpp
        core/fic/FullyIndexedPredicate.hpp
        core/fic/I_CachedPredicateSource.hpp
        core/fic/NoFIC.cpp
        core/fic/NoFIC.hpp
        core/fic/types.hpp

        core/k2tree/BandScanner.cpp
        core/k2tree/BandScanner.hpp
        core/k2tree/EmptyScanner.cpp
        core/k2tree/EmptyScanner.hpp
        core/k2tree/FullScanner.cpp
        core/k2tree/FullScanner.hpp
        core/k2tree/K2QStateWrapper.cpp
        core/k2tree/K2QStateWrapper.hpp
        core/k2tree/K2TreeBulkOp.cpp
        core/k2tree/K2TreeBulkOp.hpp
        core/k2tree/K2TreeMixed.cpp
        core/k2tree/K2TreeScanner.hpp
        core/k2tree/NullScanner.cpp
        core/k2tree/NullScanner.hpp
        core/k2tree/block_serialization.cpp
        core/k2tree/block_stats.cpp
        core/k2tree/block_stats.hpp

        core/manager/DataManager.hpp
        core/manager/K2TreeFetcher.hpp
        core/manager/PCMFactory.cpp
        core/manager/PCMFactory.hpp
        core/manager/PredicateFetchResult.cpp
        core/manager/PredicatesCacheManager.hpp
        core/manager/PredicatesCacheManagerImpl.cpp
        core/manager/PredicatesCacheMetadata.cpp
        core/manager/PredicatesIndexCacheMD.cpp

        core/nodeids/NodeId.cpp
        core/nodeids/NodeId.hpp
        core/nodeids/NodeIdsManager.hpp
        core/nodeids/NodeIdsManagerFactory.cpp
        core/nodeids/NodeIdsManagerFactory.hpp
        core/nodeids/NodeIdsManagerIdentity.cpp
        core/nodeids/NodeIdsManagerIdentity.hpp
        core/nodeids/NodeIdsManagerImpl.cpp
        core/nodeids/NodeIdsManagerImpl.hpp
        core/nodeids/NodesMap.hpp
        core/nodeids/NodesMapFactory.cpp
        core/nodeids/NodesMapFactory.hpp
        core/nodeids/NodesMapImpl.cpp
        core/nodeids/NodesMapImpl.hpp
        core/nodeids/NodesSequence.cpp
        core/nodeids/NodesSequence.hpp
        core/nodeids/TripleNodeId.cpp
        core/nodeids/TripleNodeId.hpp
        core/nodeids/node_ids_constants.hpp

        core/replacement/CacheReplacement.cpp
        core/replacement/CacheReplacement.hpp
        core/replacement/CacheReplacementFactory.cpp
        core/replacement/CacheReplacementFactory.hpp
        core/replacement/FrequencyReplacementStrategy.cpp
        core/replacement/FrequencyReplacementStrategy.hpp
        core/replacement/I_CacheReplacement.hpp
        core/replacement/I_PQTraverse.hpp
        core/replacement/I_ReplacementPriorityQueue.hpp
        core/replacement/LRUReplacementStrategy.cpp
        core/replacement/LRUReplacementStrategy.hpp
        core/replacement/NoCachingReplacement.cpp
        core/replacement/NoCachingReplacement.hpp

        core/updating/DataMerger.hpp
        core/updating/K2TreeUpdates.cpp
        core/updating/K2TreeUpdates.hpp
        core/updating/NoUpdate.cpp
        core/updating/NoUpdate.hpp
        core/updating/PCMMergerWrapper.cpp
        core/updating/PCMMergerWrapper.hpp
        core/updating/UpdatesLogger.hpp
        core/updating/UpdatesLoggerFilesManager.cpp
        core/updating/UpdatesLoggerFilesManager.hpp
        core/updating/UpdatesLoggerImpl.cpp
        core/updating/UpdatesLoggerImpl.hpp

        core/algorithms/search_paths.cpp
        core/algorithms/search_paths.hpp
        core/algorithms/DirectedPath.hpp
        core/algorithms/DirectedPath.cpp

        core/util_algorithms/fisher_yates.cpp
        core/util_algorithms/random_dataset_generation.cpp
        core/util_algorithms/random_dataset_generation.hpp
        )


set(NETWORK_SOURCE_FILES
        network/messages/Message.cpp
        network/messages/utils.cpp
        network/messages/utils.hpp

        network/scanner/CachedObjectScanner.cpp
        network/scanner/CachedObjectScanner.hpp
        network/scanner/CachedSourceScanner.hpp
        network/scanner/CachedSubjectObjectScanner.cpp
        network/scanner/CachedSubjectObjectScanner.hpp
        network/scanner/CachedSubjectScanner.cpp
        network/scanner/CachedSubjectScanner.hpp

        network/server/CacheServer.cpp
        network/server/ServerWorker.cpp
        network/server/conn/TCPServerConnection.cpp
        network/server/replacement/ReplacementTask.cpp
        network/server/replacement/ReplacementTask.hpp
        network/server/replacement/ReplacementTaskProcessor.cpp
        network/server/replacement/ReplacementTaskProcessor.hpp
        network/server/session/Updater.hpp
        network/server/session/UpdaterSession.cpp
        network/server/session/UpdaterSession.hpp
        network/server/tasks/CacheServerTaskProcessor.cpp
        network/server/tasks/ServerTask.cpp
        network/server/tasks/WriteDataLock.cpp
        network/server/tasks/WriteDataLock.hpp

        network/streaming/I_TRMatchingStreamer.hpp
        network/streaming/I_TRStreamer.hpp
        network/streaming/StreamerFromCachedSource.cpp
        network/streaming/StreamerFromCachedSource.hpp
        network/streaming/TripleMatchesPartStreamer.cpp
        network/streaming/TripleMatchesPartStreamer.hpp
        network/streaming/TriplePatternMatchingStreamer.cpp
        network/streaming/TriplePatternMatchingStreamer.hpp
        network/server/ClientReqHandler.hpp
        network/server/ClientReqHandlerImpl.cpp
        network/server/ClientReqHandlerImpl.hpp
        network/streaming/TPMSortedStreamer.cpp
        network/streaming/TPMSortedStreamer.hpp
        network/streaming/I_BGPStreamer.hpp
        network/streaming/BgpStreamer.cpp
        network/streaming/BGPStreamer.hpp
)

file(GLOB_RECURSE QUERY_ENGINE_FILES
        ${PROJECT_SOURCE_DIR}/query_engine/*.cpp
        ${PROJECT_SOURCE_DIR}/query_engine/*.hpp
        )

add_library(QueryEngine OBJECT ${QUERY_ENGINE_FILES})

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
        cache/CacheContainer.hpp
        cache/CacheContainerImpl.hpp
        cache/CacheContainerImpl.cpp
        cache/CacheContainerFactory.cpp
        cache/CacheContainerFactory.hpp
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
        sort
        query_engine
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

target_link_libraries(RDFCacheK2_Network QueryEngine)
target_link_libraries(QueryEngine RDFCacheK2)


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

        utils/FileIOStream.cpp
        utils/FileIOStream.hpp


        utils/sort/triples_to_node_ids_file.hpp
        utils/sort/triples_to_node_ids_file.cpp
        utils/sort/ULConnectorWHeaderCustomSerialization.hpp
        utils/sort/ULHeaderIOHandlerCustomSerialization.hpp

        utils/custom_endian.hpp
        )

# add_dependencies(RDFCacheK2 _k2tree_merged_noalloc)
#add_dependencies(RDFCacheK2 k2dyn Utils)
add_dependencies(RDFCacheK2 k2dyn_custom_mem Utils)
add_dependencies(RDFCacheK2_Cache RDFCacheK2)
add_dependencies(RDFCacheK2_Network RDFCacheK2 RDFCacheK2_Cache)

#add_dependencies(RDFCacheK2_GR RDFCacheK2)


find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})

find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})
message("OPENSSL INCLUDE DIRS: ${OPENSSL_INCLUDE_DIR}" ${OPENSSL_INCLUDE_DIR})
message("OPENSSL LIBS DIRS: ${OPENSSL_LIBRARIES}")

find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIR})


#add_executable(start_cache_server_example example/start_cache_server_example.cpp)

if (APPLE)
    set(STDLIB_OPT "")
else()
    set(STDLIB_OPT stdc++fs)
endif ()

set(BASE_LIBS
        RDFCacheK2
        #k2dyn
        k2dyn_custom_mem
        ProtoMessages
        ${Protobuf_LIBRARIES}
        Utils
        ${OPENSSL_LIBRARIES}
        ${STDLIB_OPT}
        #pcrecpp
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
        ${STDLIB_OPT}
        #pcrecpp
        ${CURL_LIBRARIES}
        pthread
        RDFCacheK2_Memory
        )


add_executable(external_sort scripts/triple_external_sort.cpp)
target_include_directories(external_sort PUBLIC ${INCLUDES})
target_link_libraries(external_sort pthread Utils ${STDLIB_OPT})

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

add_executable(search_paths scripts/search_paths.cpp)
target_include_directories(search_paths PUBLIC ${INCLUDES})
target_link_libraries(search_paths ${DEFAULT_LIBS})

add_executable(random_dataset_gen scripts/random_dataset_gen.cpp)
target_include_directories(random_dataset_gen PUBLIC ${INCLUDES})
target_link_libraries(random_dataset_gen ${DEFAULT_LIBS})

add_executable(random_nt_dataset_generate scripts/random_nt_dataset_generate.cpp)
target_include_directories(random_nt_dataset_generate PUBLIC ${INCLUDES})
target_link_libraries(random_nt_dataset_generate ${DEFAULT_LIBS})

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
            test/mock_structures/StringIOStream.cpp
            test/mock_structures/StringIOStream.hpp
            test/mock_structures/TestingTaskProcessor.cpp
            test/mock_structures/TestingTaskProcessor.hpp
            test/mock_structures/MockFetcher.cpp
            test/mock_structures/MockFetcher.hpp
            test/mock_structures/MockClientReqHandler.cpp
            test/mock_structures/MockClientReqHandler.hpp
            test/mock_structures/MockUpdatesLogger.cpp
            test/mock_structures/MockUpdatesLogger.hpp)

    add_library(MockStructureLib STATIC ${MOCK_STRUCTURES})
    add_dependencies(MockStructureLib RDFCacheK2 RDFCacheK2_Network RDFCacheK2_Cache Utils)

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

    add_executable(predicates_metadata_serialization_test test/predicates_metadata_serialization_test.cpp test/cache_test_util.cpp)
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

    add_executable(nodes_map_test test/nodes_map_test.cpp)
    target_link_libraries(nodes_map_test ${DEFAULT_LIBS} ${GTEST_BOTH_LIBRARIES} MockStructureLib pthread)

    add_executable(node_ids_dyn_mapper_test test/node_ids_dyn_mapper_test.cpp test/cache_test_util.cpp)
    target_link_libraries(node_ids_dyn_mapper_test MockStructureLib ${DEFAULT_LIBS} ${GTEST_BOTH_LIBRARIES} pthread)

    add_executable(nodeids_manager_impl_test test/nodeids_manager_impl_test.cpp test/cache_test_util.cpp)
    target_link_libraries(nodeids_manager_impl_test MockStructureLib ${DEFAULT_LIBS} ${GTEST_BOTH_LIBRARIES} pthread)

    add_executable(nodeids_build_flow_test test/nodeids_build_flow_test.cpp test/cache_test_util.cpp)
    target_link_libraries(nodeids_build_flow_test MockStructureLib ${DEFAULT_LIBS} ${GTEST_BOTH_LIBRARIES} pthread)

    add_executable(server_task_test test/server_task_test.cpp test/cache_test_util.cpp)
    target_link_libraries(server_task_test MockStructureLib ${DEFAULT_LIBS} ${GTEST_BOTH_LIBRARIES} pthread)

    add_executable(sort_results_test test/sort_results_test.cpp test/cache_test_util.cpp)
    target_link_libraries(sort_results_test MockStructureLib ${DEFAULT_LIBS} ${GTEST_BOTH_LIBRARIES} pthread)

    add_executable(search_paths_test test/search_paths_test.cpp test/cache_test_util.cpp)
    target_link_libraries(search_paths_test MockStructureLib ${DEFAULT_LIBS} ${GTEST_BOTH_LIBRARIES} pthread)


    add_executable(random_dataset_gen_test test/random_dataset_gen_test.cpp test/cache_test_util.cpp)
    target_link_libraries(random_dataset_gen_test MockStructureLib ${DEFAULT_LIBS} ${GTEST_BOTH_LIBRARIES} pthread)

    add_executable(bgp_ops_test test/bgp_ops_test.cpp test/cache_test_util.cpp)
    target_link_libraries(bgp_ops_test MockStructureLib ${DEFAULT_LIBS} ${GTEST_BOTH_LIBRARIES} pthread)

    add_executable(bgp_processor_test test/bgp_processor_test.cpp test/cache_test_util.cpp)
    target_link_libraries(bgp_processor_test MockStructureLib ${DEFAULT_LIBS} ${GTEST_BOTH_LIBRARIES} pthread)

    add_executable(bgp_streamer_test test/bgp_streamer_test.cpp test/cache_test_util.cpp test/bgp_test_util.hpp)
    target_link_libraries(bgp_streamer_test MockStructureLib ${DEFAULT_LIBS} ${GTEST_BOTH_LIBRARIES} pthread)

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
    add_test(NAME nodes_map_test COMMAND ./nodes_map_test)
    add_test(NAME node_ids_dyn_mapper_test COMMAND ./node_ids_dyn_mapper_test)
    add_test(NAME nodeids_manager_impl_test COMMAND ./nodeids_manager_impl_test)
    add_test(NAME server_task_test COMMAND ./server_task_test)
    add_test(NAME nodeids_build_flow_test COMMAND ./nodeids_build_flow_test)
    add_test(NAME search_paths_test COMMAND ./search_paths_test)
    add_test(NAME random_dataset_gen_test COMMAND ./random_dataset_gen_test)
    add_test(NAME bgp_ops_test COMMAND ./bgp_ops_test)
    add_test(NAME bgp_processor_test COMMAND ./bgp_processor_test)

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


add_executable(points_size_relationship benchmarks/points_size_relationship.cpp)
target_link_libraries(points_size_relationship ${BASE_LIBS})


add_executable(build_from_raw_points experiments/build_from_raw_points.cpp)
target_link_libraries(build_from_raw_points ${DEFAULT_LIBS})

add_executable(deserialize_k2tree experiments/deserialize_k2tree.cpp)
target_link_libraries(deserialize_k2tree ${DEFAULT_LIBS})

add_executable(validate_k2tree_correctness experiments/validate_k2tree_correctness.cpp)
target_include_directories(validate_k2tree_correctness PUBLIC benchmarks)
target_link_libraries(validate_k2tree_correctness ${DEFAULT_LIBS})

add_executable(insertion_speed benchmarks/insertion_speed.cpp)
target_include_directories(insertion_speed PUBLIC benchmarks)
target_link_libraries(insertion_speed ${DEFAULT_LIBS})

add_executable(cluster_insertion_speed benchmarks/cluster_insertion_speed.cpp)
target_include_directories(cluster_insertion_speed PUBLIC benchmarks)
target_link_libraries(cluster_insertion_speed ${DEFAULT_LIBS})
