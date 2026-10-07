#ifndef LEXER_H
#define LEXER_H

#include <stddef.h>
#include "tokens.h"

size_t tokenise_input(Token **token_buffer, size_t buffer_len, char *input);
	
#endif
