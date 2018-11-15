EXE=tra_map

OBJS=applier.o main.o

CC=gcc

$(EXE): $(OBJS)
	$(CC) $(OBJS) -o $(EXE)

main.o: main.c applier.h
	$(CC) -c -o main.o main.c

applier.o: applier.c applier.h
	$(CC) -c -o applier.o applier.c

clean:
	rm *.o
