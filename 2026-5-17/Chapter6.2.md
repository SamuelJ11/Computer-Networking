# 6.2: Error Detection and Correction Techniques

    • An important clarification that must be made is that error detection and correction techniques allow the receiver to sometimes, but not always, detect that bit errors have occurred.

## 6.2.1: Parity Checks

    • The simplest form of error detection is the use of a single parity bit:

        - in an even parity scheme, the sender simply includes one additional bit and chooses its value such that the total number of 1s in the d + 1 bits (the original information plus a parity bit) is even

        * odd parity schemes work analogously

        - if the reciever detects an odd number of 1-valued bits in an even parity scheme, the reciever onws that at least one bit error has occured

        * if an even number of bit errors occur in this scheme, this results in an undetected error

    • Referencing figure 6.5, the 'd' parity bits of a datagram 'D' are split into an i x j matrix:

        - a parity value is comptuted for each row and for each column (1 additional parity row (i + 1) and column (j + 1))

        - the resulting i + j + 1 parity bits comprise the link-layer frame's error-detection bits

        - suppose that a single bit error occurs in the original 'd' bits of information. With this two-dimensional parity scheme, the parity of both the column and the row containing the flipped bit will be in error

            * the receiver can thus not only detect the fact that a single bit error has occurred, but can use the column and row indices of the column and row with parity errors to actually identify the bit that was corrupted and correct that error

    • To see how this works, lets recap a couple things and work through an example.

        - recall that the golden rule of even parity is the total number of 1s in any row or column (including the parity bit itself) must be an even number
  
        * this can be verified by looking at the "no errors" matrix

        - looking at the second matrix, we see that a single bit (D₂,₂) has been flipped from 1 to zero:

            Original Row:  1  1  1  1  0 │ 0   (Four 1s total -> Even)
            Corrupted Row: 1  0  1  1  0 │ 0   (Three 1s total -> Odd!)
                              ▲
                          Flipped bit!

        - when the reciever counts the 1s in the corrupted row, it counts three 1s, and since 3 is odd, the receiver throws up a flag: "Error in Row 2!"
  
        - next, the reciever checks the corresponding column going straight down; because that bit flipped, this column now also has an odd number of 1s and the receiver throws up another flag: "Error in Column 2!"

        - finally, the receiver looks at the exact intersection of row 2, Column 2 and flipls the 1 back to 0

    • The ability of the receiver to both detect and correct errors is known as forward error correction (FEC):

        - FEC techniques are valuable because they can decrease the number of sender retransmissions required

## 6.2.2: Checksumming Methods

    • In this mode, the 'd' bits of data from before are treated as a sequence of k-bit integers;

        - one simple checksumming method is to simply sum these  integers and use the resulting sum as the error-detection bits

        * note that internet checksum is based on this approach, and this process was discussed in section 3.3.2

    • In the TCP and UDP protocols, the Internet checksum is computed over all fields (header and data fields included), but In IP, the checksum is computed only over the IP header.

    • Checksumming methods provide relatively weak protection against errors as compared with cyclic redundancy check.

## 6.2.3: Cyclic Redundancy Check (CRC)

    • CRC codes are also known as polynomial codes, since it is possible to view the bit string to be sent as a polynomial whose coefficients are the 0 and 1 values in the bit string, with operations on the bit string interpreted as polynomial arithmetic.

        * you've seen this before as the underlying math in Cyclic Redundancy Checks (CRCs) is exactly the same algebraic structure used in AES (both operate inside Galois fields, aka GF(2ⁿ))

        - for example, if a link-layer frame wants to send the data payload as 100111, it is interpreted as 

            1x⁵ + 0x⁴ + 0x³ + 1x² + 1x¹ + 1x⁰ = x⁵ + x² + x + 1

        * since we are working in GF(2ⁿ), addition or subtraction is simply XOR (⊕)

    • CRC codes operate as follows:

        (1) the sender and reciever agree on an r + 1 bit pattern, known as a generator (which we denote as G, of which the most significant bit must be 1)

        (2) for a given d-bit piece of data 'D', the sender will choose 'r' additional bits (which we denote collectively as R) and append them to 'D' such that the resulting d + r bit pattern (D * 2ʳ ⊕ R) is exactly divisible by 'G'

            * the sender calculates 'R' as the remainder when dividing D * 2ʳ by G
            * the sender computes D * 2ʳ ⊕ R = nG
            * here the data payload (D) is being shfited over to make room for the CRC bits (R) 
  
        (3) on the reciever side, the reciever divides the d + r bitstring (D * 2ʳ ⊕ R) by 'G', and if the result is non-zero, an error has occured

    • Lets look at an example of the previous explanation to make this more concrete:

        - suppose we have D = 101110, d = 6, G = 1001 and r = 3, so the sender computes R = (D * 2³) / G = 101110000 / 1001

        * the steps for the division are shown below

        (1) since the divisor is 4 bits, we look at the first four bits of the dividend (1011)

            - clearly the divisor "fits" in 1011, so we XOR them:
  
                1011 ⊕ 1001 = 0010 -> 0010 

            * if the 4-bit chunk starts with a 1, the generator fits and we append a 1 to the quotient (Q)
  
                current quotient = 1

        (2) the next bit in the dividend is 1, so we append it to the result we obtained in the previous step

                0010 -> 0101

            * chunk starts with a 0, so our next quotient bit is 0
  
                current quotient = 10

        (3) since the divisor does not fit in 101, we drop the leading zero and append the next (sixth) bit of the dividend

                0101 -> 1010

        * repeat steps 1 - 3 until done, so we have

                1010 ⊕ 1001 = 0011 (divide)
                current quotient = 101

                0011 -> 0110 (append seventh dividend bit)
                current quotient = 1010

                0110 -> 1100 (append eighth dividend bit)
                1100 ⊕ 1001 = 0101 (divide)
                current quotient = 10101

                0101 -> 1010 (append last dividend bit)
                1010 ⊕ 1001 = 0011 (divide)
                final quotient = 101011
                *final remainder = 0011

        (4) since our remainder must be r = 3 bits long, we drop the first leading zero for the final remainder of 011

        (5) thus the 9 bits transmitted are 101110(011)

        - we can verify our answer by checking that D * 2ʳ = (Q * G) ⊕ R:

            Q * G = 101011 * 001001 = 101110111

            000101011
            000001001
           *_________
            000101011
            000000000
            000000000
            101011000
           ⊕________
            101110011

            101110011 ⊕ 000000011 = 101110000 = D * 2ʳ = L₃(101110) 

    • International standards have been defined for 8, 12, 16, and 32-bit generators 'G':

        - the  CRC-32 standard, which has been adopted in a number of link-level IEEE protocols, uses a generator of

            100000100110000010001110110110111

        - each of the CRC standards can detect burst errors of 'r' bits or less, and any odd number of bit errors