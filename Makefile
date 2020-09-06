
mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
current_dir := $(notdir $(patsubst %/,%,$(dir $(mkfile_path))))
BUNDLE_DIR = _bundled_code

all: format proto-build build-libs build

all-debug: format proto-build build-libs-debug build-debug

re: clean-all all

re-debug: clean-all all-debug

build-libs:
	cd lib/c-k2tree-dyn && make
	./fetch_deps.sh
	cd lib/ntparser/gen && make


clean-libs:
	cd lib/c-k2tree-dyn && make clean-all
	rm -rf lib/c-k2tree-dyn/*.a
	rm -rf lib/libCSD

rebuild-libs: clean-libs build-libs


build:
	mkdir -p build
	cd build && \
	cmake .. && \
	make -j4

build-libs-debug:
	cd lib/c-k2tree-dyn && make debug-build
	./fetch_deps.sh
	cd lib/ntparser/gen && make


build-debug:
	mkdir -p build
	cd build && \
	cmake -DCMAKE_BUILD_TYPE=Debug .. && \
	make -j4


proto-build:
	mkdir -p ./proto/generated && \
	mkdir -p ./proto/java-generated && \
	protoc -I=proto --cpp_out=./proto/generated --java_out=./proto/java-generated \
		./proto/message_type.proto \
		./proto/k2tree.proto \
		./proto/graph_result.proto \
		./proto/request_msg.proto \
		./proto/response_msg.proto \
		./proto/predicates_index_cache.proto \
		./proto/entities_mapping.proto \
		./proto/radix_tree.proto

proto-clean:
	rm -rf ./proto/generated
	rm -rf ./proto/java-generated


clean:
	rm -rf build
	rm -rf

clean-all: clean clean-libs proto-clean clean-bundle


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
	--include 'gresults-caching/***' \
	--include 'memory/***' \
	--include 'network/***' \
	--include 'plib/***' \
	--include 'proc/***' \
	--include 'proto/' \
	--include 'proto/*.proto' \
	--include 'scripts/***' \
	--include 'test/***' \
	--include '.gitignore' \
	--include '.gitmodules' \
	--include 'CMakeLists.txt' \
	--include 'clang-format.txt' \
	--include 'Makefile' \
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




format:
	find . -path ./lib -prune -o -regex '.*\.\(cpp\|hpp\)' -exec clang-format -style=file -i {} \;
