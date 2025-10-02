CC=clang++ -fcolor-diagnostics
CFLAGS=-Wall -Wextra -O2 -std=c++17 $(shell pkg-config --cflags cpr) -Iinclude
LDFLAGS=$(shell pkg-config --libs cpr libssl libcrypto)

SRC = src/main.cpp src/client.cpp src/gtoken.cpp src/utils.cpp
TESTS = tests/test_utils.cpp

.PHONY: all test clean

all: main

main: $(SRC)
	$(CC) $(CFLAGS) $(SRC) $(LDFLAGS) -o main

test: $(SRC) $(TESTS)
	$(CC) $(CFLAGS) $(SRC) $(TESTS) $(LDFLAGS) -o test_utils
	./test_utils

clean:
	rm -f src/*.o test_utils main
