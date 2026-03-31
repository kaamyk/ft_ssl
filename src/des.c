#include "../inc/main.h"

void left_circular_shift(uint32_t *input, const uint8_t round)
{
	if (round == 0 || round == 1 || round == 8 || round == 15)
		*input = ((*input << 1) | (*input >> 27)) & 0xFFFFFFF;
	else
		*input = ((*input << 2) | (*input >> 26)) & 0xFFFFFFF;
}

uint8_t	DESs_box(uint8_t input, const uint8_t boxno)
{
	const uint8_t	y = ((input >> 5) & 1UL) << 1 | (input & 1UL);
	const uint8_t	x = (input >> 1) & 0xF;
	const uint8_t	tab[8][4][16] = {
		{
			{14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7},
			{0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8},
			{4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0},
			{15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13}
		},

		{
			{15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10},
			{3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5},
			{0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15},
			{13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9}
		},
		{
			{10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8},
			{13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1},
			{13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7},
			{1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12}
		},
		{
			{7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15},
			{13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9},
			{10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4},
			{3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14}
		},
		{
			{2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9},
			{14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6},
			{4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14},
			{11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3}
		},
		{
			{12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11},
			{10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8},
			{9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6},
			{4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13}
		},
		{
			{4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1},
			{13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6},
			{1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2},
			{6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12}
		},
		{
			{13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7},
			{1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2},
			{7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8},
			{2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11}
		}
	};

	return(tab[boxno][y][x]);
}

uint64_t	DESPermute(uint64_t input, const uint8_t *tab, const size_t tab_len, size_t in_bits)
{
	uint64_t	res = 0;
	uint8_t		src = 0;
	uint32_t	bit = 0;

	for (size_t i = 0; i < tab_len; i++)
	{
		src = tab[i] - 1;
		bit = (input >> (in_bits - 1 - src)) & 1U;
		res = (res << 1) | bit;
	}
	return (res);
}

uint32_t	DESp_box(uint32_t input)
{
	uint8_t	tab[32] = {
		16,  7, 20, 21, 29, 12, 28, 17,
		 1, 15, 23, 26,  5, 18, 31, 10,
		 2,  8, 24, 14, 32, 27,  3,  9,
		19, 13, 30,  6, 22, 11,  4, 25
	};

	return (input = DESPermute(input, tab, 32, 32));
}

uint64_t	DESInitial_permutation(uint64_t input)
{
	uint8_t	tab[64] = {
		58, 50, 42, 34, 26, 18, 10, 2,
		60, 52, 44, 36, 28, 20, 12, 4,
		62, 54, 46, 38, 30, 22, 14, 6,
		64, 56, 48, 40, 32, 24, 16, 8,
		57, 49, 41, 33, 25, 17, 9, 1,
		59, 51, 43, 35, 27, 19, 11, 3,
		61, 53, 45, 37, 29, 21, 13, 5,
		63, 55, 47, 39, 31, 23, 15, 7
	};

	return (input = DESPermute(input, tab, 64, 64));
}

uint64_t	DESInverse_initial_permutation(uint64_t input)
{
	uint8_t	tab[64] = {
		40, 8, 48, 16, 56, 24, 64, 32,
		39, 7, 47, 15, 55, 23, 63, 31,
		38, 6, 46, 14, 54, 22, 62, 30,
		37, 5, 45, 13, 53, 21, 61, 29,
		36, 4, 44, 12, 52, 20, 60, 28,
		35, 3, 43, 11, 51, 19, 59, 27,
		34, 2, 42, 10, 50, 18, 58, 26,
		33, 1, 41, 9, 49, 17, 57, 25
	};
	return (input = DESPermute(input, tab, 64, 64));
}

void	DESPermuted_1(uint64_t *input)		// 64 bits -> 56 bits
{
	uint8_t	tab[56] = {
		57, 49, 41, 33, 25, 17,  9,  1,
		58, 50, 42, 34, 26, 18, 10,  2,
		59, 51, 43, 35, 27, 19, 11,  3,
		60, 52, 44, 36, 63, 55, 47, 39,
		31, 23, 15,  7, 62, 54, 46, 38,
		30, 22, 14,  6, 61, 53, 45, 37,
		29, 21, 13,  5, 28, 20, 12,  4
	};

	*input = DESPermute(*input, tab, 56, 64);
}

void	DESPermuted_2(uint64_t *input)		// 56 bits -> 48 bits
{
	uint8_t	tab[48] = {
		14, 17, 11, 24,  1,  5,  3, 28,
		15,  6, 21, 10, 23, 19, 12,  4,
		26,  8, 16,  7, 27, 20, 13,  2,
		41, 52, 31, 37, 47, 55, 30, 40,
		51, 45, 33, 48, 44, 49, 39, 56,
		34, 53, 46, 42, 50, 36, 29, 32
	};
	*input = DESPermute(*input, tab, 48, 56);
}

uint64_t	DESExpansion(uint32_t input)
{
	uint64_t	res = 0;
	uint32_t	bit = 0;
	uint8_t		tab[48] = {
		32,  1,  2,  3,  4,  5,
		 4,  5,  6,  7,  8,  9,
		 8,  9, 10, 11, 12, 13,
		12, 13, 14, 15, 16, 17,
		16, 17, 18, 19, 20, 21,
		20, 21, 22, 23, 24, 25,
		24, 25, 26, 27, 28, 29,
		28, 29, 30, 31, 32,  1
	};

	for (size_t i = 0; i < 48; i++)
	{
		bit = (input >> (32 - 1 - (tab[i] - 1))) & 1U;
		res = (res << 1) | bit;
	}
	return (res);
}

void	key_scheduling(uint32_t *key_left, uint32_t *key_right, const uint8_t round, uint64_t *round_key)
{
	uint64_t	tmp = 0;

	/* Left Shift */
	left_circular_shift(key_left, round);
	left_circular_shift(key_right, round);
	/* 2 * 28-bit => 56-bit */
	tmp = ((uint64_t)*key_left << 28) | *key_right;
	DESPermuted_2(&tmp);
	/* uint64_t => 48-bit */
	*round_key = tmp;
}

uint32_t	ft_mangler(uint64_t to_enc, uint64_t round_key)
{
	uint32_t	enc_right = to_enc & 0xFFFFFFFF;
	uint32_t	res = 0;
	uint64_t	xored = 0;

	xored = DESExpansion(enc_right) ^ round_key;
	for (uint8_t i = 0; i < 8; i++)
		res |= (uint32_t)DESs_box((xored >> (42 - i * 6)) & 0x3F, i) << (28 - (i * 4));
	return (DESp_box(res));
}

void	DESRound(uint64_t *chunck_input, uint64_t sub_key)
{
	uint64_t	output = 0;
	uint32_t	chunck_split[2] = {0};

	memcpy(chunck_split, chunck_input, 8);
	output = ft_mangler(*chunck_input, sub_key);
	*chunck_input = chunck_split[0];
	*chunck_input <<= 32;
	*chunck_input |= (output & 0xFFFFFFFF) ^ chunck_split[1];
	// *key = ((uint64_t)key_split[0] << 28) | key_split[1];
}

char	*DESPadding(char *to_encrypt, size_t *len_to_enc)
{
	char	*to_enc_pad = NULL;
	char	pad = 0;
	
	// *len_to_enc = strlen(to_encrypt);
	if (*len_to_enc % 8)
		pad = 8 - (*len_to_enc % 8);
	to_enc_pad = malloc(*len_to_enc + pad + 1);
	strcpy(to_enc_pad, to_encrypt);
	memset(to_enc_pad + *len_to_enc, pad, pad);
	*len_to_enc += pad;
	to_enc_pad[*len_to_enc] = 0;
	free(to_encrypt);
	return (to_enc_pad);
}

void	DESEncryptLoop(uint64_t *chunck_input, uint64_t sub_keys[16])
{
	for (uint8_t j = 0; j < 16; j++)
		DESRound(chunck_input, sub_keys[j]);
}

void	DESDecryptLoop(uint64_t *chunck_input, uint64_t sub_keys[16])
{
	for (int8_t j = 15; j >= 0; j--)
		DESRound(chunck_input, sub_keys[j]);
}

void	generate_sub_keys(uint64_t sub_keys[16], uint64_t key)
{
	uint32_t	key_split[2] = {0};

	DESPermuted_1(&key);
	for(uint8_t i = 0; i < 16; i++)
	{
		key_split[0] = (key >> 28) & 0xFFFFFFF;
		key_split[1] = (key) & 0xFFFFFFF;
		key_scheduling(&key_split[0], &key_split[1], i, &sub_keys[i]);
		key = ((uint64_t)key_split[0] << 28) | key_split[1];
	}
}

char	*DESSetupInput(t_data *data, char *to_encrypt, size_t *len_to_enc)
{
	if (data->options & ENCODE)
		to_encrypt = DESPadding(to_encrypt, len_to_enc);
	else
	{
		*len_to_enc = strlen(to_encrypt) / 2;
		for (size_t k = 0; k < *len_to_enc; k++)
		{
			uint8_t hi = strchr(HEXABASE, toupper((uint8_t)to_encrypt[k * 2])) - HEXABASE;
			uint8_t lo = strchr(HEXABASE, toupper((uint8_t)to_encrypt[k * 2 + 1])) - HEXABASE;
			to_encrypt[k] = (char)((hi << 4) | lo);
		}
		to_encrypt[*len_to_enc] = 0;
	}
	return (to_encrypt);
}

uint64_t	DESSetupIV(const char *raw_init_vector)
{
	char		str_format[17] = {0};
	uint64_t	res = 0;
	
	res = strlen(raw_init_vector);
	if (res > 16)
		res = 16;
	memcpy(str_format, raw_init_vector, res);
	res = atohex(str_format);
	return (res);
}

bool	DESRoutine(t_data *data, char *runner, char *to_encrypt)
{
	(void)runner;
	char		*buf = NULL;
	uint8_t		*kdf_res = NULL;
	uint64_t	*full_output = NULL;
	size_t		buf_l = 0;
	uint64_t	chunck_input = 0;
	uint64_t	sub_keys[16] = {0};
	size_t		len_to_enc = strlen(to_encrypt);
	t_pbkdf2	l_data = {
		.password = data->password, .salt = data->salt,
		.salt_l = data->salt_len, .dk_len = 8, .c = 10000
	};
	
	//	Generate key if not in args
	if (!data->raw_key)
	{
		kdf_res = PBKDF2(l_data, HMAC256);
		// if (malloc_usable_size(kdf_res) == 32)
		if (kdf_res)
		{
			memcpy(&data->key, kdf_res, 8);
			data->key = __bswap_64(data->key);
		}
		free(kdf_res);
	}
	if ((data->options & B64) && data->options & DECODE)
	{
		buf = to_encrypt;
		to_encrypt = base64_decode(to_encrypt, len_to_enc);
		free(buf);
	}
	full_output = calloc(len_to_enc + (8 - len_to_enc % 8), sizeof(char));
	generate_sub_keys(sub_keys, data->key);
	/* --- PAD / HEX-DECODE INPUT --- */
	to_encrypt = DESSetupInput(data, to_encrypt, &len_to_enc);
	/* --- INITIALIZATION VECTOR */
	// if (data->raw_init_vector)
	// 	data->init_vector = DESSetupIV(data->raw_init_vector);
	/* --- CIPHER ALGO --- */
	for (size_t i = 0; i < len_to_enc; i += 8)
	{
		memcpy(&chunck_input, to_encrypt + i, 8);
		chunck_input = bswap_64(chunck_input);
		/* --- INIT VECTOR --- */
		if ((data->options & ENCODE) && data->init_vector)
			chunck_input ^= data->init_vector;
		chunck_input = DESInitial_permutation(chunck_input);
		if (data->options & ENCODE)
			DESEncryptLoop(&chunck_input, sub_keys);
		else if (data->options & DECODE)
			DESDecryptLoop(&chunck_input, sub_keys);
		chunck_input = (chunck_input >> 32) | (chunck_input << 32);
		chunck_input = DESInverse_initial_permutation(chunck_input);
		if ((data->options & DECODE) && data->init_vector)
			chunck_input ^= data->init_vector;
		chunck_input = bswap_64(chunck_input);
		memcpy((char *)full_output + i, &chunck_input, 8);
	}
	if ((data->options & B64) && (data->options & ENCODE))
	{
		buf = base64_encode((char *)full_output, len_to_enc);
		buf_l = strlen(buf);
	}
	else
	{
		buf = (char *) full_output;
		buf_l = len_to_enc;
		// buf_l = 8;
	}
	if (cphr_display(buf, buf_l))
		exit_err_code(data, EX_OSERR);
	if ((char *)full_output != buf)
		free(full_output);
	free(buf);
	free(to_encrypt);
	return (EXIT_SUCCESS);
}
