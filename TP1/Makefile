CC = gcc
CFLAGS = -Wall
OBJ = src/messages.o src/ll.o src/alarm.o
DEPS = src/macros.h src/messages.h src/ll.h src/alarm.h
TARGETS = cable writenoncanonical noncanonical

all: cable writenoncanonical noncanonical

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

cable:
	$(CC) $(CFLAGS) -o $@ src/$@.c

writenoncanonical: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@ src/$@.c -lm

noncanonical: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@ src/$@.c -lm

clean:
	@rm $(TARGETS) $(OBJ)
