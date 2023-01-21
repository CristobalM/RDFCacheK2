FROM cmirandat/base-debian-rdfcachek2:v2

ENV CC clang-11
ENV CXX clang++-11

ARG debug="FALSE"
ARG CACHE_CORES_BUILD="1"
ARG DISABLE_TEST="FALSE"
ENV CACHE_CORES_BUILD=$CACHE_CORES_BUILD
ENV DISABLE_TEST=$DISABLE_TEST


WORKDIR RDFCacheK2

COPY _bundled_code.tar.gz .
COPY runner.sh .

RUN echo "Debug = $debug"

RUN echo CACHE_CORES_BUILD=$CACHE_CORES_BUILD

RUN if [ "$debug" = "FALSE" ]; then \
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
;fi; \
if [ "$DISABLE_TEST" = "FALSE" ]; then \
cd build && ctest -j$CACHE_CORES_BUILD . \
;fi;

RUN chmod a+x /RDFCacheK2/runner.sh

ENTRYPOINT ["/RDFCacheK2/runner.sh"]
CMD []
