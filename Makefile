

all: format build


re: clean format build


build:
	mkdir -p build
	cd build && \
	cmake .. && \
	make


clean:
	rm -rf build


format:
	find . -path ./lib -prune -o -regex '.*\.\(cpp\|hpp\)' -exec clang-format -style=file -i {} \;
