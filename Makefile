CC=clang
CFLAGS=-pedantic -Wall -Wextra -Werror -g -fsanitize=address

all: test readme_test

test: cesu8.h test.c
	$(CC) $(CFLAGS) -ansi -o test test.c

readme_test: cesu8.h readme_test.c
	$(CC) $(CFLAGS) -o readme_test readme_test.c

clean:
	rm -f test
	rm -f readme_test

