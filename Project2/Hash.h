#ifndef HASH_H

#include "Task2.h"
#include <stdbool.h>

#define HASH_H
#define MAX_ID 32
#define TABLE_SIZE (10 * CLIENT_THREADS)

typedef struct {
    char client_id[MAX_ID];
    int expected_seq_num;
} client_state;

extern client_state *hash_table[TABLE_SIZE];

unsigned int hash(char *client_id);

void init_hash_table();

void print_table();

bool hash_table_insert(client_state *c);

client_state *hash_table_lookup(char *client_id);

#endif