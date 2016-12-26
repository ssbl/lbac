CC = gcc
CFLAGS = -Wall

cradle: cradle.c
	$(CC) $(CFLAGS) $< -o $@

parser: parser.c
	$(CC) $(CFLAGS) $< -o $@

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@
