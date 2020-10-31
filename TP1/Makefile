CC = gcc
CFLAGS = -Wall
OBJ = src/messages.o src/alarm.o src/ll.o
DEPS = src/macros.h src/messages.h src/alarm.h src/ll.h
TARGETS = application

all: application

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

cable:
	$(CC) $(CFLAGS) -o $@ src/$@.c

application: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@ src/$@.c -lm

clean:
	@rm $(TARGETS) $(OBJ)
