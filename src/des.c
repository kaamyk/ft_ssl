#include "../inc/main.h"

void left_circular_shift(uint32_t *input, const uint8_t round)
{
	if (round == 0 || round == 1 || round == 8 || round == 15)
		*input = ((*input << 1) | (*input >> 27)) & 0xFFFFFFF;
	else
		*input = ((*input << 2) | (*input >> 26)) & 0xFFFFFFF;
}

uint8_t	des_s_box(uint8_t input, const uint8_t boxno)
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

uint64_t	des_permute(uint64_t input, const uint8_t *tab, const size_t tab_len, size_t in_bits)
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

uint32_t	des_p_box(uint32_t input)
{
	uint8_t	tab[32] = {
		16,  7, 20, 21, 29, 12, 28, 17,
		 1, 15, 23, 26,  5, 18, 31, 10,
		 2,  8, 24, 14, 32, 27,  3,  9,
		19, 13, 30,  6, 22, 11,  4, 25
	};

	return (des_permute(input, tab, 32, 32));
}

uint64_t	des_initial_permutation(uint64_t input)
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

	return (des_permute(input, tab, 64, 64));
}

uint64_t	des_inverse_initial_permutation(uint64_t input)
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

	return (des_permute(input, tab, 64, 64));
}

void	des_permuted_1(uint64_t *input)		// 64 bits -> 56 bits
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

	*input = des_permute(*input, tab, 56, 64);
}

void	des_permuted_2(uint64_t *input)		// 56 bits -> 48 bits
{
	uint8_t	tab[48] = {
		14, 17, 11, 24,  1,  5,  3, 28,
		15,  6, 21, 10, 23, 19, 12,  4,
		26,  8, 16,  7, 27, 20, 13,  2,
		41, 52, 31, 37, 47, 55, 30, 40,
		51, 45, 33, 48, 44, 49, 39, 56,
		34, 53, 46, 42, 50, 36, 29, 32
	};
	*input = des_permute(*input, tab, 48, 56);
}

uint64_t	des_expansion(uint32_t input)
{
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

	return (des_permute(input, tab, 48, 32));
}

void	key_scheduling(uint32_t *key_left, uint32_t *key_right, const uint8_t round, uint64_t *round_key)
{
	uint64_t	tmp = 0;

	/* Left Shift */
	left_circular_shift(key_left, round);
	left_circular_shift(key_right, round);
	/* 2 * 28-bit => 56-bit */
	tmp = ((uint64_t)*key_left << 28) | *key_right;
	des_permuted_2(&tmp);
	/* uint64_t => 48-bit */
	*round_key = tmp;
}

uint32_t	ft_mangler(uint64_t to_enc, uint64_t round_key)
{
	uint32_t	enc_right = to_enc & 0xFFFFFFFF;
	uint32_t	res = 0;
	uint64_t	xored = 0;

	xored = des_expansion(enc_right) ^ round_key;
	for (uint8_t i = 0; i < 8; i++)		// Split the 48-bit in 8 6-bit chunck to send to S-Box
		res |= (uint32_t)des_s_box((xored >> (42 - i * 6)) & 0x3F, i) << (28 - (i * 4));
	//	res |= des_s_box(6-bit value from xored , round nb) << its place
	return (des_p_box(res));
}

void	des_round(uint64_t *chunck_input, uint64_t sub_key)
{
	uint64_t	output = 0;
	uint32_t	chunck_split[2] = {0};

	memcpy(chunck_split, chunck_input, 8);
	output = ft_mangler(*chunck_input, sub_key);
	*chunck_input = chunck_split[0];
	*chunck_input <<= 32;
	*chunck_input |= (output & 0xFFFFFFFF) ^ chunck_split[1];
}

char	*des_padding(char *to_encrypt, size_t *len_to_enc, uint32_t mod)
{
	char	*to_enc_pad = NULL;
	char	pad = 0;

	// *len_to_enc = strlen(to_encrypt);
	// pad = 8 - (*len_to_enc % 8);
	pad = mod - (*len_to_enc % mod);
	to_enc_pad = malloc(*len_to_enc + pad + 1);
	memcpy(to_enc_pad, to_encrypt, *len_to_enc);
	memset(to_enc_pad + *len_to_enc, pad, pad);
	*len_to_enc += pad;
	to_enc_pad[*len_to_enc] = 0;
	free(to_encrypt);
	return (to_enc_pad);
}

void	des_encrypt_loop(uint64_t *chunck_input, uint64_t sub_keys[16])
{
	for (uint8_t j = 0; j < 16; j++)
		des_round(chunck_input, sub_keys[j]);
}

void	des_decrypt_loop(uint64_t *chunck_input, uint64_t sub_keys[16])
{
	for (int8_t j = 15; j >= 0; j--)
		des_round(chunck_input, sub_keys[j]);
}

void	generate_sub_keys(uint64_t sub_keys[16], uint64_t key)
{
	uint32_t	key_split[2] = {0};

	des_permuted_1(&key);
	for(uint8_t i = 0; i < 16; i++)
	{
		key_split[0] = (key >> 28) & 0xFFFFFFF;
		key_split[1] = (key) & 0xFFFFFFF;
		key_scheduling(&key_split[0], &key_split[1], i, &sub_keys[i]);
		key = ((uint64_t)key_split[0] << 28) | key_split[1];
	}
}

char	*des_setup_input(t_data *data, char *to_encrypt, size_t *len_to_enc)
{
	if (data->options & ENCODE)
		to_encrypt = des_padding(to_encrypt, len_to_enc, 8);
	return (to_encrypt);
}

uint64_t	des_setup_iv(const char *raw_init_vector)
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

void	des_generate_key(t_data *data, t_pbkdf2 *l_data)
{
	uint8_t		*kdf_res = NULL;

	if (!data->password && !data->raw_salt)
		l_data->password = getpass("enter des-ecb encryption password:");
	l_data->dk_len = 16;
	kdf_res = PBKDF2(*l_data, HMAC256);
	if (kdf_res)
	{
		memcpy(&data->key, kdf_res, 8);
		data->key = __bswap_64(data->key);
		if (!data->raw_init_vector)
		{
			memcpy(&data->init_vector, kdf_res + 8, 8);
			data->init_vector = __bswap_64(data->init_vector);
		}
	}
	free(kdf_res);
}

void	des_decode_b64_input(char **to_encrypt, size_t *len_to_enc)
{
	char	*b64_input = NULL;
	size_t	b64_len = 0;
	size_t	pad_count = 0;

	b64_input = *to_encrypt;
	b64_len = *len_to_enc;
	if (b64_len >= 2)
	{
		if (b64_input[b64_len - 1] == '=') pad_count++;
		if (b64_input[b64_len - 2] == '=') pad_count++;
	}
	*to_encrypt = base64_decode(b64_input, b64_len);
	free(b64_input);
	*len_to_enc = (b64_len / 4) * 3 - pad_count;
}

bool	des_setup_key(t_pbkdf2 *l_data, t_data *data)
{
	if (data->raw_key)
	{
		size_t	len = strlen(data->raw_key);
		if (len < 16)
		{
			char	*padded = calloc(17, 1);
			if (write(STDERR_FILENO, "hex string is too short, padding with zero bytes to length\n", 60) < 0)
				return (ret_err_mess_code("ft_ssl: write", errno));
			memcpy(padded, data->raw_key, len);
			memset(padded + len, '0', 16 - len);
			free(data->raw_key);
			data->raw_key = padded;
		}
		data->key = atohex(data->raw_key);
		// if len > 16 : conversion to uint64_t troncates the string
	}
	else
		des_generate_key(data, l_data);
	return (EXIT_SUCCESS);
}

bool	des_setup_file_in(char **to_encrypt, size_t *len_to_enc, t_pbkdf2 *l_data, t_data *data)
{
	uint64_t	tmp = 0;
	char		*buf = *to_encrypt;
	char		*cipher = NULL;
	
	if (!buf || data->len_file < 16)
		return (ret_err_mess("ft_ssl: to_encrypt invalid."));
	if (memcmp(SALTBYTES, data->in, 8))
		return (ret_err_mess("ft_ssl: des: wrong magic bytes in input file."));
	memcpy(&tmp, buf + 8, 8);
	l_data->salt = __bswap_64(tmp);
	l_data->salt_l = sizeof(uint64_t);
	*len_to_enc = data->len_file - 16;
	cipher = calloc(*len_to_enc, 1);
	if (!cipher)
		return (ret_err_mess_code("ft_ssl: calloc", errno));
	memcpy(cipher, buf + 16, *len_to_enc);
	free(buf);
	*to_encrypt = cipher;
	return (EXIT_SUCCESS);
}

void	des_setup(char **to_encrypt, t_des_data *des_data, t_data *data)
{
	t_pbkdf2	l_data = {
		.password = data->password, .salt = data->salt,
		.salt_l = data->salt_len, .dk_len = 16, .c = 10000
	};

	if ((data->options & IN_FILE) && data->in_file
	 && des_setup_file_in(to_encrypt, &des_data->len_to_enc, &l_data, data))
		return ;
	if (des_setup_key(&l_data, data))
		return ;
	if (data->options & PWP)
		printf("salt=%016lX\nkey=%016lX\n", data->salt, data->key);
	if (l_data.password && l_data.password != data->password)
		free(l_data.password);
	/* --- IF NO INPUT -> READ_STDIN */
	if (data->options & READ_IN)
		*to_encrypt = cphr_getstdin(&data->options, &des_data->len_to_enc);
	if ((data->options & B64) && data->options & DECODE)
		des_decode_b64_input(to_encrypt, &des_data->len_to_enc);
	des_data->full_output = calloc(des_data->len_to_enc + (8 - (des_data->len_to_enc % 8)), sizeof(char));
	generate_sub_keys(des_data->sub_keys, data->key);
	/* --- PAD / HEX-DECODE INPUT --- */
	if (!((data->options & B64) && (data->options & DECODE)))
		*to_encrypt = des_setup_input(data, *to_encrypt, &des_data->len_to_enc);
	/* --- INITIALIZATION VECTOR */
	if (data->raw_init_vector)
		data->init_vector = des_setup_iv(data->raw_init_vector);
}

void	des_output(t_data *data, char *out_buf, char *to_encrypt, size_t out_len)
{
	/* --- OUTPUT --- */
	int out_fd = STDOUT_FILENO;
	if (data->out_file)
	{
		out_fd = open(data->out_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (out_fd < 0)
			exit_err_code(data, EX_OSERR);
	}
	if ((data->options & B64) && (data->options & ENCODE))
	{
		char	*runner = out_buf;
		char	*last_b64 = out_buf + out_len;
		
		while (last_b64 - runner >= 64)
		{
			if (write(out_fd, runner, 64) < 0
			 || write(out_fd, "\n", 1) < 0)
				fprintf(stderr, "%s: %s(%d)\n", "ft_ssl: des_output", strerror(errno), errno);
			runner += 64;
		}
		if (runner < last_b64)
		{
			if (write(out_fd, runner, last_b64 - runner) < 0
			 || write(out_fd, "\n", 1) < 0)
				fprintf(stderr, "%s: %s(%d)\n", "ft_ssl: des_output", strerror(errno), errno);
		}
		free(out_buf);
	}
	else
	{
		if (write(out_fd, out_buf, out_len) < 0)
			fprintf(stderr, "%s: %s(%d)\n", "ft_ssl: des_output", strerror(errno), errno);
		free(out_buf);
	}
	if (out_fd != STDOUT_FILENO)
		close(out_fd);
	free(to_encrypt);
}

// void	des_loop(char *to_encrypt, const size_t len_to_enc, uint64_t **full_output, uint64_t sub_keys[16], t_data *data)
void	des_loop(char *to_encrypt, t_des_data *des_data, t_data *data)
{
	uint64_t	chunck_input = 0;
	uint64_t	next_iv = 0;

	/* --- CIPHER ALGO --- */
	for (size_t i = 0; i < des_data->len_to_enc; i += 8)
	{
		memcpy(&chunck_input, to_encrypt + i, 8);
		chunck_input = bswap_64(chunck_input);
		next_iv = chunck_input;	/* save ciphertext for CBC decrypt IV */
		/* --- INIT VECTOR --- */
		if (!des_data->ebc && (data->options & ENCODE) && data->init_vector)
			chunck_input ^= data->init_vector;
		chunck_input = des_initial_permutation(chunck_input);
		if (data->options & ENCODE)
			des_encrypt_loop(&chunck_input, des_data->sub_keys);
		else if (data->options & DECODE)
			des_decrypt_loop(&chunck_input, des_data->sub_keys);
		chunck_input = (chunck_input >> 32) | (chunck_input << 32);
		chunck_input = des_inverse_initial_permutation(chunck_input);
		if (!des_data->ebc && (data->options & DECODE) && data->init_vector)
			chunck_input ^= data->init_vector;
		if (!des_data->ebc)
		{
			if (data->options & ENCODE)
				data->init_vector = chunck_input;	/* big-endian ciphertext, before bswap */
			else
				data->init_vector = next_iv;		/* original ciphertext block */
		}
		chunck_input = bswap_64(chunck_input);
		memcpy((char *)des_data->full_output + i, &chunck_input, 8);
	}
}

bool	des_routine(t_data *data, char *runner, char *to_encrypt)
{
	(void)runner;
	t_des_data	des_data = {
		.full_output = NULL,
		.sub_keys = {0},
		// size_t		len_to_enc = data->in_len ? data->in_len : strlen(to_encrypt);
		.len_to_enc = to_encrypt ? strlen(to_encrypt) : 0,
		.ebc = (data->algo[3] == '-' && data->algo[4] == 'e') // true only for "des-ebc"
	};

	// des_setup(&full_output, sub_keys, &len_to_enc, &to_encrypt, data);
	des_setup(&to_encrypt, &des_data, data);
	if (data->options & PWP)
	{
		free(des_data.full_output);
		free(to_encrypt);
		return (EXIT_SUCCESS);
	}
	// des_loop(to_encrypt, len_to_enc, &full_output, sub_keys, data);
	des_loop(to_encrypt, &des_data, data);
	/* --- PKCS7 UNPADDING (decrypt) --- */
	if (data->options & DECODE)
	{
		uint8_t pad = ((uint8_t *)des_data.full_output)[des_data.len_to_enc - 1];
		if (pad > 0 && pad <= 8)
			des_data.len_to_enc -= pad;
	}
	char	*out_buf = (char *)des_data.full_output;
	size_t	out_len = des_data.len_to_enc;
	/* --- BASE 64 ENCRYPTION */
	if ((data->options & B64) && (data->options & ENCODE))
	{
		char	*b64 = base64_encode(out_buf, out_len);
		free(out_buf);
		out_buf = b64;
		out_len = b64 ? strlen(b64) : 0;
	}
	if (out_buf)
		des_output(data, out_buf, to_encrypt, out_len);
	if (data->raw_key)
		free(data->raw_key);
	return (EXIT_SUCCESS);
}
