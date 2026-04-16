#include "../inc/main.h"

char	*cphr_getstdin(uint16_t *options, size_t *len_out)
{
	size_t	i = 0;
	size_t	j = 0;
	size_t	len = 0;
	char	*res = 0;

	if ((res = read_stdin(&len)) == NULL)
		return (ret_err_mess_code_ptr("ft_ssl: fatal error: %s\n", errno));
	if ((*options & B64) && (*options & DECODE))
	{
		len = strlen(res);
		while (j < len)
		{
			while (res[j] && is_whitespace(res[j]))
				++j;
			res[i] = res[j];
			++i;
			++j;
		}
		bzero(res + i, j - i);
		len = strlen(res);	// re-measure after stripping (handles trailing whitespace)
	}
	if (len_out)
		*len_out = len;
	return (res);
}

bool	cphr_setup(t_data *data, char **argv)
{
	data->options |= ENCODE;
	data->options |= READ_IN;
	if (cphr_parser(data, argv))
		return (EXIT_FAILURE);
	return (EXIT_SUCCESS);
}

bool	cphr_exec(t_data *data, t_algo_fn f)
{
	if (data->options & READ_IN || data->options & PWP)
	{
		if (f(data, "stdin", NULL))
			exit_err_code(data, EX_OSERR);
		data->options &= ~(READ_IN);
	}
	if (data->in_file)
	{
		data->in = file_to_str(data->in_file, &data->len_file);
		if (!data->in)
			return (EXIT_FAILURE);
		if (f(data, data->in_file, data->in))
			exit_err_code(data, EX_OSERR);
	}
	return (EXIT_SUCCESS);
}

// bool	cphr_output(t_data *data, uint8_t *output, size_t out_len, uint8_t *to_encrypt, size_t to_enc_len)
// {
// 	int	out_fd = STDOUT_FILENO;
	
// 	if ((data->options & OUT_FILE) && data->out_file)
// 	{
// 		out_fd = open(data->out_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
// 		if (out_fd < 0)
// 			ret_err_mess_code(, EX_OSERR);
// 	}
// 	if ((data->options & B64) && (data->options & ENCODE))
// 	{
// 		char	*buf = NULL;
// 		size_t	buf_l = 0;
// 		char *runner = NULL;
// 		char *last_b64 = NULL;
		
// 		buf = base64_encode((char *)output, out_len);
// 		if (!buf)
// 			return (EXIT_FAILURE);
// 		buf_l = strlen(buf);
// 		runner = buf;
// 		last_b64 = buf + buf_l;
		
// 		while (last_b64 - runner >= 64)
// 		{
// 			if (write(out_fd, runner, 64) < 0
// 			 || write(out_fd, "\n", 1) < 0)
// 				return (ret_err_mess_code("ft_ssl: write() :", errno));
// 			runner += 64;
// 		}
// 		if (runner < last_b64)
// 		{
// 			if (write(out_fd, runner, last_b64 - runner) < 0
// 			 || write(out_fd, "\n", 1) < 0)
// 			return (ret_err_mess_code("ft_ssl: write() :", errno));
// 		}
// 		free(buf);
// 	}
// 	else
// 		if (write(out_fd, output, out_len) < 0)
// 		return (ret_err_mess_code("ft_ssl: write() :", errno));
// 	free(output);
// 	if (out_fd != STDOUT_FILENO)
// 		close(out_fd);
// 	free(to_encrypt);
// 	return (EXIT_SUCCESS);
// }

bool	cphr_main(t_data *data, char **argv)
{
	static const t_algo	ciphers[] = {
		{"base64",	b64_routine	},
		{"des",		des_routine	},
		{"des-ecb",	des_routine	},
		{"des-cbc",	des_routine	},
		{NULL,		NULL		}
	};
	(void)argv;

	if (cphr_setup(data, argv))
		return (EXIT_FAILURE);
	for (uint8_t i = 0; ciphers[i].name; i++)
	{
		if (!strcmp(ciphers[i].name, data->algo))
		{
			cphr_exec(data, ciphers[i].fn);
			break ;
		}
	}
	return (EXIT_SUCCESS);
}
