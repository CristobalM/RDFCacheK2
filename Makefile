
mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
current_dir := $(notdir $(patsubst %/,%,$(dir $(mkfile_path))))

bt := ${CACHE_CORES_BUILD}
BUNDLE_DIR = _bundled_code

ifndef bt
	bt:=$(shell nproc)
endif


all: proto-build build-libs build

all-debug: proto-build build-libs-debug build-debug

re: clean-all all

re-debug: clean-all all-debug

build-libs:
	./fetch_deps.sh
	cd lib/ntparser/gen && make -j${bt}


clean-libs:
	rm -rf lib/libCSD

rebuild-libs: clean-libs build-libs


build:
	mkdir -p build
	cd build && \
	cmake .. && \
	make -j${bt}

build-libs-debug:
	./fetch_deps.sh
	cd lib/ntparser/gen && make


build-debug:
	mkdir -p build
	cd build && \
	cmake -DCMAKE_BUILD_TYPE=Debug .. && \
	make -j${bt}


proto-build:
	mkdir -p ./proto/generated && \
	mkdir -p ./proto/java-generated && \
	protoc -I=proto --cpp_out=./proto/generated --java_out=./proto/java-generated \
		./proto/message_type.proto \
		./proto/request_msg.proto \
		./proto/response_msg.proto \
		./proto/sparql_tree.proto

proto-clean:
	rm -rf ./proto/generated
	rm -rf ./proto/java-generated


clean:
	rm -rf build
	rm -rf

clean-all: clean clean-libs proto-clean clean-bundle
	echo "bt=${bt}"


clean-bundle:
	rm -rf ${BUNDLE_DIR} ${BUNDLE_DIR}.tar.gz
	rm -rf docker/${BUNDLE_DIR}.tar.gz

bundle-code: # clean-all
	rm -rf ${BUNDLE_DIR} ${BUNDLE_DIR}.tar.gz
	mkdir -p ${BUNDLE_DIR}
	rsync -rv \
	--delete \
	--delete-excluded \
	--include '/' \
	--include '/bash/*.sh' \
	--include 'cmake/***' \
	--include 'core/***' \
	--include 'docker/***' \
	--include 'example/***' \
	--include 'cache/***' \
	--include 'memory/***' \
	--include 'query_engine/***' \
	--include 'network/***' \
	--include 'proc/***' \
	--include 'sort/***' \
	--include 'utils/***' \
	--include 'algorithms/***' \
	--include 'benchmarks/***' \
	--include 'experiments/***' \
	--include 'proto/' \
	--include 'proto/*.proto' \
	--include 'scripts/***' \
	--include 'test/***' \
	--include '.gitignore' \
	--include '.gitmodules' \
	--include 'CMakeLists.txt' \
	--include 'clang-format.txt' \
	--include 'Makefile' 	\
	--include 'CTestCustom.cmake' \
	--include '*.sh' \
	--include 'lib/' \
	--include 'lib/c-k2tree-dyn/' \
	--include 'lib/c-k2tree-dyn/example/***' \
	--include 'lib/c-k2tree-dyn/include/***' \
	--include 'lib/c-k2tree-dyn/src/***' \
	--include 'lib/c-k2tree-dyn/test/***' \
	--include 'lib/c-k2tree-dyn/.gitignore' \
	--include 'lib/c-k2tree-dyn/*.txt' \
	--include 'lib/c-k2tree-dyn/*.sh' \
	--include 'lib/c-k2tree-dyn/Makefile' \
	--include 'lib/c-k2tree-dyn/*.md' \
	--exclude '*' \
	--exclude 'proto/java-generated/' \
	--exclude 'proto/generated/' \
	. ${BUNDLE_DIR}
	tar -zcvf ${BUNDLE_DIR}.tar.gz ${BUNDLE_DIR}/
	cp ${BUNDLE_DIR}.tar.gz docker
	tar -zcvf RDFCacheK2-docker.tar.gz docker
	rm -rf _bundled_code.tar.gz _bundled_code/ docker/_bundled_code.tar.gz

format:
	find . -path ./lib -prune -o -regex '.*\.\(cpp\|hpp\)' -exec clang-format -style=file -i {} \;
