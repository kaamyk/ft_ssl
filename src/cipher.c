#include "../inc/main.h"

bool	cphr_setup(t_data *data, char **argv)
{
	if (cphr_parser(data, argv))
		return (EXIT_FAILURE);
	if ((!data->inputs && (data->options & ~(ENCODE | DECODE | PRINT)) == 0)
		|| ((!data->inputs || (data->options & (PRINT))) && is_in_pipe()))
	{
		data->options |= READ_IN;
		if ((data->in = read_stdin()) == NULL)
			return (ret_err_mess_code("ft_ssl: fatal error: %s\n", errno));
	}
	return (EXIT_SUCCESS);
}

// bool	cphr_exec(t_data *data, t_algo_fn f)
// {
// 	return (EXIT_SUCCESS);
// }

bool	cphr_main(t_data *data, char **argv)
{
	(void) argv;
	static const t_algo	ciphers[] = {
		{"Base64",	B64Routine	},
		{NULL,		NULL		}
	};
	
	if (cphr_setup(data, argv))
		return (EXIT_FAILURE);
	for (uint8_t i = 0; ciphers[i].name; i++)
	{
		if (!strcmp(ciphers[i].name, data->algo))
		{
			// cphr_exec(data, ciphers[i].fn);
			break ;
		}
	}
	if (data->in)
		free(data->in);
	return (EXIT_SUCCESS);
}