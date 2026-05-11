CC = gcc
CFLAGS = -Wall -g -pthread

TASK1_SOURCES = Task1.c TimeInterval.c
TASK1_OBJECTS = $(TASK1_SOURCES:.c=.o)
TASK1_HEADERS = TimeInterval.h Task1.h
TASK1_EXEC = PA2_Task1

TASK2_SOURCES = Task2.c Packet.c TimeInterval.c Hash.c
TASK2_OBJECTS = $(TASK2_SOURCES:.c=.o)
TASK2_HEADERS = Packet.h Task2.h Hash.h
TASK2_EXEC = PA2_Task2

default: 1

1: $(TASK1_EXEC)

$(TASK1_EXEC): $(TASK1_OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

.o: %.c $(TASK1_HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

2: $(TASK2_EXEC)

$(TASK2_EXEC): $(TASK2_OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c $(TASK2_HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o 