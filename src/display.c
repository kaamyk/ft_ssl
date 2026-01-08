#include "../inc/main.h"

void	print_usage(t_data *data)
{
	if (write(STDOUT_FILENO, "ft_ssl usage:\n\techo string | ./ft_ssl algorithm [options] [-s \"string\"] arguments\nalgorithm values : selects the hash algorithm to run\n\tsha256\n\tmd5\noptions: \n\tsets the display format :\n\t\t-p : print stdin to stdout and append the checksum to stdout.\n\t\t-q : quiet mode\n\t\t-r : reverse the format of the ouput\n\t-s : print the sum of the given string. Any argument after this option is consider as a string. Each argument after the string is consider as a filename (see arguments section).\n\t-h : print usage\narguments :\n\t Each is considered has a file name. The command tries to open each file, if it fails it goes on.\n", 615) < 0)
		exit_err_mess_code("Fatal error: write(): %s (%d)\n", errno, data, EX_OSERR);
	
	return ;
}

bool	stdin_header_display(const uint8_t options, const char *to_hash)
{
	size_t	to_print_size = strlen(to_hash);
	char	*nl = strchr(to_hash, '\n');

	if (nl)
	{
		if (nl - to_hash > 20)
			to_print_size = 20;
		else
			to_print_size = nl - to_hash - 1;
		
	}
	// else if (to_hash[to_print_size - 1] == '\n')
	// 	--to_print_size;
	if (options & PRINT)
	{
		if (write(STDOUT_FILENO, "(\"", 2) < 0 \
			|| write(STDOUT_FILENO, to_hash, to_print_size) < 0 \
			|| write(STDOUT_FILENO, "\")= ", 4) < 0)
		return (EXIT_FAILURE);
	}
	else
	{
		if (write(STDOUT_FILENO, "(stdin)= ", 10) < 0)
			return (ret_err_mess_code("ft_ssl: ", errno));
	}
	return (EXIT_SUCCESS);
}

bool	header_display(const uint8_t options, const char *name , const char *to_hash)
{
	if ((options & STRING))
	{
		if (printf("(\"%s\")= ", to_hash) < 0)
			return (ret_err_mess_code("ft_ssl: fatal error:", errno));
	}
	else if (name)
	{
		if (printf("(%s)= ", name) < 0)
			return (ret_err_mess_code("ft_ssl: fatal error:", errno));
	}
	else
		return (ret_err_mess("ft_ssl: unexpected NULL pointer. Leaving."));
	return (EXIT_SUCCESS);
}

bool	footer_display(const char *name, const char *to_hash)
{
	if (name != NULL)
	{
		if (printf(" %s", name) < 0)
			return (ret_err_mess_code("ft_ssl: fatal error:", errno));
	}
	else if (to_hash != NULL)
	{
		if (printf(" \"%s\"", to_hash) < 0)
			return (ret_err_mess_code("ft_ssl: fatal error:", errno));
	}
	else
		return (ret_err_mess("ft_ssl: unexpected NULL pointer. Leaving."));
	return (EXIT_SUCCESS);
}

bool	dgst_display(const uint8_t digest[16], uint16_t options, const char *filename, const char *to_hash, const char *algoname, const uint8_t hssz)
{
	if (!(options & QUIET))
	{
		if (printf("%s", algoname) < 0)
			return (ret_err_mess_code("ft_ssl: fatal error:", errno));
		if ((options & READ_IN))
		{
			if (stdin_header_display(options, to_hash))
				return (ret_err_mess_code("ft_ssl: fatal error:", errno));
		}
		else if (!(options & REVERSE))
		{
			if (header_display(options, filename, to_hash))
				return (EXIT_FAILURE);
		}
	}
	for (uint8_t i = 0; i < hssz; i++)
		if (printf ("%02x", digest[i]) < 0)
			return (ret_err_mess_code("ft_ssl: fatal error:", errno));
	if ((options & REVERSE) && !(options & READ_IN)
		&& footer_display(filename, to_hash))
			return (EXIT_FAILURE);
	fflush(stdout);
	if (write (STDOUT_FILENO, "\n", 1) < 0)
		return (ret_err_mess_code("ft_ssl: fatal error:", errno));
	return (EXIT_SUCCESS);
}

// bool	MDDisplay(const uint8_t digest[16], uint16_t options, const char *name, const char *to_hash)
// {
// 	printf("MDDisplay() :\n\tREAD_IN == %d\n\tPRINT == %d\n\tSTRING == %d\n", options & READ_IN, options & PRINT, options & STRING);
// 	if (!(options & QUIET))
// 	{
// 		if (write(STDOUT_FILENO, "MD5", 4) < 0)
// 			return (ret_err_mess_code("ft_ssl: fatal error:", errno));
// 		if ((options & READ_IN))
// 		{
// 			if (stdin_header_display(options, to_hash))
// 				return (ret_err_mess_code("ft_ssl: fatal error:", errno));
// 		}
// 		else if (!(options & REVERSE))
// 		{
// 			if (header_display(options, name, to_hash))
// 				return (EXIT_FAILURE);
// 		}
// 	}
// 	for (uint8_t i = 0; i < MD5_HSSZ; i++)
// 		if (printf ("%02x", digest[i]) < 0)
// 			return (ret_err_mess_code("ft_ssl: fatal error:", errno));
// 	if ((options & REVERSE) && !(options & READ_IN)
// 		&& footer_display(name, to_hash))
// 			return (EXIT_FAILURE);
// 	fflush(stdout);
// 	if (write (STDOUT_FILENO, "\n", 1) < 0)
// 		return (ret_err_mess_code("ft_ssl: fatal error:", errno));
// 	return (EXIT_SUCCESS);
// }