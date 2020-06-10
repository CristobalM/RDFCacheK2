
mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
current_dir := $(notdir $(patsubst %/,%,$(dir $(mkfile_path))))

all: format proto-build build


re: clean-all format proto-build build-libs build


build-libs:
	cd lib/c-k2tree-dyn && ./fetch_deps.sh
	./fetch_deps.sh


clean-libs:
	cd lib/c-k2tree-dyn && make clean
	cd lib/libxml2-2.9.10 && make clean

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

clean-all: clean clean-libs proto-clean



format:
	find . -path ./lib -prune -o -regex '.*\.\(cpp\|hpp\)' -exec clang-format -style=file -i {} \;
