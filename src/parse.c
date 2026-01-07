#include "../inc/main.h"

bool	set_option(char *runner, uint16_t *options)
{
	if (strlen(runner) != 2)
		return(ret_err_mess_opt("ft_ssl: '%s': invalid option.", runner));
	switch(*(runner + 1))
	{
		case 'p':	// print input
			*options |= PRINT;
			break ;
		case 'q':	// quiet
			*options |= QUIET;
			break ;
		case 'r':	// reverse format
			*options |= REVERSE;
			break ;
		case 's':	// string
			*options |= STRING;
			break ;
		case 'h':	// help
			printf("Set Usage\n");
			*options |= USAGE;
			break ;
		default:
			return (ret_err_mess_opt("ft_ssl: Invalid option '%s'. Run './ft_ssl -h' to print usage.\n", runner));
	}
	return (0);
}

bool	parser(t_data *data, char **argv)
{
	char	**runner = argv + 1;
	bool	algo = 0;
	
	//	-- ALGORITHM --
	if (runner == NULL || *runner == NULL)
	{
		data->options |= READ_IN;
		return (0);
	}
	else if (**runner != '-')
	{
		data->algo = *(runner++);
		algo = 1;
	}
	//	-- OPTIONS --
	while (*runner && **runner == '-' && !(data->options & (USAGE | STRING)))
	{
		if (**runner == '-' && set_option(*runner, &data->options))
			return (1);
		++runner;
	}
	if (data->options & (QUIET))
		data->options &= ~(PRINT);
	if (data->options & (STRING))
	{
		if (*runner)
			data->string = *(runner++);
		else
			return (ret_err_mess("ft_ssl: '-s' options gets a invalid argument. Run './ft_ssl -h' for usage.\n"));
	}
	if (!algo && !(data->options & USAGE))
		return (ret_err_mess("ft_ssl: no agorithm. Run \"./ft_ssl -h\" for usage\n"));
	data->inputs = runner;
	return (0);
}