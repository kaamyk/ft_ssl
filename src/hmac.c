#include "../inc/main.h"

void	HMACPad_key(uint8_t key[64], uint8_t pad_keys[2][64])
{
	uint8_t	Ipad[64] = {[0 ... 63] = 0x36};
	uint8_t	Opad[64] = {[0 ... 63] = 0x5C};
	
	for (uint8_t i = 0; i < 64; i++)
	{
		pad_keys[0][i] = key[i] ^ Ipad[i];
		pad_keys[1][i] = key[i] ^ Opad[i];
	}
}

bool	HMACSetup_key(uint8_t pad_keys[2][64], char *input_pass)
{
	size_t		input_l = 0;
	uint8_t		digest[SHA256_HSSZ] = {0};
	uint8_t		formated_key[64] = {0};

	if (input_pass)
		input_l = strlen(input_pass);
	if (input_l > 64)
	{
		if (SHAAlgo(digest, (uint8_t *)input_pass, input_l))
			return (EXIT_FAILURE);
		memcpy(formated_key, digest, SHA256_HSSZ);
	}
	else
		memcpy(formated_key, input_pass, input_l);
	HMACPad_key(formated_key, pad_keys);
	return (EXIT_SUCCESS);
}

// HMAC(K, m) = H( (K' ⊕ opad) ‖ H( (K' ⊕ ipad) ‖ m ) )
uint8_t	*HMAC256(char *key, uint8_t *message, size_t message_l)
{
	uint8_t	pad_keys[2][64] = {0};
	uint8_t	*inner = NULL;
	uint8_t	outer[96] = {0}; // size = pad_keys[1] (64 bytes) + 32 bytes (sha256 output)
	uint8_t	*digest = calloc(SHA256_HSSZ + 1, sizeof(uint8_t));

	if (digest == NULL)
		return (ret_err_mess_code_ptr("ft_ssl: HMAC:", errno));
	if (HMACSetup_key(pad_keys, key))
		return (ret_err_mess_ptr("ft_ssl: HMAC: Setup failed. Leaving."));

	// Inner => Concatenate padded key & message
	inner = malloc(64 + message_l);
	memcpy(inner, pad_keys[0], 64);
	memcpy(inner + 64, message, message_l);
	SHAAlgo(digest, inner, 64 + message_l);

	// Outer => Concatenate SHA256 output & pad_keys[1] (key ^ Opad)
	memcpy(outer, pad_keys[1], 64);
	memcpy(outer + 64, digest, 32);
	SHAAlgo(digest, outer, 96);

	free(inner);
	return (digest);
}
