
mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
current_dir := $(notdir $(patsubst %/,%,$(dir $(mkfile_path))))
BUNDLE_DIR = _bundled_code

XML2LIB=libxml2-2.9.10
RAPTOR2LIB=raptor2-2.0.15

all: format proto-build build-libs build


re: clean-all all


build-libs:
	cd lib/c-k2tree-dyn && make
	./fetch_deps.sh


clean-libs:
	cd lib/c-k2tree-dyn && make clean-all
	rm -rf lib/c-k2tree-dyn/*.a
	#cd lib/libxml2-2.9.10 && make clean
	rm -rf lib/${XML2LIB}
	#cd lib/libCSD && make clean-all
	rm -rf lib/libCSD
	#cd lib/raptor2-2.0.15 && make clean
	rm -rf lib/${RAPTOR2LIB}

rebuild-libs: clean-libs build-libs


build:
	mkdir -p build
	cd build && \
	cmake .. && \
	make


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

bundle-code: clean-all
	rm -rf ${BUNDLE_DIR} ${BUNDLE_DIR}.tar.gz
	mkdir -p ${BUNDLE_DIR}
	rsync -rv \
	--exclude='ignore_stuff' \
	--exclude='lib/libCSD' \
	--exclude='lib/${RAPTOR2LIB}' \
	--exclude='lib/${XML2LIB}' \
	--exclude=${BUNDLE_DIR} \
	--exclude='build' \
	--exclude='*.tar.gz' \
	--exclude='*.bin.map' \
	--exclude='*.bin' \
	--exclude='*.o'  \
	--exclude='*.a' \
	--exclude='*.so' \
	--exclude='cmake-build-debug' \
	--exclude='*.git' \
	--exclude '*.vscode' \
	--exclude '*.idea' \
	--exclude 'build-release' \
	. ${BUNDLE_DIR}
	tar -zcvf ${BUNDLE_DIR}.tar.gz ${BUNDLE_DIR}/
	cp ${BUNDLE_DIR}.tar.gz docker
	tar -zcvf RDFCacheK2-docker.tar.gz docker




format:
	find . -path ./lib -prune -o -regex '.*\.\(cpp\|hpp\)' -exec clang-format -style=file -i {} \;
