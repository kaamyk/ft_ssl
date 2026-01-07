#include "../inc/main.h"

static inline uint32_t f(uint32_t x, uint32_t y, uint32_t z){ return((x & y) | (~x & z)); }
static inline uint32_t g(uint32_t x, uint32_t y, uint32_t z){ return((x & z) | (y & ~z)); }
static inline uint32_t h(uint32_t x, uint32_t y, uint32_t z){ return (x ^ y ^ z); }
static inline uint32_t i(uint32_t x, uint32_t y, uint32_t z){ return (y ^ (x | ~z)); }

static inline uint32_t ff(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac)
{
	a += f(b, c, d) + x + ac;
	a = rotate_left(s, a);
	a += b;
	return a;
}

static inline uint32_t gg(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac)
{
	a += g(b, c, d) + x + ac;
	a = rotate_left(s, a);
	a += b;
	return a;
}

static inline uint32_t hh(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac)
{
	a += h(b, c, d) + x + ac;
	a = rotate_left(s, a);
	a += b;
	return a;
}

static inline uint32_t ii(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac)
{
	a += i(b, c, d) + x + ac;
	a = rotate_left(s, a);
	a += b;
	return a;
}

uint8_t	*MD5memcpy(uint8_t *dest, const uint8_t *src, const uint32_t len)
{
	for (uint32_t i = 0; i < len; i++)
	{
		dest[i] = src[i];
	}
	return (dest);
}

void	MD5memset(uint8_t *output, const uint8_t value, const uint32_t len)
{
	for (uint32_t i = 0; i < len; i++)
	{
		output[i] = value;
	}
}

void	MD5Init(t_MD5_CTX *context)
{
	context->count[0] = 0;
	context->count[1] = 0;
	context->state[0] = 0x67452301;
	context->state[1] = 0xefcdab89;
	context->state[2] = 0x98badcfe;
	context->state[3] = 0x10325476;
}

void	MD5Transform(uint32_t state[4], uint8_t block[64])
{
	uint32_t	a = state[0];
	uint32_t	b = state[1];
	uint32_t	c = state[2];
	uint32_t	d = state[3];
	uint32_t	x[16] = {0};
	
	decode(x, block, 64);
	
	/* Round 1 */
	a = ff(a, b, c, d, x[ 0],  7, 0xd76aa478); /* 1 */
	d = ff(d, a, b, c, x[ 1], 12, 0xe8c7b756); /* 2 */
	c = ff(c, d, a, b, x[ 2], 17, 0x242070db); /* 3 */
	b = ff(b, c, d, a, x[ 3], 22, 0xc1bdceee); /* 4 */
	a = ff(a, b, c, d, x[ 4],  7, 0xf57c0faf); /* 5 */
	d = ff(d, a, b, c, x[ 5], 12, 0x4787c62a); /* 6 */
	c = ff(c, d, a, b, x[ 6], 17, 0xa8304613); /* 7 */
	b = ff(b, c, d, a, x[ 7], 22, 0xfd469501); /* 8 */
	a = ff(a, b, c, d, x[ 8],  7, 0x698098d8); /* 9 */
	d = ff(d, a, b, c, x[ 9], 12, 0x8b44f7af); /* 10 */
	c = ff(c, d, a, b, x[10], 17, 0xffff5bb1); /* 11 */
	b = ff(b, c, d, a, x[11], 22, 0x895cd7be); /* 12 */
	a = ff(a, b, c, d, x[12],  7, 0x6b901122); /* 13 */
	d = ff(d, a, b, c, x[13], 12, 0xfd987193); /* 14 */
	c = ff(c, d, a, b, x[14], 17, 0xa679438e); /* 15 */
	b = ff(b, c, d, a, x[15], 22, 0x49b40821); /* 16 */
	
	/* Round 2 */
	a = gg(a, b, c, d, x[ 1],  5, 0xf61e2562); /* 17 */
	d = gg(d, a, b, c, x[ 6],  9, 0xc040b340); /* 18 */
	c = gg(c, d, a, b, x[11], 14, 0x265e5a51); /* 19 */
	b = gg(b, c, d, a, x[ 0], 20, 0xe9b6c7aa); /* 20 */
	a = gg(a, b, c, d, x[ 5],  5, 0xd62f105d); /* 21 */
	d = gg(d, a, b, c, x[10],  9,  0x2441453); /* 22 */
	c = gg(c, d, a, b, x[15], 14, 0xd8a1e681); /* 23 */
	b = gg(b, c, d, a, x[ 4], 20, 0xe7d3fbc8); /* 24 */
	a = gg(a, b, c, d, x[ 9],  5, 0x21e1cde6); /* 25 */
	d = gg(d, a, b, c, x[14],  9, 0xc33707d6); /* 26 */
	c = gg(c, d, a, b, x[ 3], 14, 0xf4d50d87); /* 27 */
	b = gg(b, c, d, a, x[ 8], 20, 0x455a14ed); /* 28 */
	a = gg(a, b, c, d, x[13],  5, 0xa9e3e905); /* 29 */
	d = gg(d, a, b, c, x[ 2],  9, 0xfcefa3f8); /* 30 */
	c = gg(c, d, a, b, x[ 7], 14, 0x676f02d9); /* 31 */
	b = gg(b, c, d, a, x[12], 20, 0x8d2a4c8a); /* 32 */
	
	/* Round 3 */
	a = hh(a, b, c, d, x[ 5],  4, 0xfffa3942); /* 33 */
	d = hh(d, a, b, c, x[ 8], 11, 0x8771f681); /* 34 */
	c = hh(c, d, a, b, x[11], 16, 0x6d9d6122); /* 35 */
	b = hh(b, c, d, a, x[14], 23, 0xfde5380c); /* 36 */
	a = hh(a, b, c, d, x[ 1],  4, 0xa4beea44); /* 37 */
	d = hh(d, a, b, c, x[ 4], 11, 0x4bdecfa9); /* 38 */
	c = hh(c, d, a, b, x[ 7], 16, 0xf6bb4b60); /* 39 */
	b = hh(b, c, d, a, x[10], 23, 0xbebfbc70); /* 40 */
	a = hh(a, b, c, d, x[13],  4, 0x289b7ec6); /* 41 */
	d = hh(d, a, b, c, x[ 0], 11, 0xeaa127fa); /* 42 */
	c = hh(c, d, a, b, x[ 3], 16, 0xd4ef3085); /* 43 */
	b = hh(b, c, d, a, x[ 6], 23,  0x4881d05); /* 44 */
	a = hh(a, b, c, d, x[ 9],  4, 0xd9d4d039); /* 45 */
	d = hh(d, a, b, c, x[12], 11, 0xe6db99e5); /* 46 */
	c = hh(c, d, a, b, x[15], 16, 0x1fa27cf8); /* 47 */
	b = hh(b, c, d, a, x[ 2], 23, 0xc4ac5665); /* 48 */
	
	/* Round 4 */
	a = ii(a, b, c, d, x[ 0],  6, 0xf4292244); /* 49 */
	d = ii(d, a, b, c, x[ 7], 10, 0x432aff97); /* 50 */
	c = ii(c, d, a, b, x[14], 15, 0xab9423a7); /* 51 */
	b = ii(b, c, d, a, x[ 5], 21, 0xfc93a039); /* 52 */
	a = ii(a, b, c, d, x[12],  6, 0x655b59c3); /* 53 */
	d = ii(d, a, b, c, x[ 3], 10, 0x8f0ccc92); /* 54 */
	c = ii(c, d, a, b, x[10], 15, 0xffeff47d); /* 55 */
	b = ii(b, c, d, a, x[ 1], 21, 0x85845dd1); /* 56 */
	a = ii(a, b, c, d, x[ 8],  6, 0x6fa87e4f); /* 57 */
	d = ii(d, a, b, c, x[15], 10, 0xfe2ce6e0); /* 58 */
	c = ii(c, d, a, b, x[ 6], 15, 0xa3014314); /* 59 */
	b = ii(b, c, d, a, x[13], 21, 0x4e0811a1); /* 60 */
	a = ii(a, b, c, d, x[ 4],  6, 0xf7537e82); /* 61 */
	d = ii(d, a, b, c, x[11], 10, 0xbd3af235); /* 62 */
	c = ii(c, d, a, b, x[ 2], 15, 0x2ad7d2bb); /* 63 */
	b = ii(b, c, d, a, x[ 9], 21, 0xeb86d391); /* 64 */
	
	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;
	
	/* Zeroize sensitive information.*/
	MD5memset((uint8_t *)x, 0, sizeof (x));
}

void MD5Update(t_MD5_CTX *context, uint8_t *input, unsigned int inputLen)
{
	uint32_t	i = 0;
	uint32_t	partLen = 0;
	uint64_t	index = 0;
	
	/* Compute number of bytes mod 64 */
	index = (uint32_t)((context->count[0] >> 3) & 0x3F);
	/* Update number of bits */
	if ((context->count[0] += ((uint32_t)inputLen << 3)) < ((uint32_t)inputLen << 3))
		context->count[1]++;
	context->count[1] += ((uint32_t)inputLen >> 29);
	partLen = 64 - index;
	/* Transform as many times as possible.*/
	if (inputLen >= partLen)
	{
		MD5memcpy(&context->buffer[index], input, partLen);
		MD5Transform (context->state, context->buffer);
		for (i = partLen; i + 63 < inputLen; i += 64)
			MD5Transform (context->state, &input[i]);
		index = 0;
	}
	else
		 i = 0;
	
	/* Buffer remaining input */
	MD5memcpy(&context->buffer[index], &input[i], inputLen-i);
}

void	MD5Final(uint8_t digest [16], t_MD5_CTX *context)
{
	uint8_t		padding[64] = {0x80, 0x0};
	uint8_t		bits[8];
	uint32_t	index = 0;
	uint32_t	padLen = 0;
	
	//	Save number of bits
	encode(bits, context->count, 8);
	
	//	Pad out to 56 mod 64
	index = (uint32_t)((context->count[0] >> 3) & 0x3f);
	if (index < 56)
		padLen = 56 - index;	// 64 - 8
	else
		padLen = 120 - index;	// 128 - 8
	MD5Update(context, padding, padLen);
	MD5Update(context, bits, 8);
	encode(digest, context->state, 16);
	MD5memset((uint8_t *)context, 0, sizeof(*context));
}

bool	MDMain(t_data *data)
{
	char		**runner = data->inputs;
	char		*to_hash = NULL;
	uint8_t		digest[MD5_HSSZ] = {0};
	t_MD5_CTX	context = {0};
	
	while (*runner)
	{
		if (data->options & STRING)
		{
			to_hash = *runner;
			data->options &= ~(STRING);
		}
		else
		{
			to_hash = file_to_str(*runner);
			printf("to_hash == [%s]\n", to_hash);
			if (to_hash == NULL)
				exit(EX_OSERR);
		}
		MD5Init(&context);
		MD5Update(&context, (uint8_t *)to_hash, strlen(to_hash));
		MD5Final(digest, &context);
		if (MDDisplay(digest, data->options, *runner, to_hash))
			exit_err_mess("Leaving.", data, EX_OSERR);
		free(to_hash);
		++runner;
	}
	return (0);
}
