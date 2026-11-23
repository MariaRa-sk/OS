CC = gcc
CFLAGS = -Wall -g -fPIC
LDFLAGS = -L. -luthread -Wl,-rpath,'$$ORIGIN'

all: libuthread.so program

libuthread.so: uthread.c uthread.h
	$(CC) $(CFLAGS) -shared uthread.c -o $@

program: main.c libuthread.so
	$(CC) $(CFLAGS) -I. main.c $(LDFLAGS) -o $@

pos_neg_test: tests/pos_neg_test.c libuthread.so
	$(CC) $(CFLAGS) -I. tests/pos_neg_test.c $(LDFLAGS) -o $@

max_threads_test: tests/max_threads_test.c libuthread.so
	$(CC) $(CFLAGS) -I. tests/max_threads_test.c $(LDFLAGS) -o $@

global_test: tests/global_test.c libuthread.so
	$(CC) $(CFLAGS) -I. tests/global_test.c $(LDFLAGS) -o $@

test_build: pos_neg_test max_threads_test global_test
	@echo "Все тесты собраны"

clean:
	rm -f libuthread.so program pos_neg_test max_threads_test global_test