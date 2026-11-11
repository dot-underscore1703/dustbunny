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
	dustbunny_debug("adding text '%s' to temporary token string",from);

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
		}else if(str[i] == '$' && quoting != SINGLE_QUOTES){
			char *varstr = malloc(16);
			varstr[0] = '\0';

			while(str[i] != ' ' && str[i] != '\0' && str[i++] != '\n') {
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

	// you would not believe how long it took for me to find the bug this is fixing
	tmp_text[0] = '\0';

	// 0 for no quotes, 1 for single quotes, 2 for double quotes.
	int in_quotes = NO_QUOTES;

	while(*tokens) {
		Token *token = *tokens;
		switch(token->type) {
			// THESE TOKENS ARE DISCARED IN PREPROCESSING.
			case TokenQuote: {
				dustbunny_debug("found quote token");
				if(in_quotes == SINGLE_QUOTES){
					in_quotes = NO_QUOTES;
				}else if(in_quotes == NO_QUOTES){
					in_quotes = SINGLE_QUOTES;
				}else{
					char terminated[] = {'\'', '\0'};
					tmp_text = add_and_realloc(tmp_text, terminated);
				}
				
				break;
			}
			case TokenText: {
				dustbunny_debug("found text token");
				char *expanded_value = expand_string(token->value, in_quotes);
				
				if(in_quotes){
					tmp_text = add_and_realloc(tmp_text,expanded_value);										
				}else {
					processed[tokens_written++] = token_new(TokenText, expanded_value);
				}
				break;
			}
			// THESE TOKENS ARE DISCARED IN PREPROCESSING.
			case TokenSpace: {
				dustbunny_debug("found space token");
				if(in_quotes){		
					char *even_more_temporary_text_buffer = malloc(strlen(tmp_text) + 2); // +2 because strlen doesnt count the terminator character, 
					if(!even_more_temporary_text_buffer){								  // which we need, and we are also adding a character
						dustbunny_debug("allocation error");
						return NULL;
					}

					if(!strcpy(even_more_temporary_text_buffer, tmp_text)){
						dustbunny_debug("error copying string");
						return NULL;
					}
						 	
					if(!strcat(even_more_temporary_text_buffer, " ")){
						dustbunny_debug("error catenating string");
						return NULL;	
					}

					free(tmp_text);
					tmp_text = even_more_temporary_text_buffer;
					even_more_temporary_text_buffer = NULL;
					break;
				}
				break;
			}
			default: {
				dustbunny_debug("other token, appending");
				processed[tokens_written++] = token;
			}
		}
		++tokens;
	}
	size_t tmp_text_len = strlen(tmp_text);
	if(tmp_text_len > 0){
		tmp_text[tmp_text_len] = '\0';
		dustbunny_debug("cleaning up unprocessed text");
		processed[tokens_written++] = token_new(TokenText, tmp_text);	
	}
	processed[tokens_written] = NULL;
	return processed;
}	
