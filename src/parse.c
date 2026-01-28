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

	/* ============== */
	/* === CIPHER === */
	/* ============== */
	
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
		case 'a':
			*options |= B64; break ;
		case 'h':
			*options |= USAGE; break;
		default :
			return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}

bool	cphr_parse_key(char *input)
{
	if (!input)
		return (EXIT_FAILURE);
	while (*input)
	{
		if (!is_hexa(*input))
		{
			ret_err_mess("ft_ssl: des: invalid key. Run './ft_ssl -h' for usage.\n");
			return (EXIT_FAILURE);
		}
	}
	return (EXIT_SUCCESS);
}

char	**cphr_parse_opt(t_data *data, char **runner)
{
	// PASTE opt parsing to reduce cphr_parser();
	return (runner);
}

char	**cphr_parse_opt_args(t_data *data, char **runner)
{
	// PASTE opt argumehts parsing to reduce cphr_parser();
	return (runner);
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
			if (strlen(*runner) == 2)
			{
				if (*((*runner) + 1) == 'k')	// next char
				{
					if (!cphr_parse_key(*(runner + 1)))	// next string pointer
					{
						printf(">> DEFINING RAW KEY\n");
						data->raw_key = *(++runner);
					}
					else
						exit_err_code(data, EX_USAGE);
				}
				else if (cphr_set_option(*runner, &data->options))
					exit_err_mess_opt2("ft_ssl: Invalid option '%s'. Run './ft_ssl %s -h' to print usage.\n", *runner, data->algo, data, EX_USAGE);
			}
			else 
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
	printf("data->raw_key == %s\n", data->raw_key);
	return (EXIT_SUCCESS);
}