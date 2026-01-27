#include "../inc/main.h"

void	print_ssl_usage(t_data *data)
{
	if (write(STDOUT_FILENO, "ft_ssl usage:\n\t[string | ]./ft_ssl algorithm [-options arguments] arguments\nType './ft_ssl algoname -h' for specific algorithm usage\nalgorithm values : selects the hash algorithm to run\n\tDigest:\n\t\tsha256\n\t\tmd5\n\tCipher:\n\t\tbase64\n\toptions\n\t\t-h : print usage\n", 257) < 0)	
		exit_err_mess_code("Fatal error: write(): %s (%d)\n", errno, data, EX_OSERR);
}

void	print_dgst_usage(t_data *data)
{
	if (write(STDOUT_FILENO, "ft_ssl digest usage:\n\t[string | ]./ft_ssl algorithm [options] [-s \"string\"] arguments\nalgorithm values : selects the hash algorithm to run\n\tsha256\n\tmd5\noptions: \n\t-s : print the sum of the given string. Any argument after this option is consider as a string. Each argument after the string is consider as a filename (see arguments section).\n\t-h : print usage\n\t-p : print stdin to stdout and append the checksum to stdout. Truncated if length greater than 40 bytes or at the first new line\n\t-q : quiet mode\n\t-r : reverse the format of the ouput\n\targuments :\n\t Each is considered has a file name. The command tries to open each file, if it fails it goes on.\n", 650) < 0)
		exit_err_mess_code("Fatal error: write(): %s (%d)\n", errno, data, EX_OSERR);
}

void	print_cphr_usage(t_data *data)
{
	if (write(STDOUT_FILENO, "ft_ssl ciphers usage:\n\t[string | ]./ft_ssl ciphername [options] [-s \"string\"] arguments\n\tciphername : selects the cipher algorithm to run\n\t\tBase64\noptions: \n\t\t-i filename: selects 'filename' as an input file\n\t\t-o filename: command's output will be written in 'filename'\n\t\t-d : decode the input\n\t\t-e : encode the input\n", 319) < 0)
	exit_err_mess_code("Fatal error: write(): %s (%d)\n", errno, data, EX_OSERR);
}

bool	stdin_header_display(const uint8_t options, const char *to_hash)
{
	size_t	to_print_size = strlen(to_hash);
	char	*nl = strchr(to_hash, '\n');

	if (nl)
	{
		if (nl - to_hash > 40)
			to_print_size = 40;
		else
			to_print_size = nl - to_hash;
		
	}
	if (options & PRINT)
	{
		if (write(STDOUT_FILENO, "(\"", 2) < 0 \
			|| write(STDOUT_FILENO, to_hash, to_print_size) < 0)
			return (ret_err_mess_code("ft_ssl: write() :", errno));
		if ((nl - to_hash > 40 || (nl && *(nl + 1))) 
			 && write(STDOUT_FILENO, "...", 3) < 0)
			return (ret_err_mess_code("ft_ssl: write() :", errno));
		if (write(STDOUT_FILENO, "\")= ", 4) < 0)
			return (ret_err_mess_code("ft_ssl: write() :", errno));
	}
	else
	{
		if (write(STDOUT_FILENO, "(stdin)= ", 10) < 0)
			return (ret_err_mess_code("ft_ssl: write():", errno));
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
	// options = (options & ~(ENCODE | DECODE));
	if (!(options & QUIET))
	{
		if (!(options & (REVERSE)) || (options & READ_IN))
		// if (!(options & (REVERSE | QUIET)))
		{
			if (write(STDOUT_FILENO, algoname, strlen(algoname)) < 0)
				return (ret_err_mess_code("ft_ssl: fatal error:", errno));
			if (options & (READ_IN))
			{
				if (stdin_header_display(options, to_hash))
					return (EXIT_FAILURE);
			}
			else if (!(options & (REVERSE)))
			{
				if (header_display(options, filename, to_hash))
					return (EXIT_FAILURE);
			}
		}	
	}
	for (uint8_t i = 0; i < hssz; i++)
		if (printf ("%02x", digest[i]) < 0)
			return (ret_err_mess_code("ft_ssl: fatal error:", errno));
	if (((options & (REVERSE | READ_IN | QUIET)) == REVERSE)
		&& footer_display(filename, to_hash))
			return (EXIT_FAILURE);
	fflush(stdout);
	if (write (STDOUT_FILENO, "\n", 1) < 0)
		return (ret_err_mess_code("ft_ssl: fatal error:", errno));
	return (EXIT_SUCCESS);
}
