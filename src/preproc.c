#include <stdlib.h> // malloc()
#include <string.h> // strlen()

#include "tokens.h" // Token
#include "debug.h"	// dustbunny_debug()

// Quotation state
#define NO_QUOTES 0		
#define SINGLE_QUOTES 1 	// ''
#define DOUBLE_QUOTES 2 	// ""

/// Calculate available size and append text to a string. Reallocates if necessary.
static char *add_and_realloc(char *to, char *from){
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

static char *expand_string(char *str, int quoting) {
	char *expanded = malloc(32);

	// so strlen doesnt fucking shit itself
	expanded[0] = '\0';

	for(size_t i = 0; i < strlen(str); ++i) {
		if(i == 0 && str[i] == '~' && quoting == NO_QUOTES){
			switch(str[++i]){
				case '/': {
					expanded = add_and_realloc(expanded, getenv("HOME"));
					char slash[] = {'/', '\0'};
					expanded = add_and_realloc(expanded, slash);
					break;
				}
				case '-': {
					expanded = add_and_realloc(expanded, getenv("PWD"));
					break;
				}
				case '+': {
					expanded = add_and_realloc(expanded, getenv("OLDPWD"));
					break;
				}
				case '\0': {
					expanded = add_and_realloc(expanded, getenv("HOME"));
					break;
				}
				default: {
					char terminated[] = {'~', str[i], '\0'};
					expanded = add_and_realloc(expanded, terminated);
				}
			}
		}else if(i == '$' && quoting != SINGLE_QUOTES){
			char *varstr = malloc(16);

			while(str[i] != ' ' && str[i] != '\0' && str[i] != '\n') {
				char terminated[] = {str[i], '\0'};
				varstr = add_and_realloc(varstr, terminated);
			}

			expanded = add_and_realloc(expanded, getenv(varstr));

			free(varstr);
		}else {
			char terminated[] = {str[i], '\0'};
			expanded = add_and_realloc(expanded, terminated);
		}
	}
	
	return expanded;
}

/// Take generated tokens and process them, returning new tokens.
Token **preprocess_tokens(Token **tokens) {
	dustbunny_debug("preprocessing tokens");

	size_t tokens_written = 0;
	Token **processed = malloc(sizeof(Token*) * 128);

	char *tmp_text = malloc(32);

	// 0 for no quotes, 1 for single quotes, 2 for double quotes.
	int in_quotes = NO_QUOTES;

	while(*tokens) {
		Token *token = *tokens;
		switch(token->type) {
			// THESE TOKENS ARE DISCARED IN PREPROCESSING.
			case TokenQuote: {
				if(in_quotes == SINGLE_QUOTES){
					in_quotes = NO_QUOTES;
				}else if(in_quotes == NO_QUOTES){
					in_quotes = SINGLE_QUOTES;
				}else{
					char terminated[] = {'\'', '\0'};
					add_and_realloc(tmp_text, terminated);
				}
				
				break;
			}
			case TokenText: {
				char *expanded_value = expand_string(token->value, in_quotes);
				
				if(in_quotes){
					add_and_realloc(tmp_text,expanded_value);										
				}else {
					processed[tokens_written++] = token_new(TokenText, expanded_value);
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

					if(!strcpy(tmp_space_buf, tmp_text)){
						dustbunny_debug("error copying string");
						return NULL;
					}
					
					if(!strcat(tmp_space_buf, " ")){
						dustbunny_debug("error catenating string");
						return NULL;	
					}
					
					add_and_realloc(tmp_space_buf, tmp_text);
					free(tmp_space_buf);					
					break;
				}
				break;
			}
			default: {
				processed[tokens_written++] = token;
			}
		}
		++tokens;
	}

	if(strlen(tmp_text) > 0){
		processed[tokens_written++] = token_new(TokenText, tmp_text);	
	}
	processed[tokens_written] = NULL;
	return processed;
}	
