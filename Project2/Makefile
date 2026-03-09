CC = gcc
CFLAGS = -Wall -g -pthread

PROJECTHEADERS = TimeInterval.h
PROJECTOBJECTS = pa2_skeleton.o SetNonBlocking.o TimeInterval.o
PROJECTSOURCES = pa2_skeleton.c SetNonBlocking.c TimeInterval.c

EXEC = pa2_skeleton

$(EXEC): $(PROJECTOBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c $(PROJECTHEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm *.o 