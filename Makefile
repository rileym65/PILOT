PROJECT = pilot
CC = gcc
DEFS =
INCS =
INCDIR =
LIBS =
LIBDIR =
OBJS = \
	main.o

$(PROJECT): $(OBJS)
	$(CC) $(DEFS) $(LIBDIR) $(OBJS) $(LIBS) -o $(PROJECT)

.c.o:
	$(CC) $(DEFS) $(INCDIR) $(INCS) -c $<

clean:
	-rm *.o
	-rm $(PROJECT)

main.o:      header.h main.c

