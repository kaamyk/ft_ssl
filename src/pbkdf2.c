#include "../inc/main.h"

uint8_t *ft_memcat(uint8_t *a, size_t a_len, uint8_t *b, size_t b_len)
{
	uint8_t *res = malloc(a_len + b_len);
	
	if (res == NULL)
		return (NULL);
	if (a)
		memcpy(res, a, a_len);
	memcpy(res + a_len, b, b_len);
	free(a);
	return (res);
}

uint8_t	*PBKDF2(t_pbkdf2 l_data, uint8_t *hmacfn(char *, uint8_t *, size_t))
{
	uint8_t		*res = NULL;
	size_t		res_l = 0;
	uint32_t	l = (l_data.dk_len + SHA256_HSSZ - 1) / SHA256_HSSZ;
	uint8_t		*u = NULL;
	uint8_t		*prev_u = NULL;
	uint8_t		*t = NULL;
	uint8_t		*conc_salt = malloc(l_data.salt_l + 4);

	if (conc_salt == NULL)
		return (ret_err_mess_code_ptr("ft_ssl: PBKDF2:", errno));
	if (l_data.salt)
		memcpy(conc_salt, l_data.salt, l_data.salt_l);
	for (uint32_t i = 1; i <= l; i++)
	{
		// RFC specifies Big Endian encoding of block index
		conc_salt[l_data.salt_l + 0] = (i >> 24) & 0xFF;
		conc_salt[l_data.salt_l + 1] = (i >> 16) & 0xFF;
		conc_salt[l_data.salt_l + 2] = (i >>  8) & 0xFF;
		conc_salt[l_data.salt_l + 3] = (i      ) & 0xFF;
		u = hmacfn(l_data.password, conc_salt, l_data.salt_l + 4);
		t = malloc(SHA256_HSSZ);
		memcpy(t, u, SHA256_HSSZ);
		for (uint32_t j = 2; j <= l_data.c; j++)
		{
			prev_u = u;
			u = hmacfn(l_data.password, prev_u, SHA256_HSSZ);
			free(prev_u);
			for (uint8_t k = 0; k < SHA256_HSSZ; k++)
				t[k] ^= u[k];
		}
		free(u);
		res = ft_memcat(res, res_l, t, SHA256_HSSZ);
		free(t);
		res_l += SHA256_HSSZ;
	}
	free(conc_salt);
	return (res);
}