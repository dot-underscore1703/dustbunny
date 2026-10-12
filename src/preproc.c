#include <stdlib.h> // malloc()
#include <string.h> // strlen()

#include "tokens.h" // Token
#include "debug.h"

#define NO_QUOTES 0
#define SINGLE_QUOTES 1
#define DOUBLE_QUOTES 2

/// Calculate available size and append text to a string. Reallocates if necessary.
char *__add_and_realloc(char *from, char *to){
	dustbunny_debug("adding text to temporary token string");

	size_t from_size = strlen(from);
	size_t to_size = strlen(to);
	
	// use a temporary for protection in case of realloc() failure
	char *tmp = realloc(to, to_size + from_size + 1);

	// make sure that reallocation worked
	if(!tmp){
		dustbunny_debug("temporary reallocation failure");
		return NULL;
	}
	to = tmp;
	tmp = NULL;

	return strcat(to, from);
}

Token **preprocess_tokens(Token **tokens) {
	Token *token = *tokens;

	size_t tokens_written = 0;
	Token **processed = malloc(sizeof(Token*) * 128);

	size_t tmp_text_size = 32;
	char *tmp_text = malloc(32);

	// 0 for no quotes, 1 for single quotes, 2 for double quotes.
	int in_quotes = NO_QUOTES;

	while(tokens != NULL) {
		switch(token->type) {
			case TokenQuote: {
				if(in_quotes == SINGLE_QUOTES){
					in_quotes = NO_QUOTES;
				}else if(in_quotes == NO_QUOTES){
					in_quotes = SINGLE_QUOTES;
				}
				
				break;
			}
			case TokenText: {
				if(in_quotes){
										
				}else {
					processed[tokens_written++] = token;
				}
				break;
			}
			case TokenSpace: {
				if(in_quotes){
					
				}
			}
			default: {
				
			}
		}
		++tokens;
	}
}	
