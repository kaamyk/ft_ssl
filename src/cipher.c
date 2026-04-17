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
		data->in_len = data->len_file;
		if (f(data, data->in_file, data->in))
			exit_err_code(data, EX_OSERR);
	}
	return (EXIT_SUCCESS);
}

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
			if (cphr_exec(data, ciphers[i].fn))
				return (EXIT_FAILURE);
			break ;
		}
	}
	return (EXIT_SUCCESS);
}
