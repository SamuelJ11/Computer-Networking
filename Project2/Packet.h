#ifndef PACKET_H
#define PACKET_H

#include <string.h> /* for memcpy() */

/* Initialize the struct members with a fixed message size and sequence number array */
typedef struct {
    unsigned short seq_num[1000]; /* used to hold sequence number of each packet */
    unsigned short ack_num[1000]; /* used to hold ack number of each packet */
    char message[16] = "Hello, Shixiong!"; /* send 16 bytes every time */
    char client_buf[16]; /* used to hold the message recieved by the client */
    char echo_buf[16]; /* used to hold the message echoed back by the server */
} packet_struct;

char *Serialize(packet_struct packet);
char *Deserialize(char *buf);

#endif