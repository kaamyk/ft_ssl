#include "../inc/main.h"

char 	*read_stdin( void )
{
	char		buf[256] = {0};
	char	*tmp = NULL;
	char	*input = calloc(1, 1);
	if (input == NULL)
	{
		fprintf(stderr, "Fatal Error: calloc: %s\n", strerror(errno));
		return (NULL);
	}
	
	while (fread(buf, 1, 255, stdin) > 0)
	{
		tmp = input;
		if ((input = ft_strjoin(input, buf)) == NULL)
		{
			fprintf(stderr, "Fatal Error: ft_strjoin(): %s\n", strerror(errno));
			return (NULL);
		}
		free(tmp);
		bzero((char *) buf, 256);
	}
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