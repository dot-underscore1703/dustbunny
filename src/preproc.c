#include <stdlib.h>
#include <stdio.h>
#include "tokens.h"

#define NO_QUOTES 0
#define SINGLE_QUOTES 1
#define DOUBLE_QUOTES 2

Token **preprocess_tokens(Token **tokens) {
	Token *token = *tokens;
	
	Token **processed = malloc(sizeof(Token*) * 128);
	Token *tmp_token = malloc(sizeof(Token*));

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
				if(tmp_text==NULL)
				break;
			}
			default: {
				
			}
		}
		++tokens;
	}
}	
