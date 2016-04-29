CFLAGS = -g -std=c99 -Wall
lili: lili.c
	cc $(CFLAGS) lili.c mpc.c -ledit -lm -o lili

