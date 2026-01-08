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

void	dgst_launch_algo(t_data *data)
{
	static const t_algo	digests[] = {
		{"md5", MDMain},
		// {"sha256", SHAString(data)}
	};
	
	for (uint8_t i = 0; digests[i].name; i++)
	{
		if (!strcmp(digests[i].name, data->algo))
		{
			digests[i].fn(data);
			return ;
		}
	}
}

bool	dgst_main(t_data *data, char **argv)
{
	if (!dgst_setup(data, argv))
		dgst_launch_algo(data);
	if (data->in)
		free(data->in);
	return (EXIT_SUCCESS);
}