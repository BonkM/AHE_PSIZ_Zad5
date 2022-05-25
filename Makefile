
.PHONY: all
all: dist/bitmapinfo

dist/bitmapinfo: bitmapinfo.cpp
	mkdir -p dist
	g++ --std=c++17 -o dist/bitmapinfo.exe bitmapinfo.cpp
	mv dist/bitmapinfo.exe dist/bitmapinfo

.PHONY: clean
clean:
	rm -rf dist
