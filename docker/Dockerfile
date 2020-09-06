FROM alpine:3.12.0

ENV CC clang
ENV CXX clang++

ARG debug="FALSE"

WORKDIR RDFCacheK2

COPY _bundled_code.tar.gz .
COPY runner.sh .

RUN echo "Debug = $debug"
RUN apk add git libprotobuf protoc protobuf-dev make cmake clang bash libc-dev build-base libbsd-dev flex bison
RUN if [ $debug = "FALSE" ]; then \
echo "ON NORMAL BUILD" && \
tar -zxvf _bundled_code.tar.gz && \
cd _bundled_code && \
make re-debug \
;else \
echo "ON DEBUG BUILD" && \
apk add gdb && \
tar -zxvf _bundled_code.tar.gz && \
cd _bundled_code && \
make re-debug \
;fi

RUN chmod a+x /RDFCacheK2/runner.sh

ENTRYPOINT ["/RDFCacheK2/runner.sh"]
CMD []
