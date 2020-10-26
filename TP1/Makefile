CC = gcc
CFLAGS = -Wall
OBJ = src/messages.o src/alarm.o src/ll.o src/application.o
DEPS = src/macros.h src/messages.h src/alarm.h src/ll.h src/application.h
TARGETS = main

all: main

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

cable:
	$(CC) $(CFLAGS) -o $@ src/$@.c

main: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@ src/$@.c -lm

clean:
	@rm $(TARGETS) $(OBJ)
