
# Deprecated, prefer installing these with OS package manager

set(RAPTOR_PATH lib/raptor2-2.0.15)
#set(LIBXML_PATH lib/libxml2-2.9.10)

#####


# Find LibXml2 or manually compile it
find_package(LibXml2 2.9.10 QUIET)

if(false)
if(LibXml2_FOUND)
    set(_lxml2_lib ${LIBXML2_LIBRARIES})
    set(_lxml2_inc ${LIBXML2_INCLUDE_DIR})
else()
    # Build
    set(_lxml2_ver libxml2-2.9.10)
    set(_lxml2_targz ${_lxml2_ver}.tar.gz)
    set(_lxml2_download_url ftp://xmlsoft.org/libxml2/${_lxml2_targz})
    set(_lxml2_path ${CMAKE_CURRENT_SOURCE_DIR}/lib)
    set(_lxml2_path_in ${CMAKE_CURRENT_SOURCE_DIR}/lib/${_lxml2_ver})
    set(_lxml2_libfile libxml2.a)
    #set(_lxml2_lib libxml2.a)
    # set(_lxml2_target libxml2_target)

    if(EXISTS ${_lxml2_path}/${_lxml2_ver})
    else()
        if(EXISTS ${_lxml2_path}/${_lxml2_targz})
        else()
            file(DOWNLOAD ${_lxml2_download_url} ${_lxml2_path}/${_lxml2_targz}
                    TIMEOUT 300 # 5 mins
                    SHOW_PROGRESS
                    )
        endif()
        execute_process(COMMAND ${CMAKE_COMMAND} -E tar -zxvf ${_lxml2_targz} WORKING_DIRECTORY ${_lxml2_path})
        #file(REMOVE  ${_lxml2_path}/${_lxml2_targz})
    endif()
    add_custom_command(OUTPUT ${_lxml2_libfile}
            COMMAND ./configure
            COMMAND make
            WORKING_DIRECTORY ${_lxml2_path}/${_lxml2_ver}
            )
    add_custom_target(_lxml2_target ALL DEPENDS ${_lxml2_libfile})
    add_library(__lxml2_lib STATIC IMPORTED GLOBAL)
    add_dependencies(__lxml2_lib _lxml2_target)
    set_target_properties(__lxml2_lib
            PROPERTIES
            IMPORTED_LOCATION ${_lxml2_path_in}/${_lxml2_libfile}
            INTERFACE_INCLUDE_DIRECTORIES ${_lxml2_path_in}/include)
    set(xml2 __lxml2_lib)
    set(_lxml2_inc ${_lxml2_path_in}/include)
endif()

find_package(Raptor2 2.0.15)

if(Raptor2_FOUND)
    set(_raptor2_lib ${RAPTOR2_LIBRARIES})
    set(_raptor2_inc ${RAPTOR2_INCLUDE_DIR})
else()
    set(_raptor2_ver raptor2-2.0.15)
    set(_raptor2_targz ${_raptor2_ver}.tar.gz)
    set(_raptor2_download_url http://download.librdf.org/source/${_raptor2_targz})
    set(_raptor2_path ${CMAKE_CURRENT_SOURCE_DIR}/lib)
    set(_raptor2_path_in ${CMAKE_CURRENT_SOURCE_DIR}/lib/${_raptor2_ver})
    set(_raptor2_libfile libraptor2.a)


    message("RAPTOR DOWNLOAD URL:" ${_raptor2_download_url})
    message("RAPTOR PATH:" ${_raptor2_path})
    message("RAPTOR TAR GZ:" ${_raptor2_targz})

    if(EXISTS ${_raptor2_path_in})
    else()
        if(EXISTS ${_raptor2_path}/${_raptor2_targz})
        else()
            file(DOWNLOAD ${_raptor2_download_url} ${_raptor2_path}/${_raptor2_targz}
                    TIMEOUT 300 # 5 mins
                    SHOW_PROGRESS
                    )
        endif()
        execute_process(COMMAND ${CMAKE_COMMAND} -E tar -zxvf ${_raptor2_targz} WORKING_DIRECTORY ${_raptor2_path})
        #file(REMOVE  ${_raptor2_path}/${_raptor2_targz})
    endif()
    add_custom_command(OUTPUT ${_raptor2_libfile}
            COMMAND ./configure --enable-parsers="ntriples turtle" --enable-serializers=ntriples --enable-static --disable-shared
            COMMAND make
            WORKING_DIRECTORY ${_raptor2_path}/${_raptor2_ver}
            )
    add_custom_target(_raptor2_target ALL DEPENDS ${_raptor2_libfile})
    add_library(__raptor2_lib STATIC IMPORTED GLOBAL)
    add_dependencies(__raptor2_lib _raptor2_target)
    set_target_properties(__raptor2_lib
            PROPERTIES
            IMPORTED_LOCATION ${_raptor2_path_in}/src/.libs/${_raptor2_libfile}
            INTERFACE_INCLUDE_DIRECTORIES ${_raptor2_path_in}/src)
    set(raptor2 __raptor2_lib)
    set(_raptor2_inc ${_raptor2_path_in}/src)

    if(DEFINED _lxml2_lib)
        add_dependencies(_raptor2_target _lxml2_target)
    endif()
endif()

message("RAPTOR INCLUDES: " ${_raptor2_inc})

#link_directories(${RAPTOR_PATH}/src/.libs/ ${LIBXML_PATH}/.libs/)
if(DEFINED _raptor2_lib)
    message("raptor lib was defined")
    link_directories(${_raptor2_lib} )
endif()

if(DEFINED _lxml2_lib)
    message("xml2 lib was defined")
    link_directories(${_lxml2_lib})
endif()
#include_directories(${RAPTOR_PATH}/src ${LIBXML_PATH}/include ${RAX_PATH} scripts)
include_directories(${_raptor2_inc} ${_lxml2_inc} ${RAX_PATH} scripts)

set(THIRD_PARTY_DEPS)
if(TARGET _lxml2_target)
    list(APPEND THIRD_PARTY_DEPS _lxml2_target)
endif()

if(TARGET _raptor2_target)
    list(APPEND THIRD_PARTY_DEPS _raptor2_target)
endif()

message("THIRD_PARTY_DEPS: " ${THIRD_PARTY_DEPS})
list(LENGTH THIRD_PARTY_DEPS THIRD_PARTY_DEPS_SZ)
endif()

set(RAX_PATH plib/rax-31-may-2020-1927550)
add_library(Rax ${RAX_PATH}/rax.c )

