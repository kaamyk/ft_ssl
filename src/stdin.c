#include "../inc/main.h"

char 	*read_stdin(size_t *len_out)
{
	char	buf[256];
	char	*input = calloc(1, 1);
	char	*tmp = NULL;
	size_t	total = 0;
	size_t	nread = 0;

	if (input == NULL)
	{
		fprintf(stderr, "Fatal Error: calloc: %s\n", strerror(errno));
		return (NULL);
	}
	while ((nread = fread(buf, 1, 255, stdin)) > 0)
	{
		tmp = malloc(total + nread + 1);
		if (tmp == NULL)
		{
			free(input);
			return (ret_err_mess_code_ptr("ft_ssl: malloc", errno));
		}
		memcpy(tmp, input, total);
		memcpy(tmp + total, buf, nread);
		total += nread;
		tmp[total] = 0;
		free(input);
		input = tmp;
	}
	if (len_out)
		*len_out = total;
	return (input);
}

bool	is_in_pipe( void )
{
	struct stat	st = {0};
	
	if (fstat(STDIN_FILENO, &st) == -1)
	{
		fprintf(stderr, "ft_ssl: is_in_pipe(): fstat(): %s\n", strerror(errno));
		return (false);
	}
	return (S_ISFIFO(st.st_mode) || S_ISSOCK(st.st_mode));
}