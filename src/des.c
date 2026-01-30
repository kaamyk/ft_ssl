#include "../inc/main.h"

/* ====================== */
/* === KEY SCHEDULING === */
/* ====================== */
inline uint64_t left_circular_shift(uint64_t input, const uint8_t round)
{
	uint8_t	tmp[8] = {0};
	uint8_t	shifted[8] = {0};
	
	memcpy(tmp, &input, 8);
	if (round == 1 || round == 2 || round == 9 || round == 16)
	{
		shifted[0] = tmp[0] << 1 | tmp[1] >> 7;
		shifted[1] = tmp[1] << 1 | tmp[2] >> 7;
		shifted[2] = tmp[2] << 1 | tmp[3] >> 7;
		shifted[3] = tmp[3] << 1 | tmp[4] >> 7;
		shifted[4] = tmp[4] << 1 | tmp[5] >> 7;
		shifted[5] = tmp[5] << 1 | tmp[6] >> 7;
		shifted[6] = tmp[6] << 1 | tmp[7] >> 7;
		shifted[7] = tmp[7] << 1 | tmp[0] >> 7;
	}
	else
	{
		shifted[0] = tmp[0] << 2 | tmp[1] >> 6;
		shifted[1] = tmp[1] << 2 | tmp[2] >> 6;
		shifted[2] = tmp[2] << 2 | tmp[3] >> 6;
		shifted[3] = tmp[3] << 2 | tmp[4] >> 6;
		shifted[4] = tmp[4] << 2 | tmp[5] >> 6;
		shifted[5] = tmp[5] << 2 | tmp[6] >> 6;
		shifted[6] = tmp[6] << 2 | tmp[7] >> 6;
		shifted[7] = tmp[7] << 2 | tmp[0] >> 6;
	}
	memcpy(&input, shifted, 8);
	return (input);
}

uint64_t	DESPermuted_1(uint64_t input)
{
	uint8_t tmp[8] = {0};
	uint8_t	res[8] = {0};
	uint8_t	ff = 0xFF;
	
	memcpy(tmp, &input, 8);
	for (uint8_t i = 0; i < 7; i++)
	{
		printf("res[%d] = (tmp[%d] & %x) | tmp[%d] >> %d\n", 7-i, 7-(i+1), ff >> (7-i), 7-i, i+1);
		res[7 - i] = (tmp[7 - (i + 1)] & (ff >> (7 - i))) | tmp[7 - i] >> (i + 1);
	}
	memcpy(&input, res, 8);
	return (input);
}

uint64_t	DESPermuted_2(uint64_t input)
{
	uint8_t tmp[8] = {0};
	uint8_t res[8] = {0};
	uint8_t ff = 0xFF;
	
	memcpy(tmp, &input, 8);
	if (tmp[0])
		return (ret_err_mess_code("ft_ssl: DESPermuted_2: invalid input", EX_DATAERR));
	res[7]  = tmp[7] & 0x3; 		//0b00000011 |
	res[7] |= (tmp[7] & 0xF8) >> 1;	//0b01111111 | 54
	res[7] |= (tmp[6] & 0x1)  << 7;	//0b11111111 |
	
	res[6] |= (tmp[6] >> 1) & 0xF;	//0b00000001 |
	res[6] |= (tmp[6] & 0xC0) >> 2;	//0b11111111
	res[6] |= (tmp[5] & 0x3)  << 6;
	
	res[5] |= (tmp[5] & 0x18) >> 3;
	res[5] |= (tmp[5] & 0xC0) >> 4;
	res[5] |= (tmp[4] & 0xF)  << 4;
	
	res[4] |= (tmp[4] & 0x70) >> 4;
	res[4] |= (tmp[3] & 0x3)  << 3;
	res[4] |= (tmp[3] & 0x38) << 2;
	
	res[3] |= tmp[3] >> 7;
	res[3] |= tmp[2] << 1;
	
	res[2] = tmp[1];
	return (input);
}

uint8_t	DESs_box(uint8_t input)
{
	const uint8_t	y = input >> 4 | (input & 0x1);
	const uint8_t	x = (input & 0x1E) >> 1;
	const uint8_t	tab[4][16] = {
		{14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7},
		{0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11 , 6, 5, 3, 8},
		{4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0},
		{15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13}
	};

	return(tab[y][x]);
}

uint32_t	DESPermute(uint32_t input, const uint8_t *tab, const size_t tab_len, size_t in_bits)
{
	uint32_t	res = 0;
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

uint64_t	DESExpansion(uint32_t input)
{
	uint64_t	res = 0;
	uint8_t		tab[64] = {
		32, 1, 2, 3, 4, 5,
		4, 5, 6, 7, 8, 9,
		8, 9, 10 , 11, 12, 13,
		12, 13, 14, 15, 16, 17,
		16, 17, 18, 19, 20, 21,
		20, 21, 22, 23, 24, 25,
		24, 25, 26, 27, 28, 29,
		28, 29, 30, 31, 32, 1
	};
	
	return (res = DESPermute(input, tab, 48, 64));
}

bool	DESExec(const t_data *data, const char *runner, const char )

bool 	DESRoutine(const t_data *data, const char *runner, const char *to_hash)
{
	(void)runner;
	char	*res = NULL;
	
	if (data->options & B64)
	{
		if (data->options & DECODE)
			res = base64_decode(to_hash);
		else if (data->options & ENCODE)
			res = base64_encode(to_hash);
	}
	if (cphr_display(res))
		return (EXIT_FAILURE);
	free(res);
	return (EXIT_SUCCESS);
}