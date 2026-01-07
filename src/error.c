#include "../inc/main.h"

bool	ret_err_mess(const char *mess)
{
	if (write(STDERR_FILENO, mess, strlen(mess)) < 0)
		fprintf(stderr, "Fatal Error: write(): %s\n", strerror(errno));
	return (1);
}

bool	ret_err_mess_code(const char *mess, const int errnum)
{
	fprintf(stderr, "%s: %s(%d)\n", mess, strerror(errnum), errnum);
	return (1);
}

bool	ret_err_mess_opt(const char *mess, const char *opt)
{
	fprintf(stderr, mess, opt);
	return (1);
}