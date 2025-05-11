CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -Werror -g -ggdb
FILE=sps

${FILE}: ${FILE}.o

memcheck:
	valgrind --tool=memcheck --leak-check=yes --track-origins=yes ./${FILE}

debug:
	gdb -tui ./${FILE}

clear:
	rm -f ${FILE} *.o