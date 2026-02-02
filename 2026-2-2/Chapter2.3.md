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
