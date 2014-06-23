CROSS=arm-linux-

all:leds

leds:leds.c
	$(CROSS)gcc -o leds leds.c
	$(CROSS)strip leds
clean:
	@rm -vf leds *.o *~
