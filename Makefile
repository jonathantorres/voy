VPATH = src tests bin
CFLAGS = gcc -g -std=c11 -Wall -Wextra -Isrc
SOURCES = $(wildcard src/*.c)

all: voy test

voy: voy.c
	$(CFLAGS) src/voy.c -o bin/voy

# Tests
voy_dl_list_test: voy_dl_list_test.c voy_dl_list.o
	$(CFLAGS) tests/voy_dl_list_test.c voy_dl_list.o -o bin/voy_dl_list_test
voy_array_test: voy_array_test.c voy_array.o
	$(CFLAGS) tests/voy_array_test.c voy_array.o -o bin/voy_array_test
voy_htable_test: voy_htable_test.c voy_htable.o voy_array.o
	$(CFLAGS) tests/voy_htable_test.c voy_htable.o voy_array.o -o bin/voy_htable_test
voy_str_test: voy_str_test.c voy_str.o
	$(CFLAGS) tests/voy_str_test.c voy_str.o -o bin/voy_str_test

# Objects
voy_dl_list.o: voy_dl_list.c voy_dl_list.h
	$(CFLAGS) -c src/voy_dl_list.c src/voy_dl_list.h
voy_array.o: voy_array.c voy_array.h
	$(CFLAGS) -c src/voy_array.c src/voy_array.h
voy_htable.o: voy_htable.c voy_htable.h
	$(CFLAGS) -c src/voy_htable.c src/voy_htable.h
voy_str.o: voy_str.c voy_str.h
	$(CFLAGS) -c src/voy_str.c src/voy_str.h

# Run tests
.PHONY: test
test: voy_dl_list_test voy_array_test voy_htable_test voy_str_test
	./bin/voy_dl_list_test
	./bin/voy_array_test
	./bin/voy_htable_test
	./bin/voy_str_test

clean:
	rm -f ./*.o src/*.h.gch
	rm -fr ./bin
	mkdir ./bin && touch ./bin/.gitkeep
