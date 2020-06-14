FROM alpine:3.12.0

ENV CC clang
ENV CXX clang++

WORKDIR RDFCacheK2

COPY _bundled_code.tar.gz .
COPY runner.sh .


RUN apk add git libprotobuf protoc protobuf-dev make cmake clang bash libc-dev build-base
RUN tar -zxvf _bundled_code.tar.gz && \
cd _bundled_code && \
make re

RUN chmod a+x /RDFCacheK2/runner.sh


ENTRYPOINT ["/RDFCacheK2/runner.sh"]
CMD []
