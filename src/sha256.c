#include "../inc/main.h"

static inline uint32_t SHA_Ch(uint32_t x, uint32_t y, uint32_t z) { return((x & y) | (~x & z)); }
static inline uint32_t SHA_Maj(uint32_t x, uint32_t y, uint32_t z) { return (x & y) ^ (x & z) ^ (y & z); }

static inline uint32_t sha256_SIGMA0(uint32_t word) { return (rotate_right(2 , word) ^ rotate_right(13, word) ^ rotate_right(22, word)); }
static inline uint32_t sha256_SIGMA1(uint32_t word) { return (rotate_right(6 , word) ^ rotate_right(11, word) ^ rotate_right(25, word)); }
static inline uint32_t sha256_sigma0(uint32_t word) { return (rotate_right(7 , word) ^ rotate_right(18, word) ^ shift_right(3 , word)); }
static inline uint32_t sha256_sigma1(uint32_t word) { return (rotate_right(17, word) ^ rotate_right(19, word) ^ shift_right(10, word)); }

/*
 * add "length" to the length
 */
static uint32_t	addTemp = 0;
// #define SHA224_256AddLength(context, length)
// (addTemp = (context)->Length_Low, (context)->Corrupted = (((context)->Length_Low += (length)) < addTemp) && (++(context)->Length_High == 0) ? 1 : 0)
static inline uint32_t	SHA256AddLength(t_SHA256_CTX *context, uint32_t length)
{
	addTemp = context->Length_Low;
	return (context->Corrupted = (((context->Length_Low += length) < addTemp) && (++context->Length_High == 0)) ? shaInputTooLong : context->Corrupted);
}

/*
 * SHA256Reset
 *
 * Description:
 *   This function will initialize the t_SHA256_CTX in preparation
 *   for computing a new SHA256 message digest.
 *
 * Parameters:
 *   context: [in/out]
 *     The context to reset.
 *
 * Returns:
 *   sha Error Code.
 */
int	SHA256Reset(t_SHA256_CTX *context)
{
	uint32_t SHA256_H0[SHA256_HSSZ/4] = {
		0x6A09E667, 0xBB67AE85, 0x3C6EF372, 0xA54FF53A,
		0x510E527F, 0x9B05688C, 0x1F83D9AB, 0x5BE0CD19 };
	if (!context)
		return (shaNull);

	context->Length_Low = 0;
	context->Length_High = 0;

	context->Message_Block_Index = 0;

	context->Intermediate_Hash[0] = SHA256_H0[0];
	context->Intermediate_Hash[1] = SHA256_H0[1];
	context->Intermediate_Hash[2] = SHA256_H0[2];
	context->Intermediate_Hash[3] = SHA256_H0[3];
	context->Intermediate_Hash[4] = SHA256_H0[4];
	context->Intermediate_Hash[5] = SHA256_H0[5];
	context->Intermediate_Hash[6] = SHA256_H0[6];
	context->Intermediate_Hash[7] = SHA256_H0[7];

	context->Computed  = 0;
	context->Corrupted = 0;

	return (shaSuccess);
}

static void	SHA256ProcessMessageBlock(t_SHA256_CTX *context)
{
	/* Constants defined in FIPS-180-2, section 4.2.2 */
	const uint32_t K[64] = {
		0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b,
		0x59f111f1, 0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01,
		0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7,
		0xc19bf174, 0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
		0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da, 0x983e5152,
		0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147,
		0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc,
		0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
		0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819,
		0xd6990624, 0xf40e3585, 0x106aa070, 0x19a4c116, 0x1e376c08,
		0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f,
		0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
		0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2 };
	int32_t		t = 0;
	int32_t 	t4 = 0 ;                   /* Loop counter */
	uint32_t	temp1 = 0;
	uint32_t	temp2 = 0;            /* Temporary word value */
	uint32_t	W[64] = {0};                   /* Word sequence */
	uint32_t	A = 0, B = 0, C = 0, D = 0, E = 0, F = 0, G = 0, H = 0;  /* Word buffers */

	/*
	* Initialize the first 16 words in the array W
	*/
	for (t = t4 = 0; t < 16; t++, t4 += 4)
		W[t] = (((uint32_t)context->Message_Block[t4]) << 24)
			| (((uint32_t)context->Message_Block[t4 + 1]) << 16)
			| (((uint32_t)context->Message_Block[t4 + 2]) << 8)
			| (((uint32_t)context->Message_Block[t4 + 3]));

	for (t = 16; t < 64; t++)
		W[t] = sha256_sigma1(W[t-2]) + W[t-7] + sha256_sigma0(W[t - 15]) + W[t - 16];

	A = context->Intermediate_Hash[0];
	B = context->Intermediate_Hash[1];
	C = context->Intermediate_Hash[2];
	D = context->Intermediate_Hash[3];
	E = context->Intermediate_Hash[4];
	F = context->Intermediate_Hash[5];
	G = context->Intermediate_Hash[6];
	H = context->Intermediate_Hash[7];

	for (t = 0; t < 64; t++)
	{
		temp1 = H + sha256_SIGMA1(E) + SHA_Ch(E,F,G) + K[t] + W[t];
		temp2 = sha256_SIGMA0(A) + SHA_Maj(A,B,C);
		H = G;
		G = F;
		F = E;
		E = D + temp1;
		D = C;
		C = B;
		B = A;
		A = temp1 + temp2;
	}
	context->Intermediate_Hash[0] += A;
	context->Intermediate_Hash[1] += B;
	context->Intermediate_Hash[2] += C;
	context->Intermediate_Hash[3] += D;
	context->Intermediate_Hash[4] += E;
	context->Intermediate_Hash[5] += F;
	context->Intermediate_Hash[6] += G;
	context->Intermediate_Hash[7] += H;

	context->Message_Block_Index = 0;
}

static void SHA256PadMessage(t_SHA256_CTX *context, uint8_t Pad_Byte)
{
	/*
	* Check to see if the current message block is too small to hold
	* the initial padding bits and length. If so, we will pad the
	* block, process it, and then continue padding into a second
	* block.
	*/
	if (context->Message_Block_Index >= (SHA256_BLSZ - 8))
	{
		context->Message_Block[context->Message_Block_Index++] = Pad_Byte;
		while (context->Message_Block_Index < SHA256_BLSZ)
			context->Message_Block[context->Message_Block_Index++] = 0;
		SHA256ProcessMessageBlock(context);
	}
	else
		context->Message_Block[context->Message_Block_Index++] = Pad_Byte;

	while (context->Message_Block_Index < (SHA256_BLSZ - 8))
		context->Message_Block[context->Message_Block_Index++] = 0;

	/*
	* Store the message length as the last 8 octets
	*/
	context->Message_Block[56] = (uint8_t)(context->Length_High >> 24);
	context->Message_Block[57] = (uint8_t)(context->Length_High >> 16);
	context->Message_Block[58] = (uint8_t)(context->Length_High >> 8);
	context->Message_Block[59] = (uint8_t)(context->Length_High);
	context->Message_Block[60] = (uint8_t)(context->Length_Low >> 24);
	context->Message_Block[61] = (uint8_t)(context->Length_Low >> 16);
	context->Message_Block[62] = (uint8_t)(context->Length_Low >> 8);
	context->Message_Block[63] = (uint8_t)(context->Length_Low);

	SHA256ProcessMessageBlock(context);
}

/*
 * SHA256Input
 *
 * Description:
 *   This function accepts an array of octets as the next portion
 *   of the message.
 *
 * Parameters:
 *   context: [in/out]
 *     The SHA context to update
 *   message_array: [in]
 *     An array of characters representing the next portion of
 *     the message.
 *   length: [in]
 *     The length of the message in message_array
 *
 * Returns:
 *   sha Error Code.
 */
int		SHA256Input(t_SHA256_CTX *context, const uint8_t *message_array, unsigned int length)
{
	if (!context)
		return shaNull;
	if (!length)
		return shaSuccess;
	if (!message_array)
		return shaNull;
	if (context->Computed)
		return context->Corrupted = shaStateError;
	if (context->Corrupted)
		return context->Corrupted;
	
	while (length--) {
		context->Message_Block[context->Message_Block_Index++] = *message_array;
		if ((SHA256AddLength(context, 8) == shaSuccess) && (context->Message_Block_Index == SHA256_BLSZ))
			SHA256ProcessMessageBlock(context);
		message_array++;
	}
	return context->Corrupted;
}

static void SHA256Finalize(t_SHA256_CTX *context, uint8_t Pad_Byte)
{
	SHA256PadMessage(context, Pad_Byte);
	/* message may be sensitive, so clear it out */
	for (uint8_t i = 0; i < SHA256_BLSZ; ++i)
		context->Message_Block[i] = 0;
	context->Length_Low = 0;  /* and clear length */
	context->Length_High = 0;
	context->Computed = 1;
}

/*
 * SHA256FinalBits
 *
 * Description:
 *   This function will add in any final bits of the message.
 *
 * Parameters:
 *   context: [in/out]
 *     The SHA context to update
 *   message_bits: [in]
 *     The final bits of the message, in the upper portion of the
 *     byte. (Use 0b###00000 instead of 0b00000### to input the
 *     three bits ###.)
 *   length: [in]
 *     The number of bits in message_bits, between 1 and 7.
 *
 * Returns:
 *   sha Error Code.
 */
int		SHA256FinalBits(t_SHA256_CTX *context, const uint8_t message_bits, unsigned int length)
{
	static uint8_t masks[8] = {
		/* 0 0b00000000 */ 0x00, /* 1 0b10000000 */ 0x80,
		/* 2 0b11000000 */ 0xC0, /* 3 0b11100000 */ 0xE0,
		/* 4 0b11110000 */ 0xF0, /* 5 0b11111000 */ 0xF8,
		/* 6 0b11111100 */ 0xFC, /* 7 0b11111110 */ 0xFE
	};
	static uint8_t markbit[8] = {
		/* 0 0b10000000 */ 0x80, /* 1 0b01000000 */ 0x40,
		/* 2 0b00100000 */ 0x20, /* 3 0b00010000 */ 0x10,
		/* 4 0b00001000 */ 0x08, /* 5 0b00000100 */ 0x04,
		/* 6 0b00000010 */ 0x02, /* 7 0b00000001 */ 0x01
	};

	if (!context)
		return (shaNull);
	if (!length)
		return (shaSuccess);
	if (context->Corrupted)
		return (context->Corrupted);
	if (context->Computed)
		return (context->Corrupted = shaStateError);
	if (length >= 8)
		return (context->Corrupted = shaBadParam);
	
	SHA256AddLength(context, length);
	SHA256Finalize(context, (uint8_t)((message_bits & masks[length]) | markbit[length]));
	return (context->Corrupted);
}

/*
 * SHA256Result
 *
 * Description:
 *   This function will return the 256-bit message
 *   digest into the Message_Digest array provided by the caller.
 *   NOTE: The first octet of hash is stored in the 0th element,
 *      the last octet of hash in the 32nd element.
 *
 * Parameters:
 *   context: [in/out]
 *     The context to use to calculate the SHA hash.
 *   Message_Digest: [out]
 *     Where the digest is returned.
 *
 * Returns:
 *   sha Error Code.
 */
uint8_t	SHA256Result(t_SHA256_CTX *context, uint8_t *Message_Digest)
{
	if (!context)
		return (shaNull);
	if (!Message_Digest)
		return (shaNull);
	if (context->Corrupted)
		return (context->Corrupted);
	if (!context->Computed)
		SHA256Finalize(context, 0x80);
	
	for (uint8_t i = 0; i < SHA256_HSSZ; ++i)
		Message_Digest[i] = (uint8_t)(context->Intermediate_Hash[i >> 2] >> 8 * (3 - (i & 0x03)));
	return (shaSuccess);
}

bool	SHARoutine(const t_data *data, const char *runner, const char *to_hash)
{
	t_SHA256_CTX	context = {0};
	uint8_t	digest[SHA256_HSSZ] = {0};
	
	if (SHA256Reset(&context))
		ret_err_mess("ft_ssl: sha256: Reset err");
	else if (SHA256Input(&context, (const uint8_t *)to_hash, strlen(to_hash)))
		ret_err_mess("ft_ssl: sha256: Input err");
	else if (SHA256Result(&context, (uint8_t *)digest))
		ret_err_mess("ft_ssl: sha256: Result err");
	dgst_display(digest, data->options, runner, to_hash, "sha256", SHA256_HSSZ);
	return (EXIT_SUCCESS);
}

bool	SHAMain(t_data *data)
{
	char	**runner = data->inputs;
	char	*to_hash = NULL;
	
	if (data->options & DECODE)
		exit_err_mess("ft_ssl: SHA256: '-d' option invalide option.", data, EX_USAGE);
	while (*runner)
	{
		if (SHARoutine(data, *runner, to_hash))
			exit_err_code(data, EX_SOFTWARE);
		++runner;
	}
	return (EXIT_SUCCESS);
}