#include <stdlib.h> // malloc()
#include <string.h> // strlen()

#include "tokens.h" // Token
#include "debug.h"	// dustbunny_debug()

// Quotation state
#define NO_QUOTES 0		
#define SINGLE_QUOTES 1 	// ''
#define DOUBLE_QUOTES 2 	// ""

/// Calculate available size and append text to a string. Reallocates if necessary.
char *__add_and_realloc(char *to, char *from){
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

char *__expand_string(char *str) {
	char *expanded = malloc(32);

	size_t i;
	for(i = 0; i < strlen(str); ++i) {
		char c = str[i];
		if(i == 0 && c == '~'){
			switch(str[++i]){
				case '/': {
					char *home_dir = getenv("HOME");
					expanded = __add_and_realloc(expanded, home_dir);
					break;
				}
				case '+': {
					char *working_dir = getenv("PWD");
					expanded = __add_and_realloc(expanded, working_dir);
					break;
				}
				case '-': {
					char *previous_dir = getenv("OLDPWD");
					expanded = __add_and_realloc(expanded, previous_dir);
				}
			}
		}else if(c == '$'){
			++c;
			char *varstr = malloc(16);
			size_t len_written = 0;
			
			while(c != ' ' && c != '\n' && c != '\0'){
				varstr[len_written++] = c;
				++c;
			}

			char *variable = getenv(varstr);
			expanded = __add_and_realloc(expanded, variable);
		}
	}
	return expanded;
}

/// Take generated tokens and process them, returning new tokens.
Token **preprocess_tokens(Token **tokens) {
	Token *token = *tokens;

	size_t tokens_written = 0;
	Token **processed = malloc(sizeof(Token*) * 128);

	char *tmp_text = malloc(32);

	// 0 for no quotes, 1 for single quotes, 2 for double quotes.
	int in_quotes = NO_QUOTES;

	while(tokens != NULL) {
		switch(token->type) {
			// THESE TOKENS ARE DISCARED IN PREPROCESSING.
			case TokenQuote: {
				if(in_quotes == SINGLE_QUOTES){
					in_quotes = NO_QUOTES;
				}else if(in_quotes == NO_QUOTES){
					in_quotes = SINGLE_QUOTES;
				}
				
				break;
			}
			case TokenText: {
				token->value = __expand_string(token->value);
				
				if(in_quotes){
					__add_and_realloc(tmp_text,token->value);										
				}else {
					processed[tokens_written++] = token;
				}
				break;
			}
			// THESE TOKENS ARE DISCARED IN PREPROCESSING.
			case TokenSpace: {
				if(in_quotes){
					// allocate temporary buffer to add a space.
					// i just woke up so this is probably shitty
				 	size_t tmp_space_size = sizeof(token->value) + 2; // +2 because we are adding a character, and need a terminator
					char *tmp_space_buf = malloc(tmp_space_size);

					if(!strcpy(tmp_space_buf, token->value)){
						dustbunny_debug("error copying string");
						return NULL;
					}
					
					if(!strcat(tmp_space_buf, " ")){
						dustbunny_debug("error catenating string");
						return NULL;	
					}
					
					__add_and_realloc(tmp_space_buf, tmp_text);					
				}
				break;
			}
			default: {
				
			}
		}
		++tokens;
	}
}	
