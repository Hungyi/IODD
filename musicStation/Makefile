CC = arm-linux-gcc

CFLAGS = -g

EXEC = chsong
OBJS = lookdir.o chsong.o

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) -o $@ $(OBJS) -lpthread

lookdir.o: lookdir.c lookdir.h
	$(CC) -c lookdir.c $(CFLAGS)

chsong.o: chsong.c
	$(CC) -c chsong.c $(CFLAGS) 

clean:
	rm -f chsong *.o
