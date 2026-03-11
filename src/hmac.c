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
	const size_t	input_l = strlen(input_pass);
	uint8_t			digest[SHA256_HSSZ] = {0};
	uint8_t			formated_key[64] = {0};
	
	if (input_l > 64)
	{
		if (SHAAlgo(digest, input_pass))
			return (EXIT_FAILURE);
		memcpy(formated_key, digest, SHA256_HSSZ);
	}
	else
		memcpy(formated_key, input_pass, input_l);
	HMACPad_key(formated_key, pad_keys);
	return (EXIT_SUCCESS);
}

bool	HMACMain(t_data *data, uint8_t digest[SHA256_HSSZ])
{
	uint8_t *message = (uint8_t *)*data->inputs;
	size_t	message_l = strlen(*data->inputs);
	uint8_t	pad_keys[2][64] = {0};
	uint8_t	*inner = NULL;
	uint8_t	outer[96] = {0}; // size = pad_keys[1] (64 bytes) + 32 bytes (sha256 output)
	
	if (HMACSetup_key(pad_keys, data->password))
		exit_err_code(data, EX_OSERR);
	
	// Inner => Concatenate padded key & message
	inner = malloc(64 + message_l + 1);
	memcpy(inner, pad_keys[0], 64);
	memcpy(inner + 64, message, message_l);
	SHAAlgo(digest, inner);
	
	// Outer => Concatenate SHS256 output & pad_keys[1] (key ^ Opad)
	memcpy(outer, pad_keys[1], 64);
	memcpy(outer + 64, digest, 32);
	SHAAlgo(digest, outer);
	
	return (EXIT_SUCCESS);
}
