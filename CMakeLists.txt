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
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fstandalone-debug")
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
        core/PredicatesIndexCacheMDFile.cpp
        core/PredicatesCacheManager.cpp
        core/NaiveDynamicStringDictionary.cpp
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
        core/K2TreeBulkOp.hpp)

set(PROC_SOURCE_FILES
        proc/SDFeed.cpp
        proc/SDBuilder.cpp
        )

set(GRAPH_RESULT_CACHING_FILE
        query_engine/Cache.cpp
        query_engine/CacheReplacementFactory.cpp
        query_engine/LRU/LRUQueue.cpp
        query_engine/LRUCacheReplacement.cpp
        query_engine/LRUCacheReplacement.hpp
        query_engine/QueryError.cpp
        query_engine/QueryError.hpp
        query_engine/TimeControl.cpp
        query_engine/TimeControl.hpp
        query_engine/query_processing/BGPOp.hpp
        query_engine/query_processing/BGPOpsFactory.cpp
        query_engine/query_processing/BGPOpsFactory.hpp
        query_engine/query_processing/BGPProcessor.cpp
        query_engine/query_processing/BGPProcessor.hpp
        query_engine/query_processing/DateInfo.cpp
        query_engine/query_processing/DateInfo.hpp
        query_engine/query_processing/DateInfo.hpp
        query_engine/query_processing/ExprProcessor.cpp
        query_engine/query_processing/ExternalSortedFileHandler.cpp
        query_engine/query_processing/ExternalSortedFileHandler.hpp
        query_engine/query_processing/MinusProcessor.cpp
        query_engine/query_processing/MinusProcessor.hpp
        query_engine/query_processing/OneVarCPBGPOp.cpp
        query_engine/query_processing/OneVarCPBGPOp.hpp
        query_engine/query_processing/OneVarIntersectBGPOp.cpp
        query_engine/query_processing/OneVarIntersectBGPOp.hpp
        query_engine/query_processing/OneVarJoinBGPOp.cpp
        query_engine/query_processing/OneVarJoinBGPOp.hpp
        query_engine/query_processing/OptionalProcessor.cpp
        query_engine/query_processing/OptionalProcessor.hpp
        query_engine/query_processing/ParsingUtils.cpp
        query_engine/query_processing/QProc.hpp
        query_engine/query_processing/QueryProcHashing.cpp
        query_engine/query_processing/QueryProcHashing.hpp
        query_engine/query_processing/QueryProcessor.cpp
        query_engine/query_processing/QueryResult.cpp
        query_engine/query_processing/QueryResultIterator.cpp
        query_engine/query_processing/QueryResultIterator.hpp
        query_engine/query_processing/ResultTableFilterIterator.cpp
        query_engine/query_processing/ResultTableFilterIterator.hpp
        query_engine/query_processing/ResultTableIterator.cpp
        query_engine/query_processing/ResultTableIterator.hpp
        query_engine/query_processing/ResultTableIteratorBGP.cpp
        query_engine/query_processing/ResultTableIteratorBGP.hpp
        query_engine/query_processing/ResultTableIteratorCrossProduct.cpp
        query_engine/query_processing/ResultTableIteratorCrossProduct.hpp
        query_engine/query_processing/ResultTableIteratorEmpty.cpp
        query_engine/query_processing/ResultTableIteratorEmpty.hpp
        query_engine/query_processing/ResultTableIteratorExtend.cpp
        query_engine/query_processing/ResultTableIteratorExtend.hpp
        query_engine/query_processing/ResultTableIteratorFromMaterialized.cpp
        query_engine/query_processing/ResultTableIteratorFromMaterialized.hpp
        query_engine/query_processing/ResultTableIteratorJoinSeqBinding.cpp
        query_engine/query_processing/ResultTableIteratorJoinSeqBinding.hpp
        query_engine/query_processing/ResultTableIteratorLeftOuterJoin.cpp
        query_engine/query_processing/ResultTableIteratorLeftOuterJoin.hpp
        query_engine/query_processing/ResultTableIteratorMinus.cpp
        query_engine/query_processing/ResultTableIteratorMinus.hpp
        query_engine/query_processing/ResultTableIteratorOrder.cpp
        query_engine/query_processing/ResultTableIteratorOrder.hpp
        query_engine/query_processing/ResultTableIteratorProject.cpp
        query_engine/query_processing/ResultTableIteratorProject.hpp
        query_engine/query_processing/ResultTableIteratorSlice.cpp
        query_engine/query_processing/ResultTableIteratorSlice.hpp
        query_engine/query_processing/ResultTableIteratorUnion.cpp
        query_engine/query_processing/ResultTableIteratorUnion.hpp
        query_engine/query_processing/ResultTableVector.cpp
        query_engine/query_processing/ResultTableVector.hpp
        query_engine/query_processing/RowSortConnector.cpp
        query_engine/query_processing/RowSortConnector.hpp
        query_engine/query_processing/SequenceProcessor.cpp
        query_engine/query_processing/SequenceProcessor.hpp
        query_engine/query_processing/Triple.cpp
        query_engine/query_processing/TripleExistenceBGPOP.cpp
        query_engine/query_processing/TripleExistenceBGPOP.hpp
        query_engine/query_processing/TwoVarCProductBGPOp.cpp
        query_engine/query_processing/TwoVarCProductBGPOp.hpp
        query_engine/query_processing/TwoVarIntersectionBGPOp.cpp
        query_engine/query_processing/TwoVarIntersectionBGPOp.hpp
        query_engine/query_processing/TwoVarJoinOneBGPOp.cpp
        query_engine/query_processing/TwoVarJoinOneBGPOp.hpp
        query_engine/query_processing/UnionProcessor.cpp
        query_engine/query_processing/VDGNode.cpp
        query_engine/query_processing/VDGNode.hpp
        query_engine/query_processing/VarBindingQProc.cpp
        query_engine/query_processing/VarBindingQProc.hpp
        query_engine/query_processing/VarDependencyGraph.cpp
        query_engine/query_processing/VarDependencyGraph.hpp
        query_engine/query_processing/VarIndexManager.cpp
        query_engine/query_processing/VarLazyBinding.cpp
        query_engine/query_processing/VarLazyBinding.hpp
        query_engine/query_processing/VarsCollection.cpp
        query_engine/query_processing/VarsCollection.hpp
        query_engine/query_processing/expr/AddEval.cpp
        query_engine/query_processing/expr/BNodeEval.cpp
        query_engine/query_processing/expr/BoundEval.cpp
        query_engine/query_processing/expr/BoundVarsMap.cpp
        query_engine/query_processing/expr/CallEval.cpp
        query_engine/query_processing/expr/CastEval.cpp
        query_engine/query_processing/expr/CoalesceEval.cpp
        query_engine/query_processing/expr/ConditionalEval.cpp
        query_engine/query_processing/expr/DataTypeEval.cpp
        query_engine/query_processing/expr/DateTimeDayEval.cpp
        query_engine/query_processing/expr/DateTimeHoursEval.cpp
        query_engine/query_processing/expr/DateTimeMinutesEval.cpp
        query_engine/query_processing/expr/DateTimeMonthEval.cpp
        query_engine/query_processing/expr/DateTimeSecondsEval.cpp
        query_engine/query_processing/expr/DateTimeTZEval.cpp
        query_engine/query_processing/expr/DateTimeYearEval.cpp
        query_engine/query_processing/expr/DigestMD5Eval.cpp
        query_engine/query_processing/expr/DigestSHA1Eval.cpp
        query_engine/query_processing/expr/DigestSHA224Eval.cpp
        query_engine/query_processing/expr/DigestSHA256Eval.cpp
        query_engine/query_processing/expr/DigestSHA384Eval.cpp
        query_engine/query_processing/expr/DigestSHA512Eval.cpp
        query_engine/query_processing/expr/DivideEval.cpp
        query_engine/query_processing/expr/EqualsEval.cpp
        query_engine/query_processing/expr/ExistsEval.cpp
        query_engine/query_processing/expr/ExprEval.cpp
        query_engine/query_processing/expr/FunIRIEval.cpp
        query_engine/query_processing/expr/FunctionEval.cpp
        query_engine/query_processing/expr/GreaterThanEval.cpp
        query_engine/query_processing/expr/GreaterThanOrEqualEval.cpp
        query_engine/query_processing/expr/InEval.cpp
        query_engine/query_processing/expr/IsBlankEval.cpp
        query_engine/query_processing/expr/IsIRIEval.cpp
        query_engine/query_processing/expr/IsLiteralEval.cpp
        query_engine/query_processing/expr/IsNumericEval.cpp
        query_engine/query_processing/expr/LangEval.cpp
        query_engine/query_processing/expr/LangMatchesEval.cpp
        query_engine/query_processing/expr/LessThanEval.cpp
        query_engine/query_processing/expr/LessThanOrEqualEval.cpp
        query_engine/query_processing/expr/LogicalAndEval.cpp
        query_engine/query_processing/expr/LogicalNotEval.cpp
        query_engine/query_processing/expr/LogicalOrEval.cpp
        query_engine/query_processing/expr/MultiplyEval.cpp
        query_engine/query_processing/expr/NotEqualsEval.cpp
        query_engine/query_processing/expr/NotExistsEval.cpp
        query_engine/query_processing/expr/NotInEval.cpp
        query_engine/query_processing/expr/NowEval.cpp
        query_engine/query_processing/expr/NumAbsEval.cpp
        query_engine/query_processing/expr/NumCeilingEval.cpp
        query_engine/query_processing/expr/NumFloorEval.cpp
        query_engine/query_processing/expr/NumRoundEval.cpp
        query_engine/query_processing/expr/OneOfBaseEval.cpp
        query_engine/query_processing/expr/RandEval.cpp
        query_engine/query_processing/expr/RegexEval.cpp
        query_engine/query_processing/expr/SameTermEval.cpp
        query_engine/query_processing/expr/SecuredFunctionEval.cpp
        query_engine/query_processing/expr/StrAfterEval.cpp
        query_engine/query_processing/expr/StrBeforeEval.cpp
        query_engine/query_processing/expr/StrConcatEval.cpp
        query_engine/query_processing/expr/StrContainsEval.cpp
        query_engine/query_processing/expr/StrDataTypeEval.cpp
        query_engine/query_processing/expr/StrEncodeForURIEval.cpp
        query_engine/query_processing/expr/StrEndsWithEval.cpp
        query_engine/query_processing/expr/StrEval.cpp
        query_engine/query_processing/expr/StrLangEval.cpp
        query_engine/query_processing/expr/StrLengthEval.cpp
        query_engine/query_processing/expr/StrLowerCaseEval.cpp
        query_engine/query_processing/expr/StrReplaceEval.cpp
        query_engine/query_processing/expr/StrStartsWithEval.cpp
        query_engine/query_processing/expr/StrSubstringEval.cpp
        query_engine/query_processing/expr/StrUpperCaseCaseEval.cpp
        query_engine/query_processing/expr/SubtractEval.cpp
        query_engine/query_processing/expr/TermEval.cpp
        query_engine/query_processing/expr/UnaryMinusEval.cpp
        query_engine/query_processing/expr/UnaryPlusEval.cpp
        query_engine/query_processing/resources/BooleanResource.cpp
        query_engine/query_processing/resources/ConcreteRDFResource.cpp
        query_engine/query_processing/resources/DataTypeResource.cpp
        query_engine/query_processing/resources/DateTimeResource.cpp
        query_engine/query_processing/resources/DayTimeDurationResource.cpp
        query_engine/query_processing/resources/DoubleResource.cpp
        query_engine/query_processing/resources/FloatResource.cpp
        query_engine/query_processing/resources/IRIResource.cpp
        query_engine/query_processing/resources/IntegerResource.cpp
        query_engine/query_processing/resources/NullResource.cpp
        query_engine/query_processing/resources/StringLiteralLangResource.cpp
        query_engine/query_processing/resources/StringLiteralResource.cpp
        query_engine/query_processing/resources/TermResource.cpp
        query_engine/query_processing/utility/GatheredVars.cpp
        query_engine/query_processing/utility/GatheredVars.hpp
        query_engine/query_processing/utility/ProtoGatherVars.cpp
        query_engine/query_processing/utility/ProtoGatherVars.hpp
        query_engine/query_processing/utility/StringHandlingUtil.cpp
        query_engine/query_processing/utility/UuidGenerator.cpp
        query_engine/query_processing/utility/UuidGenerator.hpp
        )


set(NETWORK_SOURCE_FILES
        network/CacheServer.cpp
        network/TCPServerConnection.cpp
        network/ServerTask.cpp
        network/CacheServerTaskProcessor.cpp
        network/ServerWorker.cpp
        network/Message.cpp
        network/QueryResultPartStreamer.cpp network/QueryResultPartStreamer.hpp network/I_QRStreamer.hpp)


set(PROTO_MESSAGES
        proto/generated/message_type.pb.cc
        proto/generated/request_msg.pb.cc
        proto/generated/response_msg.pb.cc
        proto/generated/sparql_tree.pb.cc
        )

set(PROTO_MESSAGES_PATH ${CMAKE_SOURCE_DIR}/proto/generated)

set(INCLUDES
        core
        network
        proc
        algorithms
        utils
        query_engine
        ${PROTO_MESSAGES_PATH}
        )

set(K2TREE_PATH ${CMAKE_SOURCE_DIR}/lib/c-k2tree-dyn)

set(LIB_CPPBASE64_PATH lib/cpp-base64)
set(LIB_EXTERNAL_SORT_PATH lib/external-sort)

set(INCLUDES_LIBS
        ${K2TREE_PATH}/include
        ${LIB_CPPBASE64_PATH}
        ${LIB_EXTERNAL_SORT_PATH}/include
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

find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIR})


#add_executable(start_cache_server_example example/start_cache_server_example.cpp)

set(BASE_LIBS
        RDFCacheK2
        k2dyn
        ProtoMessages
        ${Protobuf_LIBRARIES}
        CSD
        cds
        Utils
        ${OPENSSL_LIBRARIES}
        stdc++fs
        pcrecpp
        ${CURL_LIBRARIES}
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

        ProtoMessages
        ${Protobuf_LIBRARIES}
        CSD
        cds
        Utils
        ${OPENSSL_LIBRARIES}
        stdc++fs
        pcrecpp
        ${CURL_LIBRARIES}
        pthread
        )


# target_link_libraries(start_cache_server_example RDFCacheK2_GR ${DEFAULT_LIBS} )

add_executable(validate_sd_from_nt scripts/validate_sd_from_nt.cpp)
target_include_directories(validate_sd_from_nt PUBLIC ${LIB_CPPBASE64_PATH})
target_link_libraries(validate_sd_from_nt ${BASE_LIBS} CppBase64 ntparser pthread)


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

add_executable(external_sort scripts/triple_external_sort.cpp)
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




add_executable(k2tree_example example/k2tree_example.cpp)
target_link_libraries(k2tree_example ${BASE_LIBS})


add_executable(count_nt_predicates scripts/count_nt_predicates.cpp)
target_link_libraries(count_nt_predicates ${PROC_LIBS} ntparser)


# Only create test executables if google test is found
find_package(GTest QUIET)
if (GTest_FOUND)
    enable_testing()
    include_directories(${GTEST_INCLUDE_DIRS})
    add_executable(serialization_test test/serialization_tests.cpp)
    target_link_libraries(serialization_test ${DEFAULT_LIBS} ${GTEST_BOTH_LIBRARIES} pthread)

    add_executable(k2tree_serialization_test test/k2tree_serialization_test.cpp)
    target_link_libraries(k2tree_serialization_test ${DEFAULT_LIBS} ${GTEST_BOTH_LIBRARIES} pthread)

    add_executable(predicates_cache_manager_test test/predicates_cache_manager_test.cpp test/cache_test_util.cpp)
    target_link_libraries(predicates_cache_manager_test ${DEFAULT_LIBS} ${GTEST_BOTH_LIBRARIES} pthread)

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

    add_executable(k2mixed_from_file test/k2mixed_from_file.cpp)
    target_link_libraries(k2mixed_from_file ${DEFAULT_LIBS} ${GTEST_BOTH_LIBRARIES})

    add_executable(query_filters_test test/query_filters_test.cpp test/cache_test_util.cpp)
    target_link_libraries(query_filters_test ${DEFAULT_LIBS} ${GTEST_BOTH_LIBRARIES})

    add_executable(query_proc_tests2 test/query_proc_tests2.cpp test/cache_test_util.cpp)
    target_link_libraries(query_proc_tests2 ${DEFAULT_LIBS} ${GTEST_BOTH_LIBRARIES})


    add_test(NAME serialization_test COMMAND ./serialization_test)
    add_test(NAME k2tree_serialization_test COMMAND ./k2tree_serialization_test)
    add_test(NAME predicates_cache_manager_test COMMAND ./predicates_cache_manager_test)
    add_test(NAME k2tree_tests COMMAND ./k2tree_tests)
    add_test(NAME result_table_test COMMAND ./result_table_test)
    add_test(NAME k2tree_mixed_test COMMAND ./k2tree_mixed_test)
    add_test(NAME query_proc_tests COMMAND ./query_proc_tests)
    add_test(NAME test_sd_builder COMMAND ./test_sd_builder)
    add_test(NAME cache_replacement_test COMMAND ./cache_replacement_test)
    add_test(NAME test_external_sort COMMAND ./test_external_sort)
    add_test(NAME predicates_metadata_serialization_test COMMAND ./predicates_metadata_serialization_test)
    add_test(NAME query_filters_test COMMAND ./query_filters_test)
    add_test(NAME query_proc_tests2 COMMAND ./query_proc_tests2)

endif ()


add_executable(k2tree_size_benchmark benchmarks/k2tree_size_benchmark.cpp)
target_link_libraries(k2tree_size_benchmark ${BASE_LIBS})

add_executable(k2tree_mixed_benchmark benchmarks/k2tree_mixed_benchmark.cpp)
target_link_libraries(k2tree_mixed_benchmark ${BASE_LIBS})
