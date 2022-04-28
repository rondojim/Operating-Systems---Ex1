
CC = gcc

CFLAGS = -c -Wall
LDFLAGS = -lm

OBJS = mngstd.o ADTMap.o ADTList.o secretary.o 

EXEC = mngstd

ARGS = -i testcase.txt

$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC) $(LDFLAGS)

clean:
	rm -f $(OBJS) $(EXEC)

run: $(EXEC)
	./$(EXEC) $(ARGS)

