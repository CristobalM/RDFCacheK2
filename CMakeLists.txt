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
set(CMAKE_CXX_FLAGS_RELEASE "-O3")


add_definitions(-DLIGHT_FIELDS)


#set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fsanitize=address")
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address")
#set(CMAKE_LINKER_FLAGS_DEBUG "-fsanitize=address")


set(CORE_SOURCE_FILES
        core/K2Tree.cpp
        core/PredicatesCacheMetadata.cpp
        core/PredicatesIndexFileBuilder.cpp
        core/PredicatesIndexCacheMD.cpp
        core/PredicatesIndexCacheMDFile.cpp
        core/PredicatesCacheManager.cpp
        core/NaiveDynamicStringDictionary.cpp
        core/ResultTable.cpp
        core/EmptyISDManager.cpp
        core/block_serialization.cpp
        core/K2TreeMixed.cpp
        core/CacheSettings.cpp
        core/InMemoryCacheSettings.cpp
        )

set(PROC_SOURCE_FILES
        proc/QueueIteratorDictString.cpp
        proc/SDFeed.cpp
        proc/SDBuilder.cpp

        )

set(GRAPH_RESULT_CACHING_FILE
        gresults-caching/GraphResult.cpp
        gresults-caching/Cache.cpp
        gresults-caching/CacheReplacementFactory.cpp
        gresults-caching/LRUCacheReplacement.cpp
        gresults-caching/cache_result/ExistingCacheResult.cpp
        gresults-caching/cache_result/NonExistingCacheResult.cpp
        gresults-caching/LRU/LRUQueue.cpp
        gresults-caching/LRUCacheReplacement.cpp
        gresults-caching/query_processing/QueryProcessor.cpp
        gresults-caching/query_processing/KeyPairStr.cpp
        gresults-caching/query_processing/QueryResult.cpp
        gresults-caching/query_processing/Triple.cpp
        gresults-caching/query_processing/hash_pair.cpp
        gresults-caching/query_processing/GroupedTriples.cpp
        gresults-caching/query_processing/ChainedInput.cpp
        gresults-caching/query_processing/BGPProcessor.cpp
        gresults-caching/query_processing/VarIndexManager.cpp
        gresults-caching/query_processing/BandMap.cpp
        gresults-caching/query_processing/BandMapLazy.cpp
        gresults-caching/query_processing/UnionProcessor.cpp
        gresults-caching/query_processing/OptionalProcessor.cpp
        gresults-caching/query_processing/ExprProcessor.cpp
        gresults-caching/query_processing/ExprListProcessor.cpp
        gresults-caching/query_processing/ExprProcessorPersistentData.cpp

        gresults-caching/query_processing/expr/IsBlankEval.cpp
        gresults-caching/query_processing/expr/ExprEval.cpp
        gresults-caching/query_processing/expr/TermEval.cpp
        gresults-caching/query_processing/expr/IsLiteralEval.cpp
        gresults-caching/query_processing/expr/IsIRIEval.cpp
        gresults-caching/query_processing/expr/BoundEval.cpp
        gresults-caching/query_processing/expr/IsNumericEval.cpp
        gresults-caching/query_processing/expr/DataTypeEval.cpp
        gresults-caching/query_processing/expr/DateTimeDayEval.cpp
        gresults-caching/query_processing/expr/DateTimeHoursEval.cpp
        gresults-caching/query_processing/expr/DateTimeMinutesEval.cpp
        gresults-caching/query_processing/expr/DateTimeMonthEval.cpp
        gresults-caching/query_processing/expr/DateTimeTZEval.cpp
        gresults-caching/query_processing/expr/DateTimeYearEval.cpp
        gresults-caching/query_processing/expr/FunIRIEval.cpp
        gresults-caching/query_processing/expr/LangEval.cpp
        gresults-caching/query_processing/expr/LogicalNotEval.cpp
        gresults-caching/query_processing/expr/NumAbsEval.cpp
        gresults-caching/query_processing/expr/NumCeilingEval.cpp
        gresults-caching/query_processing/expr/NumFloorEval.cpp
        gresults-caching/query_processing/expr/NumRoundEval.cpp
        gresults-caching/query_processing/expr/StrEval.cpp
        gresults-caching/query_processing/expr/StrEncodeForURIEval.cpp
        gresults-caching/query_processing/expr/StrLengthEval.cpp
        gresults-caching/query_processing/expr/StrLowerCaseEval.cpp
        gresults-caching/query_processing/expr/StrUpperCaseCaseEval.cpp
        gresults-caching/query_processing/expr/UnaryMinusEval.cpp
        gresults-caching/query_processing/expr/UnaryPlusEval.cpp
        gresults-caching/query_processing/expr/DigestSHA1Eval.cpp
        gresults-caching/query_processing/expr/DigestMD5Eval.cpp
        gresults-caching/query_processing/expr/DigestSHA224Eval.cpp
        gresults-caching/query_processing/expr/DigestSHA256Eval.cpp
        gresults-caching/query_processing/expr/DigestSHA384Eval.cpp
        gresults-caching/query_processing/expr/DigestSHA512Eval.cpp
        gresults-caching/query_processing/expr/LangMatchesEval.cpp
        gresults-caching/query_processing/expr/AddEval.cpp
        gresults-caching/query_processing/expr/CastEval.cpp
        gresults-caching/query_processing/expr/DivideEval.cpp
        gresults-caching/query_processing/expr/EqualsEval.cpp
        gresults-caching/query_processing/expr/GreaterThanEval.cpp
        gresults-caching/query_processing/expr/GreaterThanOrEqualEval.cpp
        gresults-caching/query_processing/expr/LessThanEval.cpp
        gresults-caching/query_processing/expr/LessThanOrEqualEval.cpp
        gresults-caching/query_processing/expr/LogicalAndEval.cpp
        gresults-caching/query_processing/expr/LogicalOrEval.cpp
        gresults-caching/query_processing/expr/MultiplyEval.cpp
        gresults-caching/query_processing/expr/NotEqualsEval.cpp
        gresults-caching/query_processing/expr/SameTermEval.cpp
        gresults-caching/query_processing/expr/StrAfterEval.cpp
        gresults-caching/query_processing/expr/StrBeforeEval.cpp
        gresults-caching/query_processing/expr/StrContainsEval.cpp
        gresults-caching/query_processing/expr/StrDataTypeEval.cpp
        gresults-caching/query_processing/expr/StrEndsWithEval.cpp
        gresults-caching/query_processing/expr/StrLangEval.cpp
        gresults-caching/query_processing/expr/StrStartsWithEval.cpp
        gresults-caching/query_processing/expr/SubtractEval.cpp
        gresults-caching/query_processing/expr/ConditionalEval.cpp
        gresults-caching/query_processing/expr/SecuredFunctionEval.cpp
        gresults-caching/query_processing/expr/BNodeEval.cpp
        gresults-caching/query_processing/expr/CallEval.cpp
        gresults-caching/query_processing/expr/CoalesceEval.cpp
        gresults-caching/query_processing/expr/FunctionEval.cpp
        gresults-caching/query_processing/expr/OneOfBaseEval.cpp
        gresults-caching/query_processing/expr/RegexEval.cpp
        gresults-caching/query_processing/expr/StrConcatEval.cpp
        gresults-caching/query_processing/expr/StrReplaceEval.cpp
        gresults-caching/query_processing/expr/StrSubstringEval.cpp
        gresults-caching/query_processing/expr/TermResource.cpp
        gresults-caching/query_processing/expr/ConcreteRDFResource.cpp
        gresults-caching/query_processing/expr/DataTypeResource.cpp
        gresults-caching/query_processing/expr/NullResource.cpp
        gresults-caching/query_processing/expr/DayTimeDurationResource.cpp
        gresults-caching/query_processing/expr/StringLiteralResource.cpp
        gresults-caching/query_processing/expr/StringLiteralLangResource.cpp
        gresults-caching/query_processing/expr/IntegerResource.cpp
        gresults-caching/query_processing/expr/FloatResource.cpp
        gresults-caching/query_processing/expr/DoubleResource.cpp
        gresults-caching/query_processing/expr/BooleanResource.cpp
        gresults-caching/query_processing/expr/StringHandlingUtil.cpp
        gresults-caching/query_processing/DateInfo.hpp
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
        proto/generated/graph_result.pb.cc
        proto/generated/k2tree.pb.cc
        proto/generated/message_type.pb.cc
        proto/generated/predicates_index_cache.pb.cc
        proto/generated/request_msg.pb.cc
        proto/generated/response_msg.pb.cc
        proto/generated/sparql_tree.pb.cc
        )

set(PROTO_MESSAGES_PATH ${CMAKE_SOURCE_DIR}/proto/generated)

set(INCLUDES memory core network proc algorithms utils ${PROTO_MESSAGES_PATH} ${CMAKE_SOURCE_DIR}/gresults-caching)

set(K2TREE_PATH ${CMAKE_SOURCE_DIR}/lib/c-k2tree-dyn)

set(LIB_CPPBASE64_PATH lib/cpp-base64)


set(INCLUDES_LIBS
        ${K2TREE_PATH}/include
        ${LIB_CPPBASE64_PATH}
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


include_directories(
        ${INCLUDES}
        ${INCLUDES_LIBS}
        ${LIB_CSD_INCLUDES}
        ${LIB_NTPARSER_INCLUDES}
)
link_directories(${LINK_LIBS})


add_library(MemoryManager STATIC
        memory/MemoryManager.cpp
        )

add_library(RDFCacheK2 STATIC ${CORE_SOURCE_FILES} ${GRAPH_RESULT_CACHING_FILE})
add_library(RDFCacheK2_Network STATIC ${NETWORK_SOURCE_FILES})
#add_library(RDFCacheK2_GR STATIC ${GRAPH_RESULT_CACHING_FILE})

add_library(ProtoMessages STATIC ${PROTO_MESSAGES})


add_library(Proc STATIC ${PROC_SOURCE_FILES})

add_library(CppBase64 ${LIB_CPPBASE64_PATH}/base64.cpp)

add_library(Utils utils/serialization_util.cpp utils/hashing.cpp)

add_dependencies(Proc CppBase64)

# add_dependencies(RDFCacheK2 _k2tree_merged_noalloc)
add_dependencies(RDFCacheK2 k2dyn Utils)
add_dependencies(RDFCacheK2_Network RDFCacheK2)

#add_dependencies(RDFCacheK2_GR RDFCacheK2)


find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})

find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIRS})

find_package(PCRE2 REQUIRED)
include_directories(${PCRE2_INCLUDE_DIRS})

find_package(ICU REQUIRED COMPONENTS i18n)
include_directories(${ICU_INCLUDE_DIRS})


#add_executable(start_cache_server_example example/start_cache_server_example.cpp)

set(BASE_LIBS
        RDFCacheK2
        k2dyn
        MemoryManager
        ProtoMessages
        ${Protobuf_LIBRARIES}
        CSD
        cds
        Utils
        ${OPENSSL_LIBRARIES}
        stdc++fs
        pcrecpp
        ${ICU_LIBRARIES}
        )

set(PROC_LIBS
        Proc
        CSD
        cds
        pthread
        CppBase64
        Utils
        )

set(DEFAULT_LIBS
        RDFCacheK2
        RDFCacheK2_Network
        k2dyn
        MemoryManager
        pthread
        ProtoMessages
        ${Protobuf_LIBRARIES}
        CSD
        cds
        Utils
        ${OPENSSL_LIBRARIES}
        stdc++fs
        pcrecpp
        ${ICU_LIBRARIES}
        )


# target_link_libraries(start_cache_server_example RDFCacheK2_GR ${DEFAULT_LIBS} )

add_executable(validate_sd_from_nt scripts/validate_sd_from_nt.cpp)
target_include_directories(validate_sd_from_nt PUBLIC ${LIB_CPPBASE64_PATH})
target_link_libraries(validate_sd_from_nt ${BASE_LIBS} CppBase64 ntparser)


#add_executable(index_by_k2tree_with_sd scripts/index_by_k2tree_with_sd.cpp)
#target_link_libraries(index_by_k2tree_with_sd ${BASE_LIBS} ntparser )


add_executable(split_nt_triples scripts/split_nt_triples.cpp)
target_include_directories(split_nt_triples PUBLIC ${LIB_CPPBASE64_PATH})
target_link_libraries(split_nt_triples ntparser CppBase64)


add_executable(sd_from_fileset_readfull scripts/sd_from_fileset_readfull.cpp)
target_link_libraries(sd_from_fileset_readfull ${PROC_LIBS})

add_executable(print_sd scripts/print_sd.cpp)
target_include_directories(print_sd PUBLIC ${LIB_CPPBASE64_PATH})
target_link_libraries(print_sd ${PROC_LIBS})


add_executable(base64_transform scripts/base64_transform.cpp)
target_include_directories(base64_transform PUBLIC ${LIB_CPPBASE64_PATH})
target_link_libraries(base64_transform ${PROC_LIBS})

add_executable(external_sort scripts/external_sort.cpp)
target_include_directories(external_sort PUBLIC ${INCLUDES})
target_link_libraries(external_sort pthread Utils stdc++fs)

add_executable(convert_nt_to_binary scripts/convert_nt_to_binary.cpp)
target_include_directories(convert_nt_to_binary PUBLIC ${INCLUDES})
target_link_libraries(convert_nt_to_binary ${PROC_LIBS} ntparser)

add_executable(extract_triples_binary scripts/extract_triples_binary.cpp)
target_include_directories(extract_triples_binary PUBLIC ${INCLUDES})
target_link_libraries(extract_triples_binary ${PROC_LIBS} ntparser)


add_executable(build_k2tree_index scripts/build_k2tree_index.cpp)
target_include_directories(build_k2tree_index PUBLIC ${INCLUDES})
target_link_libraries(build_k2tree_index ${DEFAULT_LIBS})

add_executable(print_k2tree_index scripts/print_k2tree_index.cpp)
target_include_directories(print_k2tree_index PUBLIC ${INCLUDES})
target_link_libraries(print_k2tree_index ${DEFAULT_LIBS})


add_executable(cache_server scripts/cache_server.cpp)
target_include_directories(cache_server PUBLIC ${INCLUDES})
target_link_libraries(cache_server ${DEFAULT_LIBS})

add_executable(debug_read_k2trees_metadata scripts/debug_read_k2trees_metadata.cpp)
target_include_directories(debug_read_k2trees_metadata PUBLIC ${INCLUDES})
target_link_libraries(debug_read_k2trees_metadata ${DEFAULT_LIBS})

add_executable(show_sds_map scripts/show_sds_map.cpp)
target_include_directories(show_sds_map PUBLIC ${INCLUDES})
target_link_libraries(show_sds_map ${DEFAULT_LIBS})

add_executable(validate_pcm scripts/validate_pcm.cpp)
target_include_directories(validate_pcm PUBLIC ${INCLUDES})
target_link_libraries(validate_pcm ${DEFAULT_LIBS})

add_executable(dataset_stats scripts/dataset_stats.cpp)
target_include_directories(dataset_stats PUBLIC ${INCLUDES})
target_link_libraries(dataset_stats ${DEFAULT_LIBS})


add_executable(convert_triples_to_binary scripts/convert_triples_to_binary.cpp)
target_include_directories(convert_triples_to_binary PUBLIC ${INCLUDES})
target_link_libraries(convert_triples_to_binary ${DEFAULT_LIBS})


add_executable(traverse_k2tree_band scripts/traverse_k2tree_band.cpp)
target_include_directories(traverse_k2tree_band PUBLIC ${INCLUDES})
target_link_libraries(traverse_k2tree_band ${DEFAULT_LIBS})


add_executable(four_sided_queue_example example/four_sided_queue_example.cpp)


add_executable(k2tree_example example/k2tree_example.cpp)
target_link_libraries(k2tree_example ${BASE_LIBS})


# Only create test executables if google test is found
find_package(GTest QUIET)
if (GTest_FOUND)
    enable_testing()
    include_directories(${GTEST_INCLUDE_DIRS})
    add_executable(serialization_test test/serialization_tests.cpp)
    target_link_libraries(serialization_test ${DEFAULT_LIBS} ${GTEST_BOTH_LIBRARIES})

    add_executable(k2tree_serialization_test test/k2tree_serialization_test.cpp)
    target_link_libraries(k2tree_serialization_test ${DEFAULT_LIBS} ${GTEST_BOTH_LIBRARIES})

    add_executable(predicates_cache_manager_test test/predicates_cache_manager_test.cpp test/cache_test_util.cpp)
    target_link_libraries(predicates_cache_manager_test ${DEFAULT_LIBS} ${GTEST_BOTH_LIBRARIES})

    add_executable(four_sided_queue_test test/four_sided_queue_test.cpp)
    target_link_libraries(four_sided_queue_test ${GTEST_BOTH_LIBRARIES} pthread)

    add_executable(k2tree_tests test/k2tree_tests.cpp)
    target_link_libraries(k2tree_tests ${DEFAULT_LIBS} ${GTEST_BOTH_LIBRARIES} pthread)

    add_executable(result_table_test test/result_table_test.cpp)
    target_link_libraries(result_table_test ${DEFAULT_LIBS} ${GTEST_BOTH_LIBRARIES} pthread)

    add_executable(k2tree_mixed_test test/k2tree_mixed_test.cpp)
    target_link_libraries(k2tree_mixed_test ${DEFAULT_LIBS} ${GTEST_BOTH_LIBRARIES} pthread)

    add_executable(query_proc_tests test/query_proc_tests.cpp test/cache_test_util.cpp)
    target_link_libraries(query_proc_tests ${DEFAULT_LIBS} ${PROC_LIBS} ${GTEST_BOTH_LIBRARIES} pthread)

    add_executable(test_sd_builder test/test_sd_builder.cpp)
    target_link_libraries(test_sd_builder ${PROC_LIBS} ${GTEST_BOTH_LIBRARIES} pthread)

    add_executable(cache_replacement_test test/cache_replacement_test.cpp)
    target_link_libraries(cache_replacement_test ${DEFAULT_LIBS} ${GTEST_BOTH_LIBRARIES} pthread)

    add_executable(test_external_sort test/test_external_sort.cpp)
    target_link_libraries(test_external_sort ${DEFAULT_LIBS} ${GTEST_BOTH_LIBRARIES} pthread)

    add_executable(predicates_metadata_serialization_test test/predicates_metadata_serialization_test.cpp)
    target_link_libraries(predicates_metadata_serialization_test ${DEFAULT_LIBS} ${GTEST_BOTH_LIBRARIES} pthread)

    add_executable(join_util_test test/join_util_test.cpp)
    target_link_libraries(join_util_test ${DEFAULT_LIBS} ${GTEST_BOTH_LIBRARIES})

    add_executable(k2mixed_from_file test/k2mixed_from_file.cpp)
    target_link_libraries(k2mixed_from_file ${DEFAULT_LIBS} ${GTEST_BOTH_LIBRARIES})


    add_test(NAME serialization_test COMMAND ./serialization_test)
    add_test(NAME k2tree_serialization_test COMMAND ./k2tree_serialization_test)
    add_test(NAME predicates_cache_manager_test COMMAND ./predicates_cache_manager_test)
    add_test(NAME four_sided_queue_test COMMAND ./four_sided_queue_test)
    add_test(NAME k2tree_tests COMMAND ./k2tree_tests)
    add_test(NAME result_table_test COMMAND ./result_table_test)
    add_test(NAME k2tree_mixed_test COMMAND ./k2tree_mixed_test)
    add_test(NAME query_proc_tests COMMAND ./query_proc_tests)
    add_test(NAME test_sd_builder COMMAND ./test_sd_builder)
    add_test(NAME cache_replacement_test COMMAND ./cache_replacement_test)
    add_test(NAME test_external_sort COMMAND ./test_external_sort)
    add_test(NAME predicates_metadata_serialization_test COMMAND ./predicates_metadata_serialization_test)
    add_test(NAME join_util_test COMMAND ./join_util_test)

endif ()


add_executable(k2tree_size_benchmark benchmarks/k2tree_size_benchmark.cpp)
target_link_libraries(k2tree_size_benchmark ${BASE_LIBS})

add_executable(k2tree_mixed_benchmark benchmarks/k2tree_mixed_benchmark.cpp)
target_link_libraries(k2tree_mixed_benchmark ${BASE_LIBS})
