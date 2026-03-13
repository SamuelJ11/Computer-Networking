#ifndef PACKET_H
#define PACKET_H

#define CLIENT_PACKET_SIZE 36 /* 2 + 2 bytes for base and next sequence number + 16 bytes for message + 16 bytes for client buffer */
#define SERVER_PACKET_SIZE 18 /* 2 bytes for expected sequence number + 16 bytes for echo buffer */
#define MESSAGE_SIZE 16 /* fixed message size of 16 bytes */

#include <string.h>

/* Initialize the struct members with a fixed message size and struct specific fields */
typedef struct {
    unsigned short base; /* oldest unacknowledged packet */
    unsigned short next_seqnum; /* next packet to send */
    char message[MESSAGE_SIZE]; /* send 16 bytes every time */
    char client_buf[MESSAGE_SIZE]; /* used to hold the message recieved by the client */
} client_struct;

typedef struct {
    unsigned short expected_seqnum; /* used to hold ack number of the last correctly recieved packet */
    char echo_buf[MESSAGE_SIZE]; /* used to hold the message echoed back by the server */
} server_struct;

void SerializeClient(client_struct *packet, char *buf);  /* serialize the client packet struct into a byte stream to send over the network */
void SerializeServer(server_struct *packet, char *buf);  /* serialize the server packet struct into a byte stream to send over the network */

void DeserializeClient(char *buf, client_struct *packet); /* takes the serialized buffer we generated and reconstructs the original packet struct */
void DeserializeServer(char *buf, server_struct *packet); /* takes the serialized buffer we generated and reconstructs the original packet struct */

#endif