// ==================================
// TOKENS.C - IM KINDA THE TOKEN TYPE
// ==================================

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "debug.h"

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
		if(buffer_len < 19) {
			dustbunny_debug("buffer size not large enough to hold string");
			return NULL;
		}

		// Check that token isnt null
		if(token == NULL){
			dustbunny_debug("token was null");
			return NULL;
		}
		
		switch(token->type){
			case TokenText: buffer = strcpy(buffer, "TokenText"); break;
			case TokenQuote: buffer = strcpy(buffer, "TokenQuote"); break;
			case TokenDblQuote: buffer = strcpy(buffer, "TokenDblQuote"); break;
			case TokenNewline: buffer = strcpy(buffer, "TokenNewline"); break;
			case TokenSpace: buffer = strcpy(buffer, "TokenSpace"); break;
			case TokenSemicolon: buffer = strcpy(buffer, "TokenSemicolon"); break;
			case TokenPipe: buffer = strcpy(buffer, "TokenPipe"); break;
			case TokenDblPipe: buffer = strcpy(buffer, "TokenDblPipe"); break;
			case TokenBracketIn: buffer = strcpy(buffer, "TokenBracketIn"); break;
			case TokenBracketOut: buffer = strcpy(buffer, "TokenBracketOut"); break;
			case TokenEquals: buffer = strcpy(buffer, "TokenEquals"); break;
			case TokenAmpersand: buffer = strcpy(buffer, "TokenAmpersand"); break;
			case TokenDblAmpersand: buffer = strcpy(buffer, "TokenDblAmpersand"); break;
			default: buffer = strcpy(buffer,"TokenUnknown"); break;
		}

		dustbunny_debug("concluded token type as %s",buffer);
		return buffer;
}
