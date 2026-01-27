#include "../inc/main.h"

inline char get_base_char(uint8_t to_convert) { return (BASE64STR[to_convert & 0x3F]); }

inline bool need_pad(const uint32_t input_l) { return (input_l % 3 > 0); }

uint8_t		get_base_index(uint8_t to_find)
{
	for (uint8_t i = 0; i < 64; i++)
	{
		if (BASE64STR[i] == to_find)
			return (i);
	}
	return (0);
}

size_t		ft_strlcpy(char *dest, const char *src, size_t size)
{
	unsigned int	i;
	unsigned int	srclen;

	i = 0;
	srclen = strlen(src);
	if (!size)
		return (srclen);
	while (src[i] != '\0' && i < (size - 1))
	{
		dest[i] = src[i];
		i++;
	}
	dest[i] = '\0';
	return (srclen);
}

char		*base64_encode(const char *input)
{
	const uint32_t	input_l = strlen(input);
	const uint32_t	alloc_l = (((input_l / 3) + need_pad(input_l)) * 4);
	uint32_t	tmp = 0;
	char		*res = NULL;
	
	if(!input_l)
		res = calloc(1, 1);
	else 
	{
		res = calloc(alloc_l + 1, 1);
		if (!res)
			return (NULL);
		for (uint32_t k = 0, i = 0; k < alloc_l; k += 4, i += 3)
		{
			tmp = 0;
			for (uint8_t j = 0; j < 3 && i + j < input_l; j++)
					tmp |= input[i + j] << (16 - (j * 8));
			res[k] = get_base_char(tmp >> 18);
			res[k + 1] = get_base_char(tmp >> 12);
			res[k + 2] = (tmp >> 6) & 0x3F ? get_base_char(tmp >> 6) : '=';
			res[k + 3] = tmp & 0x3F ? get_base_char(tmp) : '=';
		}
	}
	return (res);
}

char	*base64_decode(const char *input)
{
	const uint32_t	input_l = strlen(input);
	const uint32_t	alloc_l = ((input_l / 4) * 3);
	uint32_t	tmp = 0;
	char		*res = NULL;
	
	if(!input_l)
		res = calloc(1, 1);
	else 
	{
		res = calloc(alloc_l + 1, 1);
		if (!res)
			return (NULL);
		for (uint32_t k = 0, i = 0; k < alloc_l; k += 3, i += 4)
		{
			tmp = 0;
			for (uint8_t j = 0; j < 4 && i + j < input_l; j++)
				tmp |= (input[i + j] ==  '=' ? (char)0 : get_base_index(input[i + j])) << (18 - (j * 6));
			res[k] = tmp >> 16;
			res[k + 1] = (tmp >> 8) & 0xFF;
			res[k + 2] = tmp & 0xFF;
		}
	}
	return (res);
}

bool 	B64Routine(const t_data *data, const char *runner, const char *to_hash)
{
	printf("> B64ROUTINE\n");
	(void)runner;
	char	*res = NULL;
	
	if (data->options & DECODE)
		res = base64_decode(to_hash);
	else if (data->options & ENCODE)
		res = base64_encode(to_hash);
	else
		res = strdup(to_hash);
	if (cphr_display(res))
		return (EXIT_FAILURE);
	free(res);
	return (EXIT_SUCCESS);
}