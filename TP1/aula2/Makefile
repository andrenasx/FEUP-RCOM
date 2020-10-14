CC = gcc
CFLAGS = -Wall
DEPS = macros.h messages.h
OBJ = messages.o
TARGETS = cable writenoncanonical noncanonical

all: cable writenoncanonical noncanonical

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

cable:
	$(CC) $(CFLAGS) -o $@ $@.c

writenoncanonical: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $@.c $(OBJ) -lm

noncanonical: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $@.c $(OBJ) -lm

clean:
	@rm $(TARGETS) $(OBJ)
