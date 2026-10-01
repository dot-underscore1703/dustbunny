// ==================================
// TOKENS.C - IM KINDA THE TOKEN TYPE
// ==================================

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "libdustbunny/debug.h"

typedef enum { 
	TokenText, 
	TokenNewline, 
	TokenPipe,
	TokenBracketIn,
	TokenBracketOut,
	TokenEquals,
	TokenAmpersand,
	TokenUnknown
} TokenType;

typedef struct Token {
  TokenType type;
  char *value;
} Token;

/// Create new token
Token *token_new(TokenType type, char *value) {
	size_t token_size = sizeof(Token);
	
	dustbunny_debug("creating new token with size of %zu",token_size);
	Token *new_token = malloc(sizeof(Token));

	if(!new_token) {
		dustbunny_debug("failed to allocate space for token");
		return NULL;
	}

	// Token type is important so the parser knows how to use the token when constructing the AST
	new_token->type = type;

	new_token->value = value ? strdup(value) : NULL;
	return new_token;
}

/// Destroy the token at the pointer.
int token_destroy(Token *token) {
	dustbunny_debug("destroying token");
	
	if(!token){
		dustbunny_debug("...but nobody came");
		return 1;
	};

	free(token);
	token = NULL;
	
	return 0;
}

/// Get the type of the Token* passed to the function as a string. Useful for debugging.
char* token_type_as_str(char *buffer, size_t buffer_len, Token *token){
		dustbunny_debug("getting token type as string");

		// we dont malloc our own string here because if the user doesnt allocate it, they shouldnt be responsible for freeing it either
		// so we use the two params buffer and buffer_len

		// Since the caller may not know what type the Token is, we will just check to make sure that the buffer is sufficient for the largest type string they could possibly get
		if(buffer_len < 15) {
			dustbunny_debug("buffer size not large enough to hold string");
			return NULL;
		}
		
		switch(token->type){
			case TokenText: strcpy(buffer, "TokenText"); break;
			case TokenNewline: strcpy(buffer, "TokenNewline"); break;
			case TokenPipe: strcpy(buffer, "TokenPipe"); break;
			case TokenBracketIn: strcpy(buffer, "TokenBracketIn"); break;
			case TokenBracketOut: strcpy(buffer, "TokenBracketOut"); break;
			case TokenEquals: strcpy(buffer, "TokenEquals"); break;
			case TokenAmpersand: strcpy(buffer, "TokenAmpersand"); break;
			default: strcpy(buffer,"TokenUnknown"); break;
		}

		dustbunny_debug("concluded token type as %s",buffer);
		return buffer;
}
