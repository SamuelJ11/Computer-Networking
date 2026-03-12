#include "Packet.h"

char *Serialize(packet_struct packet) 
{
    /* Calculate the size of the message */
    int message_size = strlen(packet.message);  

    /* Calculate the total buffer size */
    int buffer_size = 2000 * sizeof(unsigned short) + message_size; /* (2000 * 2) + 16 = 4016 bytes */

    /* Dynamically allocate buffer_size + 1 bytes and store the pointer to that first memory block in buf */
    char *buf = buffer_size + 1;  /* allocate 4017 bytes, the extra byte is for null-termination */

    /* At buffer offset 0, we store the first field (seq_num) which has a size of 2000 bytes */
    memcpy(buf, &packet.seq_num, sizeof(packet.seq_num));

    /* At buffer offset 2000, we store the second field (ack_num) which has a size of 2000 bytes */
    memcpy(buf, &packet.ack_num, sizeof(packet.ack_num));

    /* At buffer offset 4000, we store the beggining of the message, which has a size of 16 bytes */
    memcpy(buf + 2 * sizeof(packet.seq_num), &packet.message, message_size);

    /* At buffer offset 2016, we can optionally store a null terminator to allow printing of the message */
    buf[buffer_size] = '\0';

    return buf;
}

char *Deserialize(char *buf)
{
    /* Declare a packet_struct variable to hold the deserialized data */
    packet_struct packet;

    /* At buffer offset 0, we read the first field (seq_num) which has a size of 2000 bytes */
    memcpy(&packet.seq_num, buf, sizeof(packet.seq_num));

    /* At buffer offset 2000, we read the second field (ack_num) which has a size of 2000 bytes */
    memcpy(&packet.ack_num, buf + sizeof(packet.seq_num), sizeof(packet.ack_num));

    /* At buffer offset 4000, we read the beggining of the message, which has a size of 16 bytes */
    memcpy(&packet.message, buf + 2 * sizeof(packet.seq_num), MESSAGE_SIZE);

    /* Return the reconstructed packet struct */
    return packet;
}