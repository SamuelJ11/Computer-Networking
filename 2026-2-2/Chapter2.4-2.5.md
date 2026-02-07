# 2.4: DNS - The Internet's Directory Service

    • Hosts are also identified by IP addresses

        - IP addresses consist of 4 bytes separated by periods and is organized hierachchically (as we scan the address from left to right, we obtain more and more specific information about where the host is located in the Internet)

## 2.4.1: Services Provided by DNS

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

## 2.4.2: Overview of How DNS Works

    • Suppose that some application (such as a Web browser or a mail client) running in a user’s host needs to translate a hostname to an IP address.

        (1) the application will invoke the client side of DNS via the UNIX gethostbyname() function

        (2) DNS takes over, sending a query message into the network (all query and reply messages are sent within UPD datagrams to port 53)

        (3) DNS in the user's host recieves a DNS replay message that provides the desired mapping, which is then passed ot the invoking application

### A distributed, Hierachical Database

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

### DNS Caching

    • If a hostname/IP address pair is cached in a DNS server and another query arrives to the DNS server for the same hostname, the DNS server can provide the desired IP address, even if it is not authoritative for the hostname (aka, doesn't own that domain)

    • A local DNS server can also cache the IP addresses of TLD servers, thereby allowing the local DNS server to bypass the root DNS servers in a query chain.

        - for this reason, root servers are bypassed for all but a very small fraction of DNS queries

## 2.4.3: DNS Records and Messages

    • The DNS servers that together implement the DNS distributed database store resource records (RRs), including RRs that provide hostname-to-IP address mappings.

        - these are four-tuples that contain the the following fields:

        (Name, Value, Type, TTL)

        - if 'Type' = A, then 'Name' is a hostname and 'Value' is the IP address for the hostname

        - if 'Type' = NS, then 'Name' is a domain (such as foo.com) and 'Value' is the hostname of an authoritatiave DNS server that knows how to obtain the IP addresses for hosts in the domain.

        - if 'Type' = CNAME, then 'Value' is a canonical hostname for the alias hostname 'Name'

        - if 'Type' = MX, then 'Value' is the canonical name of a mail server that has an alias hostname 'Name;

    • If a DNS server is authoritative for a particular hostname, then the DNS server will contain a Type A record for the hostname.

    • If a server is not authoritative for a hostname, then the server will contain a Type NS record for the domain that includes the hostname.

        - it will also contain a Type A record that provides the IP address of the DNS server in the  field of the NS record

### DNS Messages

    • See figure 2.19 and the following description for an overview of DNS queries and replies

### Inserting Records into the DNS Database

    • How do records get into the DNS database in the first place?

    • The first step is to register a domain name at a registrar.

    • Next, you need to provide the registrar with the names and IP addresses of you primary and secondary authoritative DNS servers

        - for each of these two authoritative DNS servers, the registrar would then make sure that a Type NS and a Type A record are entered into the TLD com servers

# 2.5: Video Streaming and Content Distribution Networks

## 2.5.1: Internet Video

    • In streaming stored video applications, the underlying medium is prerecorded video, such as a movie, a television show, a prerecorded sporting event, or a prerecorded user-generated video (such as those commonly seen on YouTube). 
    
    • These prerecorded videos are placed on servers, and users send requests to the servers to view the videos on demand. Many Internet companies today provide streaming video, including, Netflix, YouTube (Google), Amazon, and TikTok.

    • Video = fast pictures → pictures are pixels → pixels are bits → raw video is enormous → compression shrinks it by sacrificing quality → higher bitrate = better looking video.

    • In order to provide continuous playout, the network must provide an average throughput to the streaming application that is at least as large as the bit rate of the compressed video.

## 2.5.2: HTTP Streaming and Dash

    • In HTTP streaming, the video is simply stored at an HTTP server as an ordinary file with a specific URL

        - the server then sends the video file, within an HTTP response message, as quickly as the underlying network protocols and traffic conditions will allow

    • On the client side, the bytes are collected in a client application buffer.

        - once the number of bytes in this buffer exceeds a predetermined threshold, the client application begins playback
        
        — specifically, the streaming video application periodically grabs video frames from the client application buffer, decompresses the frames, and displays them on the user’s screen

    • A major disadvantage with this method is that all clients receive the same encoding of the video, dispite the large variations in the amount of bandwidth available to a client.

    • Dynamic Adaptive Streaming over HTTP (DASH) allows video to be encoded into several different bitrates and quality levels corresponding to the available network bandwidth.

    • With DASH, each video version is stored in the HTTP server, each with a different URL.

        - the HTTP server also has a manifest file, which provides a URL for each version along with its bit rate

## 2.5.3: Content Distribution Networks

    • In order to meet the challenge of distributing massive amounts of video data to users distributed around the world, almost all major video-streaming companies make use of Content Distribution Networks (CDNs).

    • CDNs typically adopt one of two different server placement philosophies:

        (1) Enter Deep: CDN servers are placed directly inside ISPs near users to improve speed and latency, but this creates massive operational complexity because there are thousands of small server locations to manage

        (2) Bring Home: build large clusters at a smaller number in Internet Exchange Points (IXPs)

    • Once its clusters are in place, the CDN replicates content across its clusters. 

### CDN Operation

    • Most CDNs take advantage of DNS to intercept and redirect requests (a simplified example of this iteraction is shown below)

        (1) User visits NetCinema webpage and clicks the video link

        (2) User’s computer asks for video.netcinema.com and sends a DNS query

        (3) NetCinema’s authoritative DNS intercepts, sees “video” in the hostname and returns a hostname in KingCDN’s domain (e.g., a1105.kingcdn.com) instead of an IP

        (4) KingCDN’s DNS resolves the hostname, LDNS (Local DNS) asks KingCDN DNS: “What IP should the client use?” and KingCDN returns the IP of the best content server for this user

        (5) LDNS sends IP to user’s computer and now the client knows which CDN server to contact

        (6) Client downloads the video, opens TCP connection to CDN server, issues HTTP GET for video

            - if using DASH, server first sends a manifest file and client picks video chunks dynamically based on bandwidth

### Cluster Selection Strategies

    • Assigning clients to the geographically closest CDN cluster is simple and works okay for many users, but it can perform poorly because:

        (1) physical closeness doesn’t always mean shortest/faster network path

        (2) some clients use remote DNS servers, misleading the CDN

        (3) internet conditions change over time, but this strategy doesn’t adapt

    • In order to determine the best cluster for a client based on the current traffic conditions, CDNs can instead perform periodic real-time measurements of delay and loss performance between their clusters and clients by periodically sending probes (for example, ping messages or DNS queries) to all of the LDNSs around the world.

### CDN Caching

    • CDNs don’t store all content at every server location because that would be too costly and require too much storage.

        - instead, each server location stores only a subset of content

        - all the servers at a location working together like this are called a cache

    • A CDN has at its disposal two fundamental strategies for placing video files (and other content) in its caches: push and pull

        - in the push strategy, the CDN forecasts which videos will be in greatest demand at each of its locations and then sends to each of its locations the videos that it expects to be in highest demand for those locations

        - in the pull strategy, the user gets redirected to a nearby cache in the CDN - if that cache has a copy of the video, then the cache streams the video to the user

            - if there is a cache miss, the cache first retrieves the video from one the CDN’s central servers, stores the video in the local cache, and streams the video to the requesting user
