#include "../inc/main.h"

	/* === DIGEST === */
bool	dgst_set_option(char *runner, uint16_t *options)
{
	switch(*(runner + 1))
	{
		case 'p':	// print input
			*options |= PRINT; break ;
		case 'q':	// quiet
			*options |= QUIET; break ;
		case 'r':	// reverse format
			*options |= REVERSE; break ;
		case 's':	// string
			*options |= STRING; break ;
		case 'h':	// help
			*options |= USAGE; break ;
		default:
			return (ret_err_mess_opt("ft_ssl: invalid options '%s'. Run \"./ft_ssl -h\" for usage.", runner));
	}
	return (EXIT_SUCCESS);
}

bool	dgst_parser(t_data *data, char **argv)
{
	char	**runner = argv + 1;
	char	*stdin = NULL;
	bool	algo = 0;
	
	//	-- ALGORITHM --
	if (runner == NULL || *runner == NULL)
		exit_err_mess("ft_ssl: no argument. Run './ft_ssl -h' for usage.\n", data, EX_USAGE);
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
	if (data->options & (STRING))
	{
		if (!*runner)
			exit_err_mess("ft_ssl: '-s' options gets a invalid argument. Run './ft_ssl -h' for usage.\n", data, EX_USAGE);
	}
	if (!algo && !(data->options & USAGE))
		exit_err_mess("ft_ssl: no agorithm. Run \"./ft_ssl -h\" for usage\n", data, EX_USAGE);
	if (*runner)
		data->inputs = runner;
	return (EXIT_SUCCESS);
}

	/* === CIPHER === */
	
	bool	cphr_set_option(char *runner, uint16_t *options)
	{
		switch(*(runner + 1))
		{
			case 'i':
				*options |= IN_FILE; break;
			case 'o':
				*options |= OUT_FILE; break;
			case 'd':
				*options |= DECODE; 
				*options &= ~(ENCODE);
				break;
			case 'e':
				*options |= ENCODE;
				*options &= ~(DECODE);
				 break;
			case 'h':
				*options |= USAGE; break;
			default :
				return (EXIT_FAILURE);
		}
		return (EXIT_SUCCESS);
	}
	
	bool	cphr_parser(t_data *data, char **argv)
	{
		char	**runner = argv + 1;
		char	*stdin = NULL;
		bool	algo = 0;
		
		//	-- ALGORITHM --
		if (runner == NULL || *runner == NULL)
			exit_err_mess("ft_ssl: no argument. Run './ft_ssl -h' for usage.\n", data, EX_USAGE);
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
				if (strlen(*runner) != 2 || cphr_set_option(*runner, &data->options))
					exit_err_mess_opt2("ft_ssl: Invalid option '%s'. Run './ft_ssl %s -h' to print usage.\n", *runner, data->algo, data, EX_USAGE);
			}
			if (data->options & USAGE)
			{
				print_cphr_usage(data);
				exit_err_code(data, EXIT_SUCCESS);
			}
			else if (data->options & IN_FILE)
			{
				if (*(runner + 1) && **(runner + 1) != '-')
				{
					data->in_file = *(++runner);
					data->options &= ~(IN_FILE);
				}
				else
					exit_err_mess_opt("ft_ssl: '%s': invalid argument. Run './ft_ssl -h for usage.", *runner, data, EX_USAGE);
			}
			else if (data->options & OUT_FILE)
			{
				if (*(runner + 1) && **(runner + 1) != '-')
				{
					data->out_file = *(++runner);
					data->options &= ~(OUT_FILE);
				}
				else
					exit_err_mess_opt2("ft_ssl: '%s': invalid argument. Run './ft_ssl %s -h for usage.", *runner, data->algo, data, EX_USAGE);
			}
			++runner;
		}
		if (!algo && !(data->options & USAGE))
			exit_err_mess("ft_ssl: no agorithm. Run \"./ft_ssl -h\" for usage\n", data, EX_USAGE);
		if (*runner)
			data->inputs = runner;
		printf("CIPHER SETUP :\n\tIn file: %s\n\tOut file: %s\n\tOptions:\n\t\tDecode: %d\n\t\tEncode: %d\n\t\tInfile: %d\n\t\tOutfile: %d\n",
				data->in_file, data->out_file, data->options & DECODE, data->options & ENCODE, data->options & IN_FILE, data->options & OUT_FILE);
		return (EXIT_SUCCESS);
	}