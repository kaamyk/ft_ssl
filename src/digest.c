#include "../inc/main.h"

bool	dgst_setup(t_data *data, char **argv)
{
	if (dgst_parser(data, argv))
		return (1);
	else if ((!data->inputs || data->options & (PRINT | READ_IN)) && is_in_pipe())
	{
		data->options |= READ_IN;
		data->in = read_stdin();
		if (data->in == NULL)
			return (1);
	}
	return (0);
}

bool	dgst_launch_algo(t_data *data)
{
	static const t_algo	digests[] = {
		{"md5", MDMain},
		// {"sha256", SHAString(data)}
	};
	
	for (uint8_t i = 0; digests[i].name; i++)
	{
		if (!strcmp(digests[i].name, data->algo))
			return(digests[i].fn(data));
	}
	return (0);
}

bool	dgst_main(t_data *data, char **argv)
{
	if (dgst_setup(data, argv) || dgst_launch_algo(data))
		return (1);
	return (0);
}