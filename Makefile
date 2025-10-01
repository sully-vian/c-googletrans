CC=clang++ -fcolor-diagnostics
CFLAGS=-Wall -Wextra -O2 -std=c++17 $(shell pkg-config --cflags cpr) -Iinclude
LDFLAGS=$(shell pkg-config --libs cpr libssl libcrypto)

SRC = src/main.cpp src/client.cpp src/gtoken.cpp src/utils.cpp
TESTS = tests/test_utils.cpp

.PHONY: all test clean

all: main

main: $(SRC)
	$(CC) $(CFLAGS) $(SRC) $(LDFLAGS) -o main

get_token: src/get_token.cpp src/gtoken.cpp libutils.a
	$(CC) $(CFLAGS) src/get_token.cpp src/gtoken.cpp libutils.a $(LDFLAGS) -o get_token

test: $(SRC) $(TESTS)
	$(CC) $(CFLAGS) $(SRC) $(TESTS) $(LDFLAGS) -o test_utils
	./test_utils

clean:
	rm -f src/*.o test_utils client
