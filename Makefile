CC=clang++ -fcolor-diagnostics
CFLAGS=-Wall -Wextra -O2 -std=c++17 $(shell pkg-config --cflags cpr) -Iinclude
LDFLAGS=$(shell pkg-config --libs cpr)

SRC = src/utils.cpp
TESTS = tests/test_utils.cpp

.PHONY: all test clean

all: libutils.a

libutils.a: $(SRC)
	$(CC) $(CFLAGS) -c $(SRC) -o src/utils.o
	ar rcs libutils.a src/utils.o

get_token: src/get_token.cpp src/gtoken.cpp libutils.a
	$(CC) $(CFLAGS) src/get_token.cpp src/gtoken.cpp libutils.a $(LDFLAGS) -o get_token

test: $(SRC) $(TESTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(SRC) $(TESTS) -o test_utils
	./test_utils

clean:
	rm -f src/*.o test_utils libutils.a get_token
