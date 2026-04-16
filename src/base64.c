#include "../inc/main.h"

// inline char get_base_char(uint8_t to_convert) { return (BASE64STR[to_convert & 0x3F]); }
char get_base_char(uint8_t to_convert) { return (BASE64STR[to_convert & 0x3F]); }

// inline bool need_pad(const uint32_t input_l) { return (input_l % 3 > 0); }
bool need_pad(const uint32_t input_l) { return (input_l % 3 > 0); }

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

char		*base64_encode(const char *input, const size_t input_l)
{
	const uint32_t	alloc_l = (((input_l / 3) + need_pad(input_l)) * 4);
	uint32_t	tmp = 0;
	char		*res = NULL;

	if(!input_l)
	{
		res = calloc(1, 1);
		if (!res)
			return (ret_err_mess_code_ptr("ft_ssl: base64_encode", errno));
	}
	else
	{
		res = calloc(alloc_l + 1, 1);
		if (!res)
			return (ret_err_mess_code_ptr("ft_ssl: base64_encode", errno));
		for (uint32_t k = 0, i = 0; k < alloc_l; k += 4, i += 3)
		{
			tmp = 0;
			for (uint8_t j = 0; j < 3 && i + j < input_l; j++)
				tmp |= (uint8_t)input[i + j] << (16 - (j * 8));
			res[k] = get_base_char(tmp >> 18);
			res[k + 1] = get_base_char(tmp >> 12);
			res[k + 2] = (input_l - i > 1) ? get_base_char(tmp >> 6) : '=';
			res[k + 3] = (input_l - i > 2) ? get_base_char(tmp) : '=';
		}
	}
	return (res);
}

char	*base64_decode(const char *input, const size_t input_l)
{
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

void	b64_output(t_data *data, char *output, size_t out_len)
{
	int	out_fd = STDOUT_FILENO;
	
	if ((data->options & OUT_FILE) && data->out_file)
	{
		out_fd = open(data->out_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (out_fd < 0)
			return ((void)ret_err_mess_code("ft_ssl: B64Output", errno));
	}
	if (data->options & ENCODE)
	{
		char	*runner = output;
		char	*last = output + out_len;
		
		while (last - runner >= 64)
		{
			if (write(out_fd, runner, 64) < 0
			 || write(out_fd, "\n", 1) < 0)
			{
				close(out_fd);
				return ((void)ret_err_mess_code("ft_ssl: write :", errno));
			}
			runner += 64;
		}
		if (runner < last)
		{
			if (write(out_fd, runner, last - runner) < 0
			 || write(out_fd, "\n", 1) < 0)
			{
				close(out_fd);
				return ((void)ret_err_mess_code("ft_ssl: write :", errno));
			}
		}
	}
	else
	{
		if (write(out_fd, output, out_len) < 0)
		{
			close(out_fd);
			return ((void)ret_err_mess_code("ft_ssl: write :", errno));
		}
	}
}

bool 	b64_routine(t_data *data, char *runner, char *to_hash)
{
	(void)runner;
	char	*res = NULL;
	size_t	in_len = data->len_file;
	size_t	out_len = 0;

	data->options |= B64;
	/* --- IF NO INPUT -> READ_STDIN */
	if (data->options & READ_IN)
		to_hash = cphr_getstdin(&data->options, &in_len);
	if (data->options & DECODE)
	{
		res = base64_decode(to_hash, in_len);
		out_len = (in_len / 4) * 3;
		if (in_len > 0 && to_hash[in_len - 1] == '=') out_len--;
		if (in_len > 1 && to_hash[in_len - 2] == '=') out_len--;
	}
	else if (data->options & ENCODE)
	{
		res = base64_encode(to_hash, in_len);
		out_len = strlen(res);
	}
	else
		res = strdup(to_hash);
	free(to_hash);
	/* --- OUTPUT --- */
	b64_output(data, res, out_len);
	free(res);
	return (EXIT_SUCCESS);
}
