CC = gcc
CFLAGS = -Wall -g -pthread

PROJECTOBJECTS = pa2_skeleton.o SetNonBlocking.o CalcTimeIntval.o
PROJECTSOURCES = pa2_skeleton.c SetNonBlocking.c CalcTimeIntval.c

EXEC = pa2_skeleton

$(EXEC): $(PROJECTOBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm *.o 