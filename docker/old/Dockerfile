FROM debian:buster-slim

ENV CC clang-11
ENV CXX clang++-11

ARG debug="FALSE"

WORKDIR RDFCacheK2

COPY _bundled_code.tar.gz .
COPY runner.sh .

RUN echo "Debug = $debug"
RUN apt-get update && apt-get install -y ca-certificates wget gnupg2 bash lsb-release software-properties-common
RUN echo "deb http://apt.llvm.org/buster/ llvm-toolchain-buster main" | tee -a /etc/apt/sources.list && \
echo "deb-src http://apt.llvm.org/buster/ llvm-toolchain-buster main" | tee -a /etc/apt/sources.list && \
echo "deb http://apt.llvm.org/buster/ llvm-toolchain-buster-11 main" | tee -a /etc/apt/sources.list && \
echo "deb-src http://apt.llvm.org/buster/ llvm-toolchain-buster-11 main" | tee -a /etc/apt/sources.list && \
wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key |  apt-key add -
RUN apt-get update && \
apt-get install -y \
 git libprotobuf-dev libprotoc-dev protobuf-compiler \
 make clang-11 clang-format bash linux-libc-dev libbsd-dev \
 flex bison wget tar rsync libssl-dev libicu-dev libcurl4-openssl-dev libpcre3-dev
# RUN ln -s /usr/bin/clang++-11 /usr/bin/clang++ && ln -s /usr/bin/clang-11 /usr/bin/clang
#RUN PCRE2_PRE=https://ftp.pcre.org/pub/pcre/ &&\
#    PCRE2_NAME=pcre2-10.36 && \
#    wget ${PCRE2_PRE}${PCRE2_NAME}.tar.gz &&\
#    tar -zxvf ${PCRE2_NAME}.tar.gz &&\
#    cd ${PCRE2_NAME} && \
#    ./configure && \
#    make -j && \
#    make install

RUN CMAKE_PRE=https://github.com/Kitware/CMake/releases/download/v3.18.2/ &&\
 CMAKE_NAME=cmake-3.18.2-Linux-x86_64 && \
 wget ${CMAKE_PRE}${CMAKE_NAME}.tar.gz && \
 tar -zxvf ${CMAKE_NAME}.tar.gz && \
 rsync -rv ${CMAKE_NAME}/ /usr/



RUN if [ $debug = "FALSE" ]; then \
echo "ON NORMAL BUILD" && \
tar -zxvf _bundled_code.tar.gz && \
cd _bundled_code && \
make re \
;else \
echo "ON DEBUG BUILD" && \
apt-get install -y gdb vim && \
tar -zxvf _bundled_code.tar.gz && \
cd _bundled_code && \
make re-debug \
;fi

RUN chmod a+x /RDFCacheK2/runner.sh

ENTRYPOINT ["/RDFCacheK2/runner.sh"]
CMD []
