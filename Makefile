
mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
current_dir := $(notdir $(patsubst %/,%,$(dir $(mkfile_path))))

all: format proto-build build


re: clean format build


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
		./proto/predicates_index_cache.proto



clean:
	rm -rf build


format:
	find . -path ./lib -prune -o -regex '.*\.\(cpp\|hpp\)' -exec clang-format -style=file -i {} \;
