/* definitions the scanner shares with the parser */

enum token {t_read, t_write, t_id, t_literal, t_gets, t_eq, t_neq,
			t_less, t_great, t_less_eq, t_great_eq, t_add, t_sub, 
			t_mul, t_div, t_lparen, t_rparen, t_eof, t_if, t_do, t_fi, 
			t_od, t_check};

#define MAX_TOKEN_LEN 200
extern char token_image[MAX_TOKEN_LEN];

extern token scan();

typedef void (*NR) ();
