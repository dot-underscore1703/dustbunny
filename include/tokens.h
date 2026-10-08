#ifndef TOKENS_H
#define TOKENS_H

#include <stdlib.h>

typedef enum { 
	TokenText, 
	TokenQuote,
	TokenDblQuote,
	TokenNewline,
	TokenSpace,
	TokenSemicolon, 
	TokenPipe,
	TokenDblPipe,
	TokenBracketIn,
	TokenBracketOut,
	TokenEquals,
	TokenAmpersand,
	TokenDblAmpersand,
	TokenUnknown
} TokenType;

typedef struct Token {
  TokenType type;	// token type
  char *value;		// string value for storing text derived from user input
  size_t val_len;	// the length of said text stored.
} Token;

Token *token_new(TokenType type, char *value);

int token_destroy(Token *token);

char* token_type_as_str(char *buffer, size_t buffer_len, Token *token);

#endif
