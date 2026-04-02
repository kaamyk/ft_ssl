#include "../inc/main.h"

bool	cphr_setup(t_data *data, char **argv)
{
	size_t	i = 0;
	size_t	j = 0;
	size_t	len = 0;
	
	data->options |= ENCODE;
	if (cphr_parser(data, argv))
		return (EXIT_FAILURE);
	if (!(data->options & ~(DECODE | ENCODE | B64 | PWP))) // if no other flags are set
	{
		data->options |= READ_IN;
		if ((data->in = read_stdin(&data->in_len)) == NULL)
			return (ret_err_mess_code("ft_ssl: fatal error: %s\n", errno));
		//	-- Delete whitespaces
		len = strlen(data->in);
		if ((data->options & B64) && (data->options & DECODE))
		{
			while (j < len)
			{
				while (data->in[j] && is_whitespace(data->in[j]))
					++j;
				data->in[i] = data->in[j];
				++i;
				++j;
			}
			bzero(data->in + i, j - i);
			data->in_len = strlen(data->in);
		}
	}
	return (EXIT_SUCCESS);
}

bool	cphr_exec(t_data *data, t_algo_fn f)
{
	if (data->options & READ_IN)
	{
		if (data->in)
		{
			if (f(data, "stdin", data->in))
				exit_err_code(data, EX_OSERR);
		}
		data->options &= ~(READ_IN);
	}
	if (data->in_file)
	{
		data->in = file_to_str(data->in_file);
		if (!data->in)
			exit_err_mess_code("ft_ssl: fatal error:", errno, data, EX_OSERR);
		if (f(data, data->in_file, data->in))
			exit_err_code(data, EX_OSERR);
	}
	return (EXIT_SUCCESS);
}

bool	cphr_main(t_data *data, char **argv)
{
	static const t_algo	ciphers[] = {
		{"base64",	B64Routine	},
		{"des",		des_routine	},
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
