#!/usr/bin/make -f

build_dir = build
#extra = -fno-stack-protector
extra = 

main: build_dir main.o tsq.o
	gcc -pthread $(build_dir)/main.o $(build_dir)/tsq.o -o $(build_dir)/main $(extra)

main.o: build_dir main.c
	gcc -c main.c -o $(build_dir)/main.o $(extra)

tsq.o: build_dir tsq/tsq.c
	gcc -c tsq/tsq.c -o $(build_dir)/tsq.o $(extra)
 
build_dir:
	mkdir -p $(build_dir)

clean:
	rm -r $(build_dir)