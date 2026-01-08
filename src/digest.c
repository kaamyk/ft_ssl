#include "../inc/main.h"

bool	dgst_setup(t_data *data, char **argv)
{
	if (dgst_parser(data, argv))
		return (EXIT_FAILURE);
	if ((!data->inputs || (data->options & (PRINT))) && is_in_pipe())
	{
		data->options |= READ_IN;
		data->in = read_stdin();
		if (data->in == NULL)
			return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}

bool	dgst_exec(t_data *data, t_algo_fn f)
{
	char		**runner = data->inputs;
	char		*to_hash = NULL;
	
	if (data->options & READ_IN)
	{
		if (f(data, "stdin", data->in))
			exit_err_code(data, EX_OSERR);
	}
	if (!runner)
		return (EXIT_SUCCESS);
	while (*runner)
	{
		if (data->options & STRING)
		{
			to_hash = *runner;
			data->options &= ~(STRING);
		}
		else 
		{
			to_hash = file_to_str(*runner);
			if (to_hash == NULL)
				exit(EX_OSERR);
		}
		if (f(data, *runner, to_hash))
			exit_err_mess("Leaving.", data, EX_OSERR);
		free(to_hash);
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
			dgst_exec(data, digests[i].fn);
	}
	if (data->in)
		free(data->in);
	return (EXIT_SUCCESS);
}