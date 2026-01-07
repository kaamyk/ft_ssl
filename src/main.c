#include "../inc/main.h"

int main( int argc, char **argv )
{
	(void) argc;
	t_data	data = {
		.options = ENCODE
	};
	static const t_cmd	cmds[] = {
		{"md5", dgst_main}, 
		{"sha256", dgst_main},
		{NULL, NULL}
	};
	
	
	if (argv && *argv && *(argv + 1) && **(argv + 1) != '-')
		data.algo = *(argv + 1);
	else
	{
		print_usage();
		return (EX_USAGE);
	}
	for (uint8_t i = 0; cmds[i].name; i++)
	{
		if (!strcmp(cmds[i].name, data.algo))
			return(cmds[i].fn(&data, argv));
	}
	if (data.algo)
		exit_err_mess_opt("ft_ssl: '%s': Unvalid argument. Run './ft_ssl -h' for usage\n", data.algo, &data, EX_USAGE);
	else
		exit_err_mess("ft_ssl: no algorithm given. Run './ft_ssl -h' for usage\n", &data, EX_USAGE);
}