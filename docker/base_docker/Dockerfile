FROM debian:bullseye-slim

RUN apt-get update && apt-get install -y ca-certificates wget gnupg2 bash lsb-release software-properties-common
RUN echo "deb http://apt.llvm.org/bullseye/ llvm-toolchain-bullseye main" | tee -a /etc/apt/sources.list && \
    echo "deb-src http://apt.llvm.org/bullseye/ llvm-toolchain-bullseye main" | tee -a /etc/apt/sources.list && \
    echo "deb http://apt.llvm.org/bullseye/ llvm-toolchain-bullseye-12 main" | tee -a /etc/apt/sources.list && \
    echo "deb-src http://apt.llvm.org/bullseye/ llvm-toolchain-bullseye-12 main" | tee -a /etc/apt/sources.list && \
    wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key |  apt-key add -
RUN apt-get update && \
    apt-get install -y \
    git \
    make clang-11 clang-format bash linux-libc-dev libbsd-dev \
    flex bison wget tar rsync libssl-dev libicu-dev libcurl4-openssl-dev libpcre3-dev \
    googletest libgtest-dev bc
#libprotobuf-dev libprotoc-dev protobuf-compiler \

RUN apt-get install -y build-essential

RUN wget https://github.com/protocolbuffers/protobuf/releases/download/v3.19.4/protobuf-all-3.19.4.tar.gz && \
    tar -zxvf protobuf-all-3.19.4.tar.gz && \
    cd protobuf-3.19.4 && \
    ./configure && \
    make -j14 && \
    make check && \
    make install && \
    ldconfig

RUN CMAKE_PRE=https://github.com/Kitware/CMake/releases/download/v3.18.2/ &&\
    CMAKE_NAME=cmake-3.18.2-Linux-x86_64 && \
    wget ${CMAKE_PRE}${CMAKE_NAME}.tar.gz && \
    tar -zxvf ${CMAKE_NAME}.tar.gz && \
    rsync -rv ${CMAKE_NAME}/ /usr/
