#include "../inc/main.h"


bool	setup(t_data *data, char **argv)
{
	if (parser(data, argv))
		return (1);
	else if (!data->inputs || data->options & (PRINT) || (data->options & READ_IN) || is_in_pipe())
	{
		data->options |= READ_IN;
		data->in = read_stdin();
		printf("data->in == %s\n", data->in);
		if (data->in == NULL)
			return (1);
	}
	return (0);
}

int main( int argc, char **argv )
{
	(void) argc;
	t_data	data = {
		.options = ENCODE
	};
	
	if (setup(&data, argv))
		return (1);
}