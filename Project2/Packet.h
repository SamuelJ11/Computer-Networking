#ifndef PACKET_H
#define PACKET_H

#include <string.h> /* for memcpy() */

/* Initialize the struct members with a fixed message size and sequence number array */
typedef struct {
    unsigned short seq_num[1000]; /* used to hold sequence number of each packet */
    char message[16] = "Hello, Shixiong!"; /* send 16 bytes every time */
    char client_buf[16]; /* used to hold the message recieved by the client */
} client_struct;

typedef struct {
    unsigned short ack_num[1000]; /* used to hold ack number of each packet */
    char echo_buf[16]; /* used to hold the message echoed back by the server */
} server_struct;

char *Serialize(unsigned char flag, packet_struct packet); /* serialize the packet struct into a byte stream to send over the network */
char *Deserialize(unsigned char flag, char *buf); /* takes the serialized buffer we generated and reconstructs the original packet struct */

#endif