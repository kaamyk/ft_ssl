#include "../inc/main.h"

int main( int argc, char **argv )
{
	(void) argc;
	uint64_t	random_salt = 0;
	int			urandom_fd = open("/dev/urandom", O_RDONLY);
	if (urandom_fd >= 0)
	{
		if (read(urandom_fd, &random_salt, sizeof(random_salt)) < 0)
			random_salt = 0;
		close(urandom_fd);
	}
	t_data	data = {
		.salt = random_salt,
		.salt_len = sizeof(uint64_t),
		.options = READ_IN
	};
	static const t_cmd	cmds[] = {
		{"md5", dgst_main}, 
		{"sha256", dgst_main},
		{"base64", cphr_main},
		{"des", cphr_main},
		{"des-cbc", cphr_main},
		{"des-ecb", cphr_main},
		{NULL, NULL}
	};
	
	if (argc < 2)
	{
		print_ssl_usage(&data);
		return (EX_USAGE);
	}
	if (argv && *argv && *(argv + 1))
	{
		if (**(argv + 1) != '-')
			data.algo = *(argv + 1);
		else if (!strcmp(*(argv + 1), "-h"))
		{
			print_ssl_usage(&data);
			return (0);
		}
		else
			exit_err_mess("ft_ssl: invalid algorithm. Run './ft_ssl -h' for usage\n", &data, EX_USAGE);
	}
	for (uint8_t i = 0; cmds[i].name; i++)
	{
		if (!strcmp(cmds[i].name, data.algo))
			return(cmds[i].fn(&data, argv));
	}
	exit_free(&data);
	if (data.algo)
		exit_err_mess_opt("ft_ssl: '%s': invalid command. Available commands: md5, sha256, base64, des, des-cbc, des-ecb. Run './ft_ssl -h' for usage\n", data.algo, &data, EX_USAGE);
	else
		exit_err_mess("ft_ssl: no algorithm given. Run './ft_ssl -h' for usage\n", &data, EX_USAGE);
}