FROM debian:buster-slim

ENV CC clang
ENV CXX clang++

ARG debug="FALSE"

WORKDIR RDFCacheK2

COPY _bundled_code.tar.gz .
COPY runner.sh .

RUN echo "Debug = $debug"
RUN apt-get update && apt-get install -y \
 git libprotobuf-dev libprotoc-dev protobuf-compiler \
 make clang clang-format bash linux-libc-dev libbsd-dev \
 flex bison wget tar rsync
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
