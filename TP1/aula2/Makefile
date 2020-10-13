CC = gcc
CFLAGS = -Wall
DEPS = macros.h messages.h
OBJ = messages.o
TARGETS = writenoncanonical noncanonical

all: writenoncanonical noncanonical

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

writenoncanonical: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $@.c $(OBJ) -lm

noncanonical: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $@.c $(OBJ) -lm

clean:
	@rm $(TARGETS) $(OBJ)
