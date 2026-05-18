# 6.4: Switched Local Area Networks

    • Instead of using IP addresses, switches use link-layer addresses to forward link-layer frames through the network of switches.

## 6.4.1: Link-Layer Addressing and ARP

### MAC Addresses

    • In truth, it is not hosts and routers that have link-layer addresses but rather their adapters (that is, network interfaces) that have link-layer addresses. 

        - it’s important to note, however, that link-layer switches do not have link-layer addresses associated with their interfaces that connect to hosts and routers.

        * hosts dont send frames to the switch's MAC address; the switch simply forwards them

    • It should be obvious by now that a link-layer address is called a MAC address (also sometimes called a LAN or physical address)

        - because MAC address are 6 hexadecimal bytes, there are 2⁴⁸ possible MAC addresses

        - although MAC addresses were designed to be permanent, it is now possible to change an adapter’s MAC address via software.

    • The IEEE ensures that manufacturers assign globally unique MAC address for each piece of hardware:

        - when a company wants to manufacture adapters, it purchases a chunk of the address space consisting of 2²⁴ addresses for a nominal fee. IEEE allocates the chunk of these addresses by fixing the first 24 bits of a MAC address and letting the company create unique combinations of the last 24 bits for each adapter

    • When an adapter wants to send a frame to some destination adapter, the sending adapter inserts the destination adapter’s MAC address into the frame and then sends the frame into the LAN:

        - thus, when an adapter receives a frame, it will check to see whether the destination MAC address in the frame matches its own MAC address 

    • However, sometimes a sending adapter does want all the other adapters on the LAN to receive and process the frame it is about to send;
    
      -  in which case the sending adapter insersts a special MAC broadcast address into the destination address field of the frame (this is donated as FF-FF-FF-FF-FF-FF)

### Address Resolution Protocol (ARP)