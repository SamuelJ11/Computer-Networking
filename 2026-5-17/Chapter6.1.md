# 6.1: Introduction to the Link Layer

    • We refer to any device that runs a link-layer (level 2) protocol as a node;

        - nodes inlude hosts, routers, switches, and WiFi access points

        - we'll refer to the communication channelsthat connect adjacent nodes along the communication path as links

        - over a given link, a transmitting node encapsulates teh datagram in a link-layer frame and transmits the frame into the link

## 6.1.1: The Services Provided by the Link Layer

    • Although the basic service of any link layer is to move a datagram from one node to an adjacent node over a single link, the details of the provided service can vary from one link-layer protocol to the next;

        - framing: almost all link-layer protocols encapsulate each network-layer datagram within a link-layer frame before transmission over the link (We’ll see several different frame formats when we examine specific link-layer protocols in the second half of this chapter)

        - link access: a medium access controll (MAC) protocol specifies the rules by which a frame is transmitted onto the link.  the interesting case is when multiple nodes share a single broadcast link—the so-called multiple access problem. here, the MAC protocol serves to coordinate the frame transmissions of the many nodes

        - reliable delivery: when a link-layer protocol provides reliable delivery service, it guarantees to move each network-layer datagram across the link without error (similar to a transport-layer reliable delivery service, a link-layer reliable delivery service can be achieved with acknowledgments and retransmissions)

            * the link-layer reliable delivery service is often used for links that are prone to high error rates, such as a wireless link, with the goal of correcting an error locally—on the link where the error occurs

            * however, link-layer reliable delivery can be considered an unnecessary overhead for low bit-error links, including fiber, coax, and many twisted-pair copper links
  
        - error detection and correction: the link-layer hardware in a receiving node can incorrectly decide that a bit in a frame is zero when it was transmitted as a one, and vice versa. because there is no need to forward a datagram that has an error, many link-layer protocols provide a mechanism to detect such bit errors

## 6.1.2: Where is the Link Layer Implemented?

    • For the most part, the link layer is implemented on a chip called the network adapter, also sometimes known as a network interface controller (NIC).

    • On the sending side, the controller takes a datagram that has been created and stored in host memory by the higher layers of the protocol stack, encapsulates the datagram in a link-layer frame (filling in the frame’s various fields), and then transmits the frame into the communication link, following the link-access protocol:

        - on the receiving side, a controller receives the entire frame, and extracts the network-layer datagram. If the link layer performs error detection, then it is the sending controller that sets the error-detection bits in the frame header and it is the receiving controller that performs error detection

    • The link layer is a combination of hardware and software—the place in the protocol stack where software meets hardware.