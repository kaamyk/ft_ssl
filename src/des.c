#include "../inc/main.h"

bool 	DESRoutine(const t_data *data, const char *runner, const char *to_hash)
{
	(void)runner;
	char	*res = NULL;
	
	if (data->options & B64)
	{
		if (data->options & DECODE)
			res = base64_decode(to_hash);
		else if (data->options & ENCODE)
			res = base64_encode(to_hash);
	}
	if (cphr_display(res))
		return (EXIT_FAILURE);
	free(res);
	return (EXIT_SUCCESS);
}