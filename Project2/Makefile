CC = gcc
CXXFLAGS = -Wall -g -pthread

PROJECTOBJECTS = pa2_skeleton.o SetNonBlocking.o 
PROJECTSOURCES = pa2_skeleton.c SetNonBlocking.c

EXEC = pa2_skeleton

$(EXEC): $(PROJECTOBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm *.o 