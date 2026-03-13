#include "../inc/main.h"

void	exit_free(t_data *data)
{
	if (data->in)
	{
		free(data->in);
		bzero(data, sizeof(t_data));
	}
}

void	exit_err_code(t_data *data, const uint8_t ret_value)
{
	exit_free(data);
	exit(ret_value);
}

void	exit_err_mess(const char *mess, t_data *data, const uint8_t ret_value)
{
	if (write(STDERR_FILENO, mess, strlen(mess)) < 0)
	{
		fprintf(stderr, "Fatal Error: write(): %s\n", strerror(errno));
		exit (EX_OSERR);
	}
	exit_free(data);
	exit (ret_value);
}

void	exit_err_mess_code(const char *mess, const int errnum, t_data *data, const uint8_t ret_value)
{
	fprintf(stderr, "%s: %s(%d)\n", mess, strerror(errnum), errnum);
	exit_free(data);
	exit (ret_value);
}

void	exit_err_mess_opt(const char *mess, const char *opt, t_data *data, const uint8_t ret_value)
{
	fprintf(stderr, mess, opt);
	exit_free(data);
	exit (ret_value);
}

void	exit_err_mess_opt2(const char *mess, const char *opt, const char *opt1, t_data *data, const uint8_t ret_value)
{
	fprintf(stderr, mess, opt, opt1);
	exit_free(data);
	exit (ret_value);
}

bool	ret_err_mess_code(const char *mess, const int errnum)
{
	fprintf(stderr, "%s: %s(%d)\n", mess, strerror(errnum), errnum);
	return (EXIT_FAILURE);
}

bool	ret_err_mess(const char *mess)
{
	fprintf(stderr, "%s\n", mess);
	return (EXIT_FAILURE);
}

bool	ret_err_mess_opt(const char *mess, const char *opt)
{
	fprintf(stderr, mess, opt);
	return (EXIT_FAILURE);
}

void	*ret_err_mess_ptr(const char *mess)
{
	fprintf(stderr, "%s\n", mess);
	return (NULL);
}

void	*ret_err_mess_code_ptr(const char *mess, const int errnum)
{
	fprintf(stderr, "%s: %s(%d)\n", mess, strerror(errnum), errnum);
	return (NULL);
}