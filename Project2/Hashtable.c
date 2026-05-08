#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_NAME 32
#define TABLE_SIZE 10

typedef struct {
    char client_id[MAX_NAME];
    int next_seq_num;
} client;

client *hash_table[TABLE_SIZE];

unsigned int hash (char *client_id)
{
    unsigned int length = strnlen(client_id, MAX_NAME);
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

    if (hash_table[index] != NULL)
    {
        return false;
    }

    hash_table[index] = c;

    return true;
}

client *hash_table_lookup(char *client_id)
{
    int index = hash(client_id);

    if (hash_table[index] != NULL && strncmp(hash_table[index]->client_id, client_id, TABLE_SIZE) == 0)
    {
        return hash_table[index];
    }

    else
    {
        return NULL;
    }
}

int main() 
{
    init_hash_table();
    
    client Sam = {.client_id = "192.168.12.1", .next_seq_num = 1};
    client Kate = {.client_id = "192.168.12.2", .next_seq_num = 3};
    client Jake = {.client_id = "192.168.12.3", .next_seq_num = 4};

    hash_table_insert(&Sam);
    hash_table_insert(&Kate);
    hash_table_insert(&Jake);

    print_table();

    client *tmp = hash_table_lookup("192.168.12.1");
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