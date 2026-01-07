#include "../inc/main.h"

char	*ft_strjoin(char const *s1, char const *s2)
{
	char	*result;
	int		t_len;
	int		i;

		// if (!s1 || !s2)
		// 	return (NULL);
	if (s1 != NULL && s2 == NULL)
		t_len = strlen(s1) + 1;
	else if (s1 == NULL && s2 != NULL)
		t_len = strlen(s2) + 1;
	else
		t_len = strlen(s1) + strlen(s2) + 1;
	result = malloc(sizeof(char) * t_len);
	if (!result)
		return (NULL);
	i = 0;
	if (s1 != NULL)
	{
		while (*s1)
		{
			result[i] = *s1;
			s1++;
			i++;
		}
	}
	if (s2 != NULL)
	{
		while (*s2)
		{
			result[i] = *s2;
			s2++;
			i++;
		}
	}
	result[i] = 0;
	return (result);
}
