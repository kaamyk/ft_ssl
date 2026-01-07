#include "../inc/main.h"

void	exit_free(t_data *data)
{
	if (data->in)
		free(data->in);
}

void	exit_err_code(t_data *data, const uint8_t ret_value)
{
	exit_free(data);
	exit(ret_value);
}

void	exit_err_mess(const char *mess, t_data *data, const uint8_t ret_value)
{
	exit_free(data);
	if (write(STDERR_FILENO, mess, strlen(mess)) < 0)
	{
		fprintf(stderr, "Fatal Error: write(): %s\n", strerror(errno));
		exit (EX_OSERR);
	}
	exit (ret_value);
}

void	exit_err_mess_code(const char *mess, const int errnum, t_data *data, const uint8_t ret_value)
{
	exit_free(data);
	fprintf(stderr, "%s: %s(%d)\n", mess, strerror(errnum), errnum);
	exit (ret_value);
}

void	exit_err_mess_opt(const char *mess, const char *opt, t_data *data, const uint8_t ret_value)
{
	exit_free(data);
	fprintf(stderr, mess, opt);
	exit (ret_value);
}

bool	ret_err_mess_code(const char *mess, const int errnum)
{
	fprintf(stderr, "%s: %s(%d)\n", mess, strerror(errnum), errnum);
	return (1);
}