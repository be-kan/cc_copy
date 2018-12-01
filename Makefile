CFLAGS=-Wall -std=c11 -g
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

9cc: $(OBJS)
	cc -o $@ $(OBJS) $(LDFLAGS)

$(OBJS): 9cc.h

test: 9cc test/test.c
	./9cc -test

	@gcc -E -P test/test.c | ./9cc - > tmp-test1.s
	@./9cc test/token.c > tmp-test2.s
	@gcc -c -o tmp-test2.o test/gcc.c
	@gcc -static -o tmp-test tmp-test1.s tmp-test2.s tmp-test2.o
	@./tmp-test

clean:
	rm -f 9cc *.o *~ tmp* a.out test/*~

.PHONY: test clean