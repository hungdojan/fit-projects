CC=gcc
CFLAGS=-std=c11 -Wall -Wextra -pedantic
#CFLAGS+=-g
CFLAGS+=-O2
LDLIBS=-lpcap -lm
#DEFINES=-DDEBUG

TARGET=ipk-sniffer
OBJS=$(patsubst %.c,%.o,$(wildcard *.c))

LOGIN=xdohun00

############################################

.PHONY: all clean run pack valgrind debug

############################################

all: $(TARGET)

run: $(TARGET)
	./$< -i wlp3s0 --icmp

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS) $(DEFINES)

clean:
	rm -f *.o $(TARGET) *.tar

valgrind: $(TARGET)
	sudo valgrind --leak-check=full --track-origins=yes ./$<

debug: $(TARGET)
	gdb -tui ./$<

pack:
	rm -f $(LOGIN).tar
	tar -cvf $(LOGIN).tar Makefile *.c *.h README.md manual.pdf
