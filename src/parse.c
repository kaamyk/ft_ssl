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
	
// bool	cphr_set_option(char *runner, uint16_t *options)
// {
// 	switch(*(runner + 1))
// 	{
// 		case 'i':
// 			*options |= IN_FILE; break;
// 		case 'o':
// 			*options |= OUT_FILE; break;
// 		case 'd':
// 			*options |= DECODE; 
// 			*options &= ~(ENCODE);
// 			break;
// 		case 'e':
// 			*options |= ENCODE;
// 			*options &= ~(DECODE);
// 			 break;
// 		case 'a':
// 			*options |= B64; break ;
// 		case 'h':
// 			*options |= USAGE; break;
// 		case 'p':
// 			break ;
// 		case 'k':
// 			break ;
// 		case 's':
// 			break ;
// 		case 'v':
// 			break ;
// 		default :
// 			return (EXIT_FAILURE);
// 	}
// 	return (EXIT_SUCCESS);
// }

bool	cphr_parse_hexa_input(char *input)
{
	if (!input)
		return (EXIT_FAILURE);
	while (*input)
	{
		if (!is_hexa(*input))
			return (EXIT_FAILURE);
		++input;
	}
	return (EXIT_SUCCESS);
}

bool	cphr_parse_pw(char *input)
{
	if (!input)
		return (EXIT_FAILURE);
	while (*input)
	{
		if (!isascii(*input))
		{
			ret_err_mess("ft_ssl: des: invalid password. Run './ft_ssl -h' for usage.\n");
			return (EXIT_FAILURE);
		}
		++input;
	}
	return (EXIT_SUCCESS);
}

char	**cphr_parse_opt(t_data *data, char **runner)
{
	if (strlen(*runner) == 2)
	{
		switch (*((*runner) + 1))	// next char
		{
			case 'i':
				data->options |= IN_FILE;
				if (*(runner + 1) && **(runner + 1) != '-') // next ptr
				{
					data->in_file = *(++runner);
					// data->options &= ~(IN_FILE);
				}
				else
					exit_err_mess_opt("ft_ssl: '%s': invalid argument. Run './ft_ssl -h for usage.", *runner, data, EX_USAGE);
				break ;
			case 'o':
				data->options |= OUT_FILE;
				if (*(runner + 1) && **(runner + 1) != '-')
				{
					data->out_file = *(++runner);
					data->options &= ~(OUT_FILE);
				}
				else
					exit_err_mess_opt2("ft_ssl: '%s': invalid argument. Run './ft_ssl %s -h for usage.", *runner, data->algo, data, EX_USAGE);
				break ;
			case 'd':
				data->options |= DECODE; 
				data->options &= ~(ENCODE);
				break;
			case 'e':
				data->options |= ENCODE;
				data->options &= ~(DECODE);
				 break;
			case 'a':
				data->options |= B64;
				break ;
			case 'h':
			printf("la\n");
			data->options |= USAGE;
				break;
			case 'k':
				// if (!cphr_parse_hexa_input(*(runner + 1)))	// next string pointer
				// 	data->raw_key = *(++runner);
				// else
				// 	exit_err_mess_opt2("ft_ssl: '%s': unvalid argument. Run './ft_ssl %s -h for usage.", *(runner + 1), data->algo, data, EX_USAGE);
				data->raw_key = *(++runner);
				break ;
			case 'p':
				// if (!cphr_parse_pw(*(runner + 1)))
				// 	data->password = *(++runner);
				// else
				// 	exit_err_mess_opt2("ft_ssl: '%s': unvalid argument. Run './ft_ssl %s -h for usage.", *(runner + 1), data->algo, data, EX_USAGE);
				data->password = *(++runner);
				break ;
			case 's':
				// if (!cphr_parse_hexa_input(*(runner + 1)))	// next string pointer
				// 	data->salt = *(++runner);
				// else
				// 	exit_err_mess_opt2("ft_ssl: '%s': unvalid argument. Run './ft_ssl %s -h for usage.", *(runner + 1), data->algo, data, EX_USAGE);
				data->salt = *(++runner);
				break ;
			case 'v':
				// if (!cphr_parse_hexa_input(*(runner + 1)))	// next string pointer
				// 	data->init_vector = *(++runner);
				// else
				// 	exit_err_mess_opt2("ft_ssl: '%s': unvalid argument. Run './ft_ssl %s -h for usage.", *(runner + 1), data->algo, data, EX_USAGE);
				data->init_vector = *(++runner);
				break ;
			default:
				break ;
		}
		// else if (cphr_set_option(*runner, &data->options))
		// 	exit_err_mess_opt2("ft_ssl: Invalid option '%s'. Run './ft_ssl %s -h' to print usage.\n", *runner, data->algo, data, EX_USAGE);
	}
	else 
		exit_err_mess_opt2("ft_ssl: Invalid option. Run './ft_ssl %s -h' to print usage.\n", *runner, data->algo, data, EX_USAGE);
	return (runner);
}

void	cphr_parse_opt_args(t_data *data)
{
	if (data->options & USAGE)
	{
		print_cphr_usage(data);
		exit_err_code(data, EXIT_SUCCESS);
	}
	if (data->raw_key)
	{
		if (cphr_parse_hexa_input(data->raw_key))
			exit_err_mess_opt2("ft_ssl: '%s': unvalid argument. Run './ft_ssl %s -h for usage.", data->raw_key, data->algo, data, EX_USAGE);
	}
	if (data->password)
	{
		if (cphr_parse_hexa_input(data->password))
			exit_err_mess_opt2("ft_ssl: '%s': unvalid argument. Run './ft_ssl %s -h for usage.", data->password, data->algo, data, EX_USAGE);
	}
	if (data->salt)
	{
		if (cphr_parse_hexa_input(data->salt))
			exit_err_mess_opt2("ft_ssl: '%s': unvalid argument. Run './ft_ssl %s -h for usage.", data->salt, data->algo, data, EX_USAGE);
	}
	if (data->init_vector)
	{
		if (cphr_parse_hexa_input(data->init_vector))
			exit_err_mess_opt2("ft_ssl: '%s': unvalid argument. Run './ft_ssl %s -h for usage.", data->init_vector, data->algo, data, EX_USAGE);
	}
	// else if (data->options & IN_FILE)
	// {
	// 	if (*(runner + 1) && **(runner + 1) != '-')
	// 	{
	// 		data->in_file = *(++runner);
	// 		data->options &= ~(IN_FILE);
	// 	}
	// 	else
	// 		exit_err_mess_opt("ft_ssl: '%s': invalid argument. Run './ft_ssl -h for usage.", *runner, data, EX_USAGE);
	// }
	// else if (data->options & OUT_FILE)
	// {
	// 	if (*(runner + 1) && **(runner + 1) != '-')
	// 	{
	// 		data->out_file = *(++runner);
	// 		data->options &= ~(OUT_FILE);
	// 	}
	// 	else
	// 		exit_err_mess_opt2("ft_ssl: '%s': invalid argument. Run './ft_ssl %s -h for usage.", *runner, data->algo, data, EX_USAGE);
	// }
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
		runner = cphr_parse_opt(data, runner);
		++runner;
	}
	cphr_parse_opt_args(data);
	if (!algo && !(data->options & USAGE))
		exit_err_mess("ft_ssl: no agorithm. Run \"./ft_ssl -h\" for usage\n", data, EX_USAGE);
	else if (!(data->options & B64) && !data->raw_key)
		exit_err_mess("ft_ssl: missing key. Run \"./ft_ssl -h\" for usage\n", data, EX_USAGE);
	if (*runner)
		data->inputs = runner;
	return (EXIT_SUCCESS);
}