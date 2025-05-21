CC=gcc
CFLAGS=-Wall -g 
BINARY=icsh
SRCS = icsh.c icsh_buildin.c icsh_external.c

all: $(BINARY)

$(BINARY): $(SRCS)
	$(CC) -o $(BINARY) $(CFLAGS) $(SRCS)

.PHONY: clean

clean:
	rm -f $(BINARY)
