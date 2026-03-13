#include "Packet.h"
#include <string.h>

void SerializeClient(client_struct *packet, char *buf)
{
    size_t offset = 0; /* this variable will keep track of the current offset in the buffer where we will write the next field */
    /* At buffer offset 0, we store the first field (base) which has a size of 2 bytes */
    memcpy(buf, &packet->base, sizeof(packet->base));
    offset += sizeof(packet->base); /* update the offset to point to the next field */

    /* At buffer offset 2, we store the second field (next_seqnum) which has a size of 2 bytes */
    memcpy(buf + offset, &packet->next_seqnum, sizeof(packet->next_seqnum));
    offset += sizeof(packet->next_seqnum); /* update the offset to point to the next field */

    /* At buffer offset 4, we store the beggining of the message, which has a size of 16 bytes */
    memcpy(buf + offset, packet->message, MESSAGE_SIZE);
    offset += MESSAGE_SIZE; /* update the offset to point to the next field */

    /* At buffer offset 20, we store the beggining of the client buffer, which has a size of 16 bytes */
    memcpy(buf + offset, packet->client_buf, MESSAGE_SIZE);
}

void DeserializeClient(char *buf, client_struct *packet)
{
    size_t offset = 0; /* this variable will keep track of the current offset in the buffer where we will write the next field */

    /* At buffer offset 0, we read the first field (base) which has a size of 2 bytes */
    memcpy(&packet->base, buf, sizeof(packet->base));
    offset += sizeof(packet->base); /* update the offset to point to the next field */

    /* At buffer offset 2, we read the second field (next_seqnum) which has a size of 2 bytes */
    memcpy(&packet->next_seqnum, buf + offset, sizeof(packet->next_seqnum));
    offset += sizeof(packet->next_seqnum); /* update the offset to point to the next field */

    /* At buffer offset 4, we read the third field (message) which has a size of 16 bytes */
    memcpy(packet->message, buf + offset, MESSAGE_SIZE);
    offset += MESSAGE_SIZE; /* update the offset to point to the next field */

    /* At buffer offset 20, we read the fourth field (client_buf) which has a size of 16 bytes */
    memcpy(packet->client_buf, buf + offset, MESSAGE_SIZE);
}

void SerializeServer(server_struct *packet, char *buf)
{
    size_t offset = 0; /* this variable will keep track of the current offset in the buffer where we will write the next field */

    /* At buffer offset 0, we store the first field (seq_num) which has a size of 2 bytes */
    memcpy(buf, &packet->expected_seqnum, sizeof(packet->expected_seqnum));
    offset += sizeof(packet->expected_seqnum); /* update the offset to point to the next field */
    
    /* At buffer offset 2, we store the beggining of the echo buffer, which has a size of 16 bytes */
    memcpy(buf + offset, packet->echo_buf, MESSAGE_SIZE);
}

void DeserializeServer(char *buf, server_struct *packet)
{
    size_t offset = 0; /* this variable will keep track of the current offset in the buffer where we will write the next field */

    /* At buffer offset 0, we read the first field (expected_seqnum) which has a size of 2 bytes */
    memcpy(&packet->expected_seqnum, buf, sizeof(packet->expected_seqnum));
    offset += sizeof(packet->expected_seqnum); /* update the offset to point to the next field */

    /* At buffer offset 2, we read the second field (echo_buf) which has a size of 16 bytes */
    memcpy(packet->echo_buf, buf + offset, MESSAGE_SIZE);
}