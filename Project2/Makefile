CC = gcc
CFLAGS = -Wall -g -pthread

HEADERS = TimeInterval.h
COMMON_SOURCES = SetNonBlocking.c TimeInterval.c

TASK1_SOURCES = Task1.c $(COMMON_SOURCES)
TASK1_OBJECTS = $(TASK1_SOURCES:.c=.o)
TASK1_EXEC = PA2_Task1

TASK2_SOURCES = Task2.c $(COMMON_SOURCES)
TASK2_OBJECTS = $(TASK2_SOURCES:.c=.o)
TASK2_EXEC = PA2_Task2

default: 1

1: $(TASK1_EXEC)

$(TASK1_EXEC): $(TASK1_OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

2: $(TASK2_EXEC)

$(TASK2_EXEC): $(TASK2_OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o 