all:	powermate-hooks led

powermate-hooks:	powermate-hooks.o lib/powermate.o
	$(CC) lib/powermate.o powermate-hooks.o -o powermate-hooks

led:	led.o lib/powermate.o
	$(CC) lib/powermate.o led.o -o led

clean:
	rm -f *.o *~ powermate-hooks led

%.o:    %.c
	$(CC) -O2 -Wall -c $< -o $@
