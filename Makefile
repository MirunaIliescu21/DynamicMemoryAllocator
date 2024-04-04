CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g

.PHONY:	build run_sfl clean

build: sfl

run_sfl: sfl
	./sfl

sfl: main.o function.o function_malloc.o
	$(CC) $(CFLAGS) -o $@ $^

main.o: main.c function.h struct.h function_malloc.h
	$(CC) $(CFLAGS) -c $<

function.o: function.c function.h struct.h function_malloc.c function_malloc.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f sfl *.o
