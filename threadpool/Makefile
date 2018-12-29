CC := cc
CFLAGS := -I inc -Wall -g -lpthread
INC := inc/cond.h inc/threadpool.h

all: threadpool_test

threadpool_test: main.o cond.o threadpool.o
	$(CC) $(CFLAGS) -o $@ $^

main.o: src/main.c
cond.o: src/cond.c
threadpool.o: src/threadpool.c

.PHONY: clean
clean:
	@rm -rf *.dSYM *.o $(BIN)
