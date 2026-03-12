#include "Packet.h"

char *Serialize(unsigned char flag, packet_struct packet) 
{
    /* This byte will be used to indicate whether the packet is from the client or server */
    unsigned char flag = flag; 

    switch (flag)
    {
        case 0: /* packet is from client */

            /* Calculate the size of the message */
            int message_size = strlen(packet.message);  

            /* Calculate the total buffer size for the client */
            int buffer_size = sizeof(packet.seq_num) + message_size + sizeof(client_buf); /* 2000 + 32 = 2032 bytes */

            /* Allocate buffer_size + 1 bytes */
            char *buf = buffer_size + 1;  /* allocate 2033 bytes, the extra byte is for null-termination */

            /* At buffer offset 0, we store the first field (seq_num) which has a size of 2000 bytes */
            memcpy(buf, &packet.seq_num, sizeof(packet.seq_num));

            /* At buffer offset 2000, we store the beggining of the message, which has a size of 16 bytes */
            memcpy(buf + sizeof(packet.seq_num), &packet.message, message_size);

            /* At buffer offset 2016, we store the beggining of the client buffer, which has a size of 16 bytes */
            memcpy(buf + sizeof(packet.seq_num) + message_size, &packet.client_buf, message_size);

            /* At buffer offset 2032, we can optionally store a null terminator to allow printing of the message */
            buf[buffer_size] = '\0';

            return buf;
            break;

        case 1: /* packet is from server */
            
            /* Calculate the size of the message */
            int message_buf = strlen(packet.echo_buf);  

            /* Calculate the total buffer size for the server */
            int buffer_size = sizeof(packet.ack_num) + message_buf; /* 2000 + 16 = 2016 bytes */

            /* Allocate buffer_size + 1 bytes */
            char *buf = buffer_size + 1;  /* allocate 2017 bytes, the extra byte is for null-termination */

            /* At buffer offset 0, we store the first field (seq_num) which has a size of 2000 bytes */
            memcpy(buf, &packet.ack_num, sizeof(packet.ack_num));

            /* At buffer offset 2000, we store the beggining of the echo buffer, which has a size of 16 bytes */
            memcpy(buf + sizeof(packet.ack_num), &packet.echo_buf, message_buf);

            /* At buffer offset 2016, we can optionally store a null terminator to allow printing of the message */
            buf[buffer_size] = '\0';

            return buf;
            break;

        default:
            /* Invalid flag recieved */
            DieWithError("invalid flag for serialization");
    }
}

char *Deserialize(unsigned char flag, char *buf)
{
    /* This byte will be used to indicate whether the packet is from the client or server */
    unsigned char flag = flag; 

    switch (flag)
    {
        case 0: /* packet is from client */

            /* Declare a client_struct variable to hold the deserialized data */
            client_struct client_packet;

            /* Calculate the size of the message */
            int message_size = strlen(packet.message); 

            /* At buffer offset 0, we read the first field (seq_num) which has a size of 2000 bytes */
            memcpy(&client_packet.seq_num, buf, sizeof(client_packet.seq_num));

            /* At buffer offset 2000, we read the second field (message) which has a size of 16 bytes */
            memcpy(&client_packet.message, buf + sizeof(client_packet.seq_num), message_size);

            /* At buffer offset 2016, we read the third field (client_buf) which has a size of 16 bytes */
            memcpy(&client_packet.client_buf, buf + sizeof(client_packet.seq_num) + message_size, sizeof(client_buf));

            /* Return the reconstructed client packet struct */
            return client_packet;
            break;

        case 1: /* packet is from server */

            /* Declare a server_struct variable to hold the deserialized data */
            server_struct server_packet;

            /* At buffer offset 0, we read the first field (ack_num) which has a size of 2000 bytes */
            memcpy(&server_packet.ack_num, buf, sizeof(server_packet.ack_num));

            /* At buffer offset 2000, we read the second field (echo_buf) which has a size of 16 bytes */
            memcpy(&server_packet.echo_buf, buf + sizeof(server_packet.ack_num), sizeof(echo_buf));

            /* Return the reconstructed server packet struct */
            return server_packet;
            break;

        default:
            /* Invalid flag recieved */
            DieWithError("invalid flag for deserialization");
    }
}