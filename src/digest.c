#include "../inc/main.h"

bool	dgst_setup(t_data *data, char **argv)
{
	if (dgst_parser(data, argv))
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

// bool	dgst_stdin(t_data *data, t_algo_fn f)
// {
// 	char	*strin = NULL;
// 	char	buf[256] = {0};
	
// 	while (fgets(buf, 255, stdin))
// 	{
// 		strin = ft_strjoin(strin, buf);
// 		bzero(buf, 256);
// 	}
// 	data->inputs = ft_split(strin, '\n');
// 	while (*data->inputs)
// 	{
// 		if (f(data, *data->inputs, *data->inputs))
// 			exit_err_code(data, EX_OSERR);
// 		++data->inputs;
// 	}
// 	free(strin);
// 	return (EXIT_SUCCESS);
// }

bool	dgst_exec(t_data *data, t_algo_fn f)
{
	char		**runner = data->inputs;
	char		*to_hash = NULL;
	
	if (data->options & READ_IN)
	{
		if (data->in)
		{
			if (f(data, "stdin", data->in))
				exit_err_code(data, EX_OSERR);
		}
		// else
		// 	return (dgst_stdin(data, f));
		data->options &= ~(READ_IN);
	}
	if (!runner)
		return (EXIT_SUCCESS);
	while (*runner)
	{
		if (data->options & STRING)
		{
			if ((to_hash = strdup(*runner)) == NULL)
				exit_err_mess_code("ft_ssl: fatal error:", errno, data, EX_OSERR);
		}
		else 
			to_hash = file_to_str(*runner);
		if (to_hash)
		{
			if (f(data, *runner, to_hash))
				exit_err_code(data, EX_OSERR);
			free(to_hash);
		}
		data->options &= ~(STRING);
		++runner;
	}
	return (EXIT_SUCCESS);
}

bool	dgst_main(t_data *data, char **argv)
{
	static const t_algo	digests[] = {
		{"md5",		MDRoutine	},
		{"sha256",	SHARoutine	},
		{NULL,		NULL		}
	};
	
	if (dgst_setup(data, argv))
		return (EXIT_FAILURE);
	for (uint8_t i = 0; digests[i].name; i++)
	{
		if (!strcmp(digests[i].name, data->algo))
		{
			dgst_exec(data, digests[i].fn);
			break ;
		}
	}
	if (data->in)
		free(data->in);
	return (EXIT_SUCCESS);
}