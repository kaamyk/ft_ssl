#include "../inc/main.h"

	/* === DIGEST === */
bool	dgst_set_option(char *runner, uint16_t *options)
{
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
			return (ret_err_mess_opt("ft_ssl: invalid options '%s'. Run \"./ft_ssl -h\" for usage.", runner));
	}
	return (EXIT_SUCCESS);
}

bool	dgst_parser(t_data *data, char **argv)
{
	char	**runner = argv + 1;
	bool	algo = 0;
	
	//	-- ALGORITHM --
	if (runner == NULL || *runner == NULL)
		exit_err_mess("ft_ssl: No argument. Run './ft_ssl -h' for usage.\n", data, EX_USAGE);
	else if (**runner != '-')
	{
		data->algo = *(runner++);
		algo = 1;
	}
	//	-- OPTIONS --
	while (*runner && **runner == '-' && !(data->options & (USAGE | STRING)))
	{
		if (**runner == '-')
		{
			if (strlen(*runner) != 2)
				exit_err_mess_opt("ft_ssl: '%s': invalid option.", *runner, data, EX_USAGE);
			if (dgst_set_option(*runner, &data->options))
				exit_err_mess_opt("ft_ssl: Invalid option '%s'. Run './ft_ssl -h' to print usage.\n", *runner, data, EX_USAGE);
		}
		++runner;
	}
	if (data->options & (QUIET))
		data->options &= ~(PRINT);
	if (data->options & (STRING))
	{
		data->inputs = runner;
		if (!data->inputs)
			exit_err_mess("ft_ssl: '-s' options gets a invalid argument. Run './ft_ssl -h' for usage.\n", data, EX_USAGE);
	}
	if (!algo && !(data->options & USAGE))
		exit_err_mess("ft_ssl: no agorithm. Run \"./ft_ssl -h\" for usage\n", data, EX_USAGE);
	if (runner && *runner)
		data->inputs = runner;
	return (EXIT_SUCCESS);
}

	/* === CIPHER === */