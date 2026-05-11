#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Hash.h"

client_state *hash_table[TABLE_SIZE];

unsigned int hash(char *client_id)
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
            printf("\t%d\t---\n", i);
        }
        else
        {
            printf("\t%d\t%s\n", i, hash_table[i]->client_id);
        }
    }
}

bool hash_table_insert(client_state *c)
{
    if (c == NULL)
    {
        return false;
    }

    unsigned int index = hash(c->client_id);

    for (int i = 0; i < TABLE_SIZE; i++)
    {
        unsigned int probe = (index + i) % TABLE_SIZE;

        if (hash_table[probe] == NULL)
        {
            hash_table[probe] = c;
            return true;
        }
    }

    return false; 
}

client_state *hash_table_lookup(char *client_id)
{
    unsigned int index = hash(client_id);

    for (int i = 0; i < TABLE_SIZE; i++)
    {
        unsigned int probe = (index + i) % TABLE_SIZE;

        if (hash_table[probe] == NULL)
        {
            return NULL; 
        }

        if (strncmp(hash_table[probe]->client_id, client_id, MAX_ID) == 0)
        {
            return hash_table[probe];
        }
    }

    return NULL;
}