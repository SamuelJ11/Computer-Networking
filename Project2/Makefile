CC = gcc
CXXFLAGS = -Wall -g -pthread

PROJECTOBJECTS = pa1_skeleton.o SetNonBlocking.o 
PROJECTSOURCES = pa1_skeleton.c SetNonBlocking.c

EXEC = pa1_skeleton

$(EXEC): $(PROJECTOBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm *.o 