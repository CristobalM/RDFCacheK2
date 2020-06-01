#!/bin/bash


exit_on_error() {
    exit_code=$1
    last_command=${@:2}
    if [ $exit_code -ne 0 ]; then
        >&2 echo "\"${last_command}\" command failed with exit code ${exit_code}."
        exit $exit_code
    fi
}

# enable !! command completion
set -o history -o histexpand

mkdir -p lib
cd lib || (echo "cant enter lib" && exit)


LIBXML2=libxml2-2.9.10
LIBXML2_TAR_GZ=${LIBXML2}.tar.gz
if [[ ! -d  ${LIBXML2} ]]; then
    wget ftp://xmlsoft.org/libxml2/${LIBXML2_TAR_GZ}
    tar -zxvf ${LIBXML2_TAR_GZ} && rm ${LIBXML2_TAR_GZ}
    cd ${LIBXML2}  || (echo "cant enter ${LIBXML2} folder" && exit)
    ./configure
    make
    cd ..
fi


RAPTOR=raptor2-2.0.15
RAPTOR_TAR_GZ=${RAPTOR}.tar.gz
if [[ ! -d ${RAPTOR} ]]; then
    wget http://download.librdf.org/source/${RAPTOR_TAR_GZ}
    tar -zxvf ${RAPTOR_TAR_GZ} && rm ${RAPTOR_TAR_GZ}
    cd ${RAPTOR} || (echo "${RAPTOR} folder was not created" && exit)
    ./configure --enable-parsers="ntriples turtle" --enable-serializers=ntriples --enable-static --disable-shared
    make
    cd ..
fi
