#ifndef FTSSL
#define FTSSL

#include <stdint.h>
#include <ctype.h>
#include <stdlib.h>
#include <malloc.h>
#include <stdbool.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <sysexits.h>
#include <byteswap.h>
#include <math.h>
#include <time.h>

enum
{
	shaSuccess,
	shaNull,			/* Null pointer parameter */
	shaInputTooLong,	/* input data too long */
	shaStateError,		/* called Input after FinalBits or Result */
	shaBadParam			/* passed a bad parameter */
};

enum
{
	digest,
	print,
	quiet,
	reverse,
	string,
	usage,
	dec,
	enc,
	in_file,
	out_file,
	read_in,
	base64,
	pwprint,
	pwread
};

#define DGST	1 << digest
#define PRINT	1 << print
#define QUIET	1 << quiet
#define REVERSE	1 << reverse
#define STRING	1 << string
#define USAGE	1 << usage
#define DECODE	1 << dec
#define ENCODE	1 << enc
#define IN_FILE	1 << in_file
#define OUT_FILE 1 << out_file
#define READ_IN	1 << read_in
#define B64		1 << base64
#define PWP		1 << pwprint
#define PWR		1 << pwread

#define BASE64STR	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"
#define HEXABASE	"0123456789ABCDEF"

#define SHA256_HSSZ 32
#define SHA256_BLSZ	64
#define MD5_HSSZ 	16

typedef struct	s_data
{
	// const char	algostr[3][7];
	uint16_t	options;
	char		*algo;
	char		*in;
	char		*string;
	char		*raw_key;
	uint64_t	key;
	char		*password;
	uint64_t	salt;
	char		*raw_salt;
	size_t		salt_len;
	char		*raw_init_vector;
	uint64_t	init_vector;
	char		**inputs;
	char		*in_file;
	char		*out_file;
	size_t		in_len;
}				t_data;

typedef struct	s_des_data
{
	uint64_t	*full_output;
	uint64_t	sub_keys[16];
	size_t		len_to_enc;
	bool		ebc;
}				t_des_data;

typedef struct	s_pbkdf2
{
	char		*password;
	uint64_t	salt;
	size_t		salt_l;
	uint32_t	dk_len;
	uint32_t	c;
}				t_pbkdf2;

typedef bool (*t_cmd_fn)(t_data *, char **);
typedef struct	s_cmd
{
	const char		*name;
	const t_cmd_fn	fn;
}				t_cmd;

typedef bool (*t_algo_fn)(t_data *, char *, char *);
typedef struct	s_algo
{
	const char		*name;
	const t_algo_fn	fn;
}				t_algo;

typedef struct SHA256_CTX
{
	uint32_t Intermediate_Hash[SHA256_HSSZ / 4]; /* Message Digest */
	uint32_t Length_Low;                /* Message length in bits */
	uint32_t Length_High;               /* Message length in bits */
	int_least16_t Message_Block_Index;  /* Message_Block array index */
	uint8_t Message_Block[SHA256_BLSZ];
	int Computed;                       /* Is the digest computed? */
	int Corrupted;                      /* Is the digest corrupted? */
}				t_SHA256_CTX;

typedef struct MD5_CTX
{
	uint32_t	state[4];
	uint32_t	count[2];
	uint8_t		buffer[64];
}				t_MD5_CTX;

//	error.c
void	exit_err_code(t_data *data, const uint8_t ret_value);
void	exit_err_mess(const char *mess, t_data *data, const uint8_t ret_value);
void	exit_err_mess_code(const char *mess, const int errnum, t_data *data, const uint8_t ret_value);
void	exit_err_mess_opt(const char *mess, const char *opt, t_data *data, const uint8_t ret_value);
bool	ret_err_mess_code(const char *mess, const int errnum);
bool	ret_err_mess(const char *mess);
bool	ret_err_mess_opt(const char *mess, const char *opt);
void	exit_err_mess_opt2(const char *mess, const char *opt, const char *opt1, t_data *data, const uint8_t ret_value);
void	*ret_err_mess_ptr(const char *mess);
void	*ret_err_mess_code_ptr(const char *mess, const int errnum);

//	utils.c
uint32_t	rotate_left(uint8_t bits, uint32_t word);
uint32_t	rotate_right(uint8_t bits, uint32_t word);
uint32_t	shift_right(uint8_t bits, uint32_t word);
bool		is_whitespace(const char c);
bool		is_hexa(const char c);
void	decode(uint32_t *output, uint8_t *input, uint32_t len);
void	encode(uint8_t *output, uint32_t *input, uint32_t len);
char	*file_to_str(const char *filename);
uint64_t	atohex(char *str);

//	stdin.c
char 	*read_stdin(size_t *len_out);
bool	is_in_pipe( void );

//	display.c
void	print_ssl_usage(t_data *data);
void	print_dgst_usage(t_data *data);
void	print_cphr_usage(t_data *data);
bool	dgst_display(const uint8_t digest[16], uint16_t options, const char *filename, const char *to_hash, const char *algoname, const uint8_t hssz);
bool	cphr_display(char *cipher, size_t cipher_l);

// ft_strjoin.c
char	*ft_strjoin(char const *s1, char const *s2);

//	parser.c
bool	dgst_parser(t_data *data, char **argv);
bool	cphr_parser(t_data *data, char **argv);

//	sha256.c
extern int		SHA256Reset(t_SHA256_CTX *);
extern int		SHA256Input(t_SHA256_CTX *, const uint8_t *bytes, unsigned int bytecount);
extern int		SHA256FinalBits(t_SHA256_CTX *, const uint8_t bits, unsigned int bitcount);
extern uint8_t	SHA256Result(t_SHA256_CTX *, uint8_t Message_Digest[SHA256_HSSZ]);//
bool			SHAAlgo(uint8_t digest[SHA256_HSSZ], uint8_t *to_hash, const size_t to_hash_l);
bool			SHARoutine(t_data *data, char *runner, char *to_hash);

//	md5.c
void	MD5Init(t_MD5_CTX *context);
void	MD5Transform(uint32_t state[4], uint8_t block[64]);
void	MD5Update(t_MD5_CTX *context, uint8_t *input, unsigned int inputLen);
void	MD5Final(uint8_t digest [16], t_MD5_CTX *context);
void	MD5memset(uint8_t *output, const uint8_t value, const uint32_t len);
uint8_t	*MD5memcpy(uint8_t *dest, const uint8_t *src, const uint32_t len);
bool	MDRoutine(t_data *data, char *runner, char *to_hash);
// bool	MDMain(t_data *data);

//	base64.c
char	*base64_encode(const char *input, const size_t input_l);
char	*base64_decode(const char *input, const size_t input_l);
bool 	B64Routine(t_data *data, char *runner, char *to_hash);

//	hmac.c
uint8_t	*HMAC256(char *key, uint8_t *message, size_t message_l);
// uint8_t	*HMACMain(char *key, char *message);
// bool	HMACMain(t_data *data, uint8_t digest[SHA256_HSSZ])

//	pbkdf2.c
uint8_t	*PBKDF2(t_pbkdf2 l_data, uint8_t *hmacfn(char *, uint8_t *, size_t));

//	des.c
bool 	des_routine(t_data *data, char *runner, char *to_encrypt);

//	routine.c
void	routine(t_data *data);

//	digest.c
bool	dgst_main(t_data *data, char **argv);

//	cipher.c
char	*cphr_getstdin(uint16_t *options, size_t *len_out);
bool	cphr_main(t_data *data, char **argv);

//	main.c
#endif
