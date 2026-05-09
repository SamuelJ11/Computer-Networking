#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "Task2.h"

#define MAX_ID 32
#define TABLE_SIZE (10 * CLIENT_THREADS)

typedef struct {
    char client_id[MAX_ID];
    int next_seq_num;
} client;

client *hash_table[TABLE_SIZE];

unsigned int hash (char *client_id)
{
    unsigned int length = strnlen(client_id, MAX_ID);
    unsigned int hash_value = 0;

    for (int i = 0; i < length; i++)
    {
        hash_value += client_id[i];
    }

    return hash_value % TABLE_SIZE;
}

void init_hash_table()
{
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        hash_table[i] = NULL;
    }
}

void print_table()
{
    for (int i = 0; i < TABLE_SIZE; i++)
    {   
        if (hash_table[i] == NULL)
        {
            printf("\t%i\t---\n", i);
        }
        else
        {
            printf("\t%i\t%s\n", i, hash_table[i]->client_id);
        }
    }
}

bool hash_table_insert(client *c)
{
    if (c == NULL)
    {
        return false;
    }

    int index = hash(c->client_id);

    for (int i = 0; i < TABLE_SIZE; i++)
    {
        int probe = (i + index) % TABLE_SIZE;

        if (hash_table[probe] == NULL)
        {
            hash_table[probe] = c;
            return true;
        }

    }

    return false;
}

client *hash_table_lookup(char *client_id)
{
    int index = hash(client_id);

    for (int i = 0; i < TABLE_SIZE; i++)
    {
        int probe = (i + index) % TABLE_SIZE;

        if (hash_table[probe] == NULL)
        {
            return NULL;
        }
        else if (strncmp(hash_table[probe]->client_id, client_id, MAX_ID) == 0)
        {
            return hash_table[probe];
        }
    }

    return NULL;
}

int main() 
{
    init_hash_table();
    
    client Sam  = {.client_id = "172.31.44.198", .next_seq_num = 1};
    client Kate = {.client_id = "10.84.12.57",   .next_seq_num = 3};
    client Jake = {.client_id = "192.168.77.203", .next_seq_num = 4};
    client Edna = {.client_id = "172.20.9.144",   .next_seq_num = 2};
    client Sara = {.client_id = "10.15.201.88",   .next_seq_num = 5};
    client Ben  = {.client_id = "192.168.5.241",  .next_seq_num = 7};
    client Ethan = {.client_id = "13.15.181.88",   .next_seq_num = 5};
    client Melanie  = {.client_id = "123.120.70.14",  .next_seq_num = 7};

    hash_table_insert(&Sam);
    hash_table_insert(&Kate);
    hash_table_insert(&Jake);
    hash_table_insert(&Edna);
    hash_table_insert(&Sara);
    hash_table_insert(&Ben);
    hash_table_insert(&Ethan);
    hash_table_insert(&Melanie);

    print_table();

    client *tmp = hash_table_lookup("123.120.70.14");
    if (tmp == NULL)
    {
        printf("Not found\n");
    }
    else
    {
        printf("found %s\n", tmp->client_id);
    }

    return 0;
}