#include "Packet.h"
#include <string.h>

void SerializeClient(client_struct *packet, char *buf)
{
    /* At buffer offset 0, we store the first field (seq_num) which has a size of 2 bytes */
    memcpy(buf, &packet->seq_num, sizeof(packet->seq_num));

    /* At buffer offset 2, we store the beggining of the message, which has a size of 16 bytes */
    memcpy(buf + sizeof(packet->seq_num), packet->message, MESSAGE_SIZE);

     /* At buffer offset 18, we store the beggining of the client buffer, which has a size of 16 bytes */
    memcpy(buf + sizeof(packet->seq_num) + MESSAGE_SIZE, packet->client_buf, MESSAGE_SIZE);
}

void DeserializeClient(char *buf, client_struct *packet)
{
    /* At buffer offset 0, we read the first field (seq_num) which has a size of 2 bytes */
    memcpy(&packet->seq_num, buf, sizeof(packet->seq_num));

    /* At buffer offset 2, we read the second field (message) which has a size of 16 bytes */
    memcpy(packet->message, buf + sizeof(packet->seq_num), MESSAGE_SIZE);

    /* At buffer offset 18, we read the third field (client_buf) which has a size of 16 bytes */
    memcpy(packet->client_buf, buf + sizeof(packet->seq_num) + MESSAGE_SIZE, MESSAGE_SIZE);
}

void SerializeServer(server_struct *packet, char *buf)
{
    /* At buffer offset 0, we store the first field (seq_num) which has a size of 2 bytes */
    memcpy(buf, &packet->expected_seqnum, sizeof(packet->expected_seqnum));

    /* At buffer offset 2, we store the beggining of the echo buffer, which has a size of 16 bytes */
    memcpy(buf + sizeof(packet->expected_seqnum), packet->echo_buf, MESSAGE_SIZE);
}

void DeserializeServer(char *buf, server_struct *packet)
{
    /* At buffer offset 0, we read the first field (expected_seqnum) which has a size of 2 bytes */
    memcpy(&packet->expected_seqnum, buf, sizeof(packet->expected_seqnum));

    /* At buffer offset 2, we read the second field (echo_buf) which has a size of 16 bytes */
    memcpy(packet->echo_buf, buf + sizeof(packet->expected_seqnum), MESSAGE_SIZE);
}