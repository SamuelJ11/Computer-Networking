# 2.4: DNS - The Internet's Directory Service

    • Hosts are also identified by IP addresses

        - IP addresses consist of 4 bytes separated by periods and is organized hierachchically (as we scan the address from left to right, we obtain more and more specific information about where the host is located in the Internet)

# 2.4.1: Services Provided by DNS

    • The DNS is a distributed database implemented in a hierarchy of DNS servers AND an application-layer protocol that allows hosts to query the distributed database

        - DNS protocol runs over UDP and uses port 53

    • Below is an outline of the process that takes place when browser (HTTP client) running on some user's host requests the URL www.someschool.edu/index.html (which first requires the host to obtain the IP address of www.someschool.edu):

        (1) user machine runs the client side of the DNS application

        (2) browser extracts the hostname (www.someschool.edu) from the URL and passes the hostname to the clinet side of the DNS application

        (3) the DNS client sends a query containing the hostname to a DNS server

        (4) the DNS client eventually receives a reply, which includes the IP address for the hostname

        (5) now the browser can initiate a TCP connection to the HTTP server process located at port 80 at that IP address

    • DNS provides a few other important services in addition to translating  hostname to IP addresses:

        (1) Host aliasing: a cononical hostname like relay1.west-coast.enterprise.com could have two aliases such as enterpise.com and www.enterprise.com

            - DNS can be invoked by an application to obtain the canonical hostname for a supplied alias hostname as well as the IP address of the host

        (2) Mail server aliasing: DNS can be invoked by a mail application to obtain the canocial hostname for a supplied alias hostname (gives relay1.west-coast.yahoo.com when inputted with yahoo.com)

        (3) Load distribution: busy sites such as cnn.com are replicated over multiple servers, which each server running on a different end system and each having a different IP address

            - in this case, a set of IP addresses is associated with one alias hostname

            - here the server will respond with the entire set of IP addresses, but rotates the ordering of the addresses within each reply, distributing the traffic among the replicated servers

# 2.4.2: Overview of How DNS Works

    • Suppose that some application (such as a Web browser or a mail client) running in a user’s host needs to translate a hostname to an IP address.

        (1) the application will invoke the client side of DNS via the UNIX gethostbyname() function

        (2) DNS takes over, sending a query message into the network (all query and reply messages are sent within UPD datagrams to port 53)

        (3) DNS in the user's host recieves a DNS replay message that provides the desired mapping, which is then passed ot the invoking application

## A distributed, Hierachical Database

    • DNS uses a large number of servers, organized in a hierarchical fashion and distributed around the world.

    • There are three classes of DNS servers - root, top-level domain (TLD) and authoritative - organized in a hierarchy (see figure 2.15)

        (1) Root DNS servers: there are nearly 2000 copies of 13 different root servers, and provide the IP addresses of the TLD servers

        (2) Top-level domain servers:  for each of the top-level domains—top-level domains such as com, org, net, edu, and gov, and all of the country top-level domains such as uk, fr, ca, and jp—there is TLD server (or server cluster)

        (3) Authoritative DNS servers: every organization with publicly accessible hosts (such as Web servers and mail servers) on the Internet must provide publicly accessible DNS records that map the names of those hosts to IP addresses

            - An organization’s authoritative DNS server houses these DNS records

    • A local DNS server does not strictly belong to the hierarchy of servers but is nevertheless central to the DNS architecture.

        - when a host connects to an ISP, the ISP provides the hsot with the IP addresses of one or more of its local DNS servers

        - rather than using your ISPs or institution's local DNS server, you can also configure your device to receive its local DNS service from a commercial provider such as Google (8.8.8.8)

    • When a host makes a DNS query, the query is sent to the local DNS server, which acts a proxy, forwarding the query into the DNS server hierarchy.

        * see figure 2.17 that ties some of these principles together in a nice picture

    • In a recursive DNS query, the client asks one server to do all the work, whereas in an iterative DNS query, each server gives partial answers ("I don't know, but ask this next server")

## DNS Caching

    • If a hostname/IP address pair is cached in a DNS server and another query arrives to the DNS server for the same hostname, the DNS server can provide the desired IP address, even if it is not authoritative for the hostname (aka, doesn't own that domain)

    • A local DNS server can also cache the IP addresses of TLD servers, thereby allowing the local DNS server to bypass the root DNS servers in a query chain.

        - for this reason, root servers are bypassed for all but a very small fraction of DNS queries

## 2.4.3: DNS Records and Messages

    • 