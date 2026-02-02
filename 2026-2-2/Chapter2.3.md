# 2.3: Electronic Mail in the Internet

    • User agents allow users to read, reply to, forward, save, and compose messages.

        - e.g., Microsoft Outlook, Apple mail, web-based Gmail, etc.

    • Today, many users send emails to their mail servers and receive emails from their mail servers via HTTP-based user agents such as gmail and Yahoo! mail

        - see figure 2.12 for a pictoral representation of this

    • Mail servers form the core of the e-mail infrastructure; each recipient has a mailbox located in one of the mail servers. 

    • A flow chart of the journey for electronic mail is as follows

        senders's mail agent  -->  sender's mail server  -->  recepient's mail server  -->  recepients's mail box

    • If a sender's server cannot deliver mail to a recepient's mail server, the sender's server holds the message in a message queue and attempts to transfer the message later (usually every 30 minutes)

        - after several days of failed attempts, the sender's mail server simply removes the email

# 2.3.1: SMTP

    • SMTP is the principal application-layer protocol for Internet electronic mail.

        - has a client side and server side

    • SMPT is a legacy technology that has many archaic features.

        - restricts the body (not just the headers) of all mail messages to simple 7-bit ASCII before transmission, which then must be decoded back to binary after SMTP transport

    • SMTP does not normally use intermediate mail servers for sending mail, even when the two mail servers are located at opposite ends of the world. 

# 2.3.2: Mail Message Formats

    • When an e-mail message is sent from one person to another, a header containing peripheral information precedes the body of the message itself.

        - the header and body are separated by a blank line (CRLF)

    • It is important to note that these header lines are different from the SMTP commands we studied in Section 2.3.1, which were part of the handshaking commands

    • A typical message header looks like this:

        From: sjaboud@gmail.com
        To: sjaboud@gmail.com
        Subject: This is a test

# 2.3.3: Mail Access Protocols
    
    • Typically the sender’s user agent does not dialogue directly with the recipient’s mail server:
        
        - instead, as shown in Figure 2.14, the sender's user agent uses SMTP or HTTP to deliver the e-mail message into their mail server, then the sender's mail server uses SMTP (as an SMTP client) to relay the e-mail message to recipient's mail server

    • This two-step procedure is necessary for recourse in the event of a failed delivery:

        - by having the sender first deposit the e-mail in thier own mail server, the mail server can repeatedly try to send the message to to the recipient's mail server, say every 30 minutes, until the recipient's mail server becomes operational

    • Recall that SMTP is a push protocol, meaning a recipient can't use SMTP to obtain and read messages from a mail server

        - if the recipient is using a web-based email or a smartphone app, then the user agent will use HTTP to retrieve their email

        - the alternative method, typically used with mail clients such as Microsoft Outlook, is to use the Internet Mail Access Protocol (IMAP)