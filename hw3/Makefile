CFLAGS=-Wall -std=gnu99 -pedantic
all: player ringmaster
ringmaster: ringmaster.c
	gcc $(CFLAGS) -g -o ringmaster ringmaster.c
player: player.c
	gcc $(CFLAGS) -g -o player player.c
clean:
	\rm -f player ringmaster