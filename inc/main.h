#ifndef FTSSL
#define FTSSL

#include <stdint.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <sysexits.h>

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
	base64
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

#define BASE64STR	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_"

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
	char		*password;
	char		*salt;
	char		*init_vector;
	char		**inputs;
	char		*in_file;
	char		*out_file;
}				t_data;

typedef bool (*t_cmd_fn)(t_data *, char **);
typedef struct	s_cmd
{
	const char		*name;
	const t_cmd_fn	fn;
}				t_cmd;

typedef bool (*t_algo_fn)(const t_data *, const char *, const char *);
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

//	utils.c
uint32_t	rotate_left(uint8_t bits, uint32_t word);
uint32_t	rotate_right(uint8_t bits, uint32_t word);
uint32_t	shift_right(uint8_t bits, uint32_t word);
bool		is_whitespace(const char c);
bool		is_hexa(const char c);
void	decode(uint32_t *output, uint8_t *input, uint32_t len);
void	encode(uint8_t *output, uint32_t *input, uint32_t len);
char	*file_to_str(const char *filename);

//	stdin.c
char 	*read_stdin( void );
bool	is_in_pipe( void );

//	display.c
void	print_ssl_usage(t_data *data);
void	print_dgst_usage(t_data *data);
void	print_cphr_usage(t_data *data);
bool	dgst_display(const uint8_t digest[16], uint16_t options, const char *filename, const char *to_hash, const char *algoname, const uint8_t hssz);
bool	cphr_display(char *cipher);

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
bool			SHARoutine(const t_data *data, const char *runner, const char *to_hash);
// bool			SHAMain(t_data *data);

//	md5.c
void	MD5Init(t_MD5_CTX *context);
void	MD5Transform(uint32_t state[4], uint8_t block[64]);
void	MD5Update(t_MD5_CTX *context, uint8_t *input, unsigned int inputLen);
void	MD5Final(uint8_t digest [16], t_MD5_CTX *context);
void	MD5memset(uint8_t *output, const uint8_t value, const uint32_t len);
uint8_t	*MD5memcpy(uint8_t *dest, const uint8_t *src, const uint32_t len);
bool	MDRoutine(const t_data *data, const char *runner, const char *to_hash);
// bool	MDMain(t_data *data);

//	base64.c
char	*base64_encode(const char *input);
char	*base64_decode(const char *input);
bool 	B64Routine(const t_data *data, const char *runner, const char *to_hash);

//	des.c
bool 	DESRoutine(const t_data *data, const char *runner, const char *to_hash);

//	routine.c
void	routine(t_data *data);

//	stdin.c
char 	*read_stdin( void );
bool	is_in_pipe( void );

//	digest.c
bool	dgst_main(t_data *data, char **argv);

//	cipher.c
bool	cphr_main(t_data *data, char **argv);

//	main.c
#endif