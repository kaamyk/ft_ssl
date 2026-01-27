#include "../inc/main.h"

inline uint32_t	rotate_left(uint8_t bits, uint32_t word) { return ((word << bits | (word >> (32 - bits)))); }
inline uint32_t	rotate_right(uint8_t bits, uint32_t word) { return ((word >> bits) | (word << (32 - bits))); }
inline uint32_t	shift_right(uint8_t bits, uint32_t word) { return (word >> bits); }

void	decode(uint32_t *output, uint8_t *input, uint32_t len)
{
	for (uint32_t i = 0, j = 0; j < len; i++, j += 4)
	{
		output[i] = ((uint32_t)input[j]) | (((uint32_t)input[j + 1]) << 8) | (((uint32_t)input[j + 2]) << 16) | (((uint32_t)input[j + 3]) << 24);
	}
}

void	encode(uint8_t *output, uint32_t *input, uint32_t len)
{
	for (uint32_t i = 0, j = 0; j < len; i++, j += 4)
	{
		output[j] = (uint8_t)(input[i] & 0xff);
		output[j + 1] = (uint8_t)((input[i] >> 8) & 0xff);
		output[j + 2] = (uint8_t)((input[i] >> 16) & 0xff);
		output[j + 3] = (uint8_t)((input[i] >> 24) & 0xff);
	}
}

char	*file_to_str(const char *filename)
{
	char	*content = NULL;
	char	*buf[256] = {0};
	char	*tmp = NULL;
	FILE	*file = fopen(filename, "r");
	
	if (file == NULL)
	{
		fprintf(stderr, "ft_ssl: fopen: %s: %s\n", filename, strerror(errno));
		return (NULL);
	}
	while (fread((char *)buf, 1, 255, file) > 0)
	{
		tmp = content;
		if (content != NULL)
			content = ft_strjoin(content, (char *)buf);
		else
			content = strdup((char *)buf);
		if (content == NULL)
		{
			fprintf(stderr, "ft_ssl: fatal error: %s\n", strerror(errno));
			free(tmp);
			return (NULL);
		}
		bzero(buf, 256);
		free(tmp);
	}
	if (ferror(file))
		fprintf(stderr, "ft_ssl: fgets: %s\n", strerror(errno));
	fclose(file);
	return (content);
}
