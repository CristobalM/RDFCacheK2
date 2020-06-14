FROM alpine:3.12.0

WORKDIR RDFCacheK2

COPY _bundled_code.tar.gz .
COPY runner.sh .

RUN apk add git protobuf make cmake clang && \
tar -zxvf _bundled_code.tar.gz && \
cd _bundled_code && \
make re

ENTRYPOINT ["/RDFCacheK2/runner.sh"]
CMD []
