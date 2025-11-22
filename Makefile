CC := cc
CFLAGS := -Wall -Wextra

build:
	$(CC) $(CFLAGS) -o mine main.c
