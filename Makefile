CC = gcc
CFLAGS = -Wall -g -fPIC
LDFLAGS = -shared

all: libmythread.so

libmythread.so: mythread.c mythread.h
	$(CC) $(CFLAGS) $(LDFLAGS) mythread.c -o $@

%: tests/%.c libmythread.so
	$(CC) $(CFLAGS) -I. $< -L. -lmythread -Wl,-rpath,'$$ORIGIN' -o $@

tests: $(patsubst tests/%.c,%,$(wildcard tests/*.c))

clean:
	rm -f libmythread.so pos_test_* neg_test_*
