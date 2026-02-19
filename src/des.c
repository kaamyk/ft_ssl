#include "../inc/main.h"

/* ====================== */
/* === KEY SCHEDULING === */
/* ====================== */
// inline void left_circular_shift(uint8_t input[4], const uint8_t round)
// {
// 	uint8_t	tmp[4] = {0};
// 	uint8_t	shifted[8] = {0};

// 	memcpy(tmp, &input, 4);
// 	if (round == 1 || round == 2 || round == 9 || round == 16)
// 	{
// 		input[0] = tmp[0] << 1 | tmp[1] >> 7;
// 		input[1] = tmp[1] << 1 | tmp[2] >> 7;
// 		input[2] = tmp[2] << 1 | tmp[3] >> 7;
// 		input[3] = tmp[3] << 1 | tmp[0] >> 7;
// 		// input[4] = tmp[4] << 1 | tmp[5] >> 7;
// 		// input[5] = tmp[5] << 1 | tmp[6] >> 7;
// 		// input[6] = tmp[6] << 1 | tmp[0] >> 7;
// 		// input[7] = tmp[7] << 1 | tmp[0] >> 7;
// 	}
// 	else
// 	{
// 		input[0] = tmp[0] << 2 | tmp[1] >> 6;
// 		input[1] = tmp[1] << 2 | tmp[2] >> 6;
// 		input[2] = tmp[2] << 2 | tmp[3] >> 6;
// 		input[3] = tmp[3] << 2 | tmp[0] >> 6;
// 		// input[4] = tmp[4] << 2 | tmp[5] >> 6;
// 		// input[5] = tmp[5] << 2 | tmp[6] >> 6;
// 		// input[6] = tmp[6] << 2 | tmp[0] >> 6;
// 		// input[7] = tmp[7] << 2 | tmp[0] >> 6;
// 	}
// 	// memcpy(&input, input, 8);
// 	// return (input);
// }

// inline void left_circular_shift(uint32_t *input, const uint8_t round)
void left_circular_shift(uint32_t *input, const uint8_t round)
{
	printf("left_circu(%x, %d)\n", *input, round);
	if (round == 1 || round == 2 || round == 9 || round == 16)
		*input = (((*input << 1) & 0xFFFFFFF) | (*input >> 26 & 0x1)) & 0xFFFFFFF;
	else
		*input = (((*input << 2) & 0xFFFFFFF) | (*input >> 25 & 0x1)) & 0xFFFFFFF;
		// *input = (*input << 2 | *input >> 25) & 0xFFFFFFF;
	printf("end(%x, %d)\n", *input, round);
}

// void	DESPermuted_1(uint64_t *input)
// {
// 	uint8_t tmp[8] = {0};
// 	uint8_t key_rot[8] = {0};
// 	uint8_t	ff = 0xFF;

// 	printf("DESPermuted1(%lx)\n", *input);
// 	memcpy(tmp, &input, 8);
// 	for (uint8_t i = 0; i < 7; i++)
// 		key_rot[7 - i] = (tmp[7 - (i + 1)] & (ff >> (7 - i))) | (tmp[7 - i] >> (i + 1));
// 	bzero(input, 8);
// 	memcpy((uint8_t *)input + 1, key_rot, 7);
// 	printf("end %lx\n", *input);
// 	// Viens de finit cette fonction
// 	// dans le cadre d'initialiser la clef
// 	// avant d'entrer dans l'algo
// }

uint8_t	DESs_box(uint8_t input, const uint8_t boxno)
{
	const uint8_t	y = input >> 4 | (input & 0x1);
	const uint8_t	x = (input & 0x1E) >> 1;
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
		16, 7, 20, 21, 29, 12, 28, 17,
		1, 15, 23, 26, 5, 18, 31, 10,
		2, 8, 24, 14, 32, 27, 3, 9,
		19, 13, 30, 6, 22, 11, 4, 25
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
		40, 8, 48, 16, 39, 7, 47, 15,
		38, 6, 46, 14, 37, 5, 45, 13,
		36, 4, 44, 12, 35, 3, 43, 11,
		34, 2, 42, 10, 33, 1, 41, 9
	};

	return (input = DESPermute(input, tab, 64, 64));
}

void	DESPermuted_1(uint64_t *input)		// 64 bits -> 56 bits
{
	uint8_t	tab[56] = {
		 1,  2,  3,  4,  5,  6,  7,  9,
		10, 11, 12, 13, 14, 15, 17, 18,
		19, 20, 21, 22, 23, 25, 26, 27,
		28, 29, 30, 31, 33, 34, 35, 36,
		37, 38, 39, 41, 42, 43, 44, 45,
		46, 47, 49, 50, 51, 52, 53, 54,
		55, 57, 58, 59, 60, 61, 62, 63
	};
	
	*input = DESPermute(*input, tab, 56, 64);
}

void	DESPermuted_2(uint64_t *input)		// 56 bits -> 48 bits
{
	// modifier avec la methode du tableau et de lq fonction au dessus

	uint8_t	tab[48] = {
		 1,  2,  3,  4,  5,  6,  7,  8,
		10, 11, 12, 13, 14, 15, 16, 17,
		19, 20, 21, 23, 24, 26, 27, 28,
		29, 30, 31, 32, 33, 34, 36, 37,
		39, 40, 41, 42, 44, 45, 46, 47,
		48, 49, 50, 51, 52, 53, 54, 55
	};
	*input = DESPermute(*input, tab, 48, 56);
}

void	DESExpansion(uint32_t input, uint8_t expanded[6])
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
	memcpy(expanded, (uint8_t *)&res + 2, 6);
}

void	key_scheduling(uint32_t *key_left, uint32_t *key_right, const uint8_t round, uint64_t *round_key)
{
	uint8_t		key[7] = {0};
	// uint8_t		res[6] = {0};
	uint64_t	tmp = 0;

	/* Left Shift */
	left_circular_shift(key_left, round);		// left shift doit etre fait sur sur
	left_circular_shift(key_right, round);		// la clef 56-bit et paas la clef de round
	/* 2 * 28-bit => 56-bit */
	*key_left <<= 4;
	memcpy(&tmp, &key_left, 4);
	key[3] |= ((*key_right >> 24) & 0x0F);
	memcpy(key + 4, (uint8_t *)&key_right + 1, 3);
	memcpy((uint8_t *)&tmp + 1, key, 7);
	/* Permuted Choice 2 */
	DESPermuted_2(&tmp);
	/* uint64_t => 48-bit */
	tmp <<= 16;
	*round_key = tmp;
	// memcpy(round_key, &tmp, 6);
}

uint32_t	ft_mangler(uint64_t to_enc, uint64_t round_key)
{
	printf("ft_mangler(%lx, %lx)\n", to_enc, round_key);
	uint8_t		exp_to_enc[6] = {0};
	uint8_t		key_split[6] = {0};
	uint32_t	enc_right = to_enc & 0xFFFFFFFF;
	uint32_t	res = {0};
	uint64_t	tmp = 0;

	memcpy(key_split, &round_key, 6);
	DESExpansion(enc_right, exp_to_enc);
	for (uint8_t i = 0; i < 6; i++)
		exp_to_enc[i] = exp_to_enc[i] ^ key_split[i];
	memcpy((uint8_t *)&tmp + 2, exp_to_enc, 6);
	for (uint8_t i = 0; i < 8; i++)
		res |= (DESs_box(tmp, i)) << (28 - (i * 4)); // Check if each box different
	res = DESp_box(res);
	return (res);
}

void	DESRound(uint64_t *chunck_input, uint64_t *key, const uint8_t round)
{
	uint64_t	output = 0;
	uint32_t	key_split[2] = {0};
	uint64_t	round_key = 0;

	printf("DESRound(%lx, %lx, %d)\n", *chunck_input, *key, round);
	// memcpy(key_split, key, 8);
	key_split[0] = (*key >> 28) & 0xFFFFFFF;
	key_split[1] = (*key) & 0xFFFFFFF;
	key_scheduling(&key_split[0], &key_split[1], round, &round_key);
	DESPermuted_2(&round_key);
	output = ft_mangler(*chunck_input, round_key);
	*chunck_input = key_split[1];
	*chunck_input <<= 32;
	*chunck_input |= (output & 0xFFFFFFFF) ^ key_split[0];
	printf("chunck_input -> %lx\n", *chunck_input);
	*key = key_split[0];
	*key <<= 28;
	*key |= key_split[1];
	
	// return (output);
}

char	*DESPadding(char *to_encrypt, size_t *len_to_enc)
{
	char	*to_enc_pad = NULL;
	char	pad = 0;
	
	*len_to_enc = strlen(to_encrypt);
	if (*len_to_enc % 8)
		pad = 8 - (*len_to_enc % 8);
	to_enc_pad = malloc(*len_to_enc + pad + 1);
	strcpy(to_enc_pad, to_encrypt);
	memset(to_enc_pad + *len_to_enc, pad, pad);
	to_enc_pad[*len_to_enc + pad] = 0;
	free(to_encrypt);
	return (to_enc_pad);
}

bool	DESRoutine(t_data *data, char *runner, char *to_encrypt)
{
	printf("DESRoutine(data(%p), *runner(%p) -> [%s], *to_encrypt(%p) -> [%s])\n", data, runner, runner, to_encrypt, to_encrypt);
	(void)runner;
	char		*res = NULL;
	uint64_t	chunck_input = 0;
	uint64_t	key = data->key;
	size_t		len_to_enc = 0;

	// /* --- BASE 64--- */
	// if (data->options & B64)
	// {
	// 	if (data->options & DECODE)
	// 		res = base64_decode(to_encrypt);
	// 	else if (data->options & ENCODE)
	// 		res = base64_encode(to_encrypt);
	// }
	/* --- PAD TO_ENCRYPT --- */
	to_encrypt = DESPadding(to_encrypt, &len_to_enc);
	/* --- CIPHER ALGO --- */
	for (uint32_t i = 0; i < len_to_enc; i += 64)
	{
		printf("========= Chunck %d =============\n", i);
		chunck_input = DESInitial_permutation((uint64_t)to_encrypt + i);
		printf("chunck %d == %lx | key == %lx\n", i, chunck_input, key);
		DESPermuted_1(&key);
		for (uint8_t j = 0; j < 16; j++)
		{
			printf("--------- Round %d -------------\n", j);
			DESRound(&chunck_input, &key, j);
			printf("Round End : chunck = %lx | key = %lx\n", chunck_input, key);
		}
		chunck_input = (chunck_input >> 32 ) | (chunck_input << 32);
		chunck_input = DESInverse_initial_permutation(chunck_input);
		if (res)
			res = ft_strjoin(res, (char*)chunck_input);
		else
			res = (char *)chunck_input;
		// printf("res => [%s]\n", res? res : "null");
	}
	// if (cphr_display(res))
	// 	return (EXIT_FAILURE);
	// free(res);
	free(to_encrypt);
	return (EXIT_SUCCESS);
}
