# FT_SSL
> A simple **openssl** from scratch.

## Summary:
- [Theory](#theory)
	- [Definitions](#definitions)
	- [More details](#more-details)
	- [The method](#the-method)
- [In practice](#in-practice)

## Theory
### Definitions
*Cryptographic hash function* : it is a mathematical algorithm that maps data of arbitrary size to a bit string of a fixed size.  
*Md5* (Message Digest version 5): hash function producing a 128-bit hash value.  
*Sha256* (Secure Hash Algorithm 256-bit) : hash function producing a 256-bit hash value, whose digest is eight 32-bit words.  
*Message digest* :  
*Round* : in cryptograhy, it refers to one iteration of the iternal transformation process applied by the algorithm.  

### More details
#### Md5:
Md5 was designed in 1991, specified in 1992, to replace an earlier hash function md4.  
It was used as cryptographic hash function but it has been found that it suffers from multiple vulnerabilities. They are mainly collisions. For example in 2012, the Flame malware could fake a Microsoft digital signature. It remains suitable to use for non cryptographic uses and might be prefered due to its lower computational requirements.

#### Sha256:
Sha256 is part of the SHA-2 algorithms family containing 6 hash functions : SHA-224, SHA-256, SHA-384, SHA-512, SHA-512/224 and SHA-512/256. SHA-2 were designed by the NSA and published in 2001.
The SHA-2 showed some weakness but a hash made with 64 rounds of sha-256 never been cracked.

### The method
#### Md5:
*From RFC-1321*.  
Note: RFC-1321 *method and implementation order look like differing* but they are not. The method describe the theory and requirements to perfom the algorithm. The algorithm process data by 64-byte chunck : the length appended at the end is processed last but is still append at the end of the input and the padding.  
**Step 1 : Bit padding**  
The message should be padded so that its length is 64 bits shorter than a multiple of 512 (bits).  

**Step 2 : Append length**  
The 64 bits missing are filled filled with the 64-bit value of the total length of the message **before** padding.  

**Step 3 : Buffer initialization**  
- Create **4 buffers** (A, B, C and D), each with a bit length of 32. They will be initiated as follows:
	- A: 0x01234567
	- B: 0x89ABCDEF
	- C: 0xFEDCBA89
	- D: 0x76543210  
	*Note that these values are low-order bytes first.* 

- **step 4 : Message processing**  
	- Define **4 functions** :
		- F(X, Y, Z) = (X & Y) | (!X & Z)
		- G(X, Y, Z) = (X & Y) | (X & !Z)
		- H(X, Y, Z) = (X ^ Y ^ Z)
		- I(X, Y, Z) = Y ^ (X | !Z)
	- Create a 64 constant elements table. These 64 values are defined following a formula as follows:  
		When passed to sin() function i must be in radian.  
	```
	for (i = 0; i < 64; i++):  
		T[i] = 2^32 * abs(sin(i))  
	```
	- **step 5 : Process 16-word blocks**
		The documentation simply gives pseudo code here. See RFC Page 4-5.  
		Looping through all 16-word blocks in message, a variable X is set from each 16-word block from the message.  
			Using the value of X and the table T and the variables A, B, C and D, 4 rounds are peformed running bit wise operations.  
			The new values assigned to A, B, C and D are added to their old value saved in AA, BB, CC and DD.  
	- **step 6 : Output**
		The algorithm produces 4 variables A, B, C and D. The display starts from the low-order byte of A and end with the high-order byte of D.
	#### Sha256
	*From RFC-6234 for sha256*.  
	The method is basically the same than md5 with the exception of the output length and the constants used.  