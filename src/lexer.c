#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "tokens.h"
#include "debug.h"

enum Quotations {
	NO_QUOTES,		// Since we haven't assigned a custom value to this, its the same as 0, which makes it falsy. This means we can still use (!state.in_quotes) and (state.quotes).
	SINGLE_QUOTES,	// These have the values of 1 and
	DOUBLE_QUOTES	// 2 though, so they are truthy.
};

struct LexerState {
	enum Quotations in_quotes;
	size_t tokens_written;
	bool first_token;
	
	size_t tmp_text_size;
	char *tmp_text_buf;
	size_t text_written;
};

static int write_and_realloc(
	char **buffer, 
	size_t *buf_size, 
	size_t *amount_written, 
	char to_write
) {
	dustbunny_debug("appending char '%c' to data",to_write);
	
	if(*amount_written + 1 > *buf_size) {
		dustbunny_debug("buffer cant support more data, reallocating");

		// avoid memory leak by using temporary pointer
		char *tmp = realloc(*buffer, *buf_size *= 2);
		if(!tmp) {
			// ya probably fucked but lets not make assumptions.
			dustbunny_debug("reallocation failed");
			return 1;
		}

		// we have confirmed the temporary is safe to use so we give back to buffer and discard pointer
		*buffer = tmp;
		tmp = NULL;
	}

	(*buffer)[(*amount_written)++] = to_write;
	return 0;
}

static void finalise_and_write_token(
	char **text_buffer,
	size_t *text_buf_size,
	size_t *text_written,
	 
	Token **token_buffer,
	size_t *tokens_written
) {
	dustbunny_debug("finalising for token write.");
	
	// null-terminate token and write to list
	write_and_realloc(
		text_buffer,
		text_buf_size,
		text_written,
		'\0'
	);

	token_buffer[(*tokens_written)++] = token_new(TokenText, *text_buffer);
	
	// clean up and reset temporaries to be reused.
	*text_buf_size = 32;
	*text_written = 0;
					
	free(*text_buffer);
	*text_buffer = malloc(*text_buf_size);
	*text_buffer[0] = '\0';
}

int tokenise_input(Token **token_buffer, size_t buffer_len, char *input) {
	dustbunny_debug("tokenising input...");

	struct LexerState state;
	
	if(!token_buffer || buffer_len < 1) {
		return 1;
	}

	state.tokens_written = 0;

	// We only want TokenEquals tokens if we are on the first arg
	state.first_token = true;

	// Allocate space to temporarily hold the values for text tokens.
	state.tmp_text_size = 32;
	state.tmp_text_buf = malloc(state.tmp_text_size);
	state.text_written = 0;
	
	for(size_t idx = 0; input[idx] != '\0'; ++idx) {
		if(state.tokens_written + 1 > buffer_len) {
			Token **tmp = realloc(token_buffer, state.tokens_written *= 2);
			if(!tmp) {
				dustbunny_debug("Reallocation error");
				free(token_buffer);
				return 1;
			}

			token_buffer = tmp;
			tmp = NULL;
		}
		switch(input[idx]) {
			case ' ': {
				dustbunny_debug("found space");
			
				if(state.text_written > 0) {
					finalise_and_write_token(
						&state.tmp_text_buf,
						&state.tmp_text_size,
						&state.text_written,
						token_buffer,
						&state.tokens_written	
					);
				}
				if(
					state.tokens_written > 0 &&
					token_buffer[state.tokens_written - 1] != NULL && 
					token_buffer[state.tokens_written - 1]->type != TokenSpace
				){
					token_buffer[state.tokens_written++] = 
						token_new(TokenSpace, NULL);
				}
				break;
			}
			case '\n': {
				dustbunny_debug("found newline");
				
				if(state.text_written > 0) {
					finalise_and_write_token(
						&state.tmp_text_buf,
						&state.tmp_text_size,
						&state.text_written,
						token_buffer,
						&state.tokens_written	
					);
				}

				token_buffer[state.tokens_written++] = token_new(TokenNewline, NULL);
				break;
			}
			case '\'': {
				dustbunny_debug("found quote");
			
				if(state.in_quotes == DOUBLE_QUOTES) {
					write_and_realloc(
						&state.tmp_text_buf,
						&state.tmp_text_size,
						&state.text_written,
						'\''
					);
					break;
				}else if(state.in_quotes == SINGLE_QUOTES) {
					state.in_quotes = NO_QUOTES;
				}else if(state.in_quotes == NO_QUOTES) {
					state.in_quotes = SINGLE_QUOTES;
				}

				if(state.text_written > 0){
					finalise_and_write_token(
						&state.tmp_text_buf,
						&state.tmp_text_size,
						&state.text_written,
						token_buffer,
						&state.tokens_written
					);
				}
				
				token_buffer[state.tokens_written++] =
					token_new(TokenQuote,NULL);

				break;
			}
			case '"': {
				dustbunny_debug("found dblquote");
			
				if(state.in_quotes == SINGLE_QUOTES) {
					write_and_realloc(
						&state.tmp_text_buf,
						&state.tmp_text_size,
						&state.text_written,
						'"'
					);
					break;
				}else if(state.in_quotes == DOUBLE_QUOTES) {
					state.in_quotes = NO_QUOTES;
				}else if(state.in_quotes == NO_QUOTES) {
					state.in_quotes = DOUBLE_QUOTES;
				}

				if(state.text_written > 0){
					finalise_and_write_token(
						&state.tmp_text_buf,
						&state.tmp_text_size,
						&state.text_written,
						token_buffer,
						&state.tokens_written
					);
				}
				
				token_buffer[state.tokens_written++] =
					token_new(TokenDblQuote,NULL);

				break;
			}
			case '\\': {
				dustbunny_debug("found escape");
			
				// We shouldn't escape chars if in single quotes.
				char to_write;
				if(state.in_quotes == SINGLE_QUOTES){
					to_write = '\\';
				}else {
					to_write = input[++idx];
				}
				
				write_and_realloc(
					&state.tmp_text_buf, 
					&state.tmp_text_size, 
					&state.text_written, 
					to_write
				);
				break;
			}
			case '|': {
				dustbunny_debug("found pipe");

				if(state.in_quotes){
					write_and_realloc(
						&state.tmp_text_buf, 
						&state.tmp_text_size, 
						&state.text_written, 
						'|'
					);	
					break;
				}
				if(state.text_written > 0) {
					finalise_and_write_token(
						&state.tmp_text_buf,
						&state.tmp_text_size,
						&state.text_written,
						token_buffer,
						&state.tokens_written	
					);
				}

				// two consecutive pipes has its own token
				if(input[idx + 1] == '|'){
					token_buffer[state.tokens_written++] = token_new(TokenDblPipe, NULL);
					++idx;
				}else {
					token_buffer[state.tokens_written++] = token_new(TokenPipe, NULL);
				}

				state.first_token = 0;
				break;
			}
			case '<': {
				dustbunny_debug("found bracketin");

				if(state.in_quotes){
					write_and_realloc(
						&state.tmp_text_buf, 
						&state.tmp_text_size, 
						&state.text_written, 
						'<'
					);	
					break;
				}		
				if(state.text_written > 0) {
					finalise_and_write_token(
						&state.tmp_text_buf,
						&state.tmp_text_size,
						&state.text_written,
						token_buffer,
						&state.tokens_written	
					);
				}

				token_buffer[state.tokens_written++] = token_new(TokenBracketIn, NULL);				

				state.first_token = 0;
				break;
			}
			case '>': {
				dustbunny_debug("found bracketout");

				if(state.in_quotes){
					write_and_realloc(
						&state.tmp_text_buf, 
						&state.tmp_text_size, 
						&state.text_written, 
						'>'
					);	
					break;
				}
				if(state.text_written > 0) {
					finalise_and_write_token(
						&state.tmp_text_buf,
						&state.tmp_text_size,
						&state.text_written,
						token_buffer,
						&state.tokens_written	
					);
				}

				token_buffer[state.tokens_written++] = token_new(TokenBracketOut, NULL);				

				state.first_token = 0;
				break;
			}
			case '=': {
				dustbunny_debug("found equals");

				if(state.in_quotes){
					write_and_realloc(
						&state.tmp_text_buf, 
						&state.tmp_text_size, 
						&state.text_written, 
						'='
					);	
					break;
				}
				if(state.first_token){
					finalise_and_write_token(
						&state.tmp_text_buf,
						&state.tmp_text_size,
						&state.text_written,
						token_buffer,
						&state.tokens_written	
					);

					token_buffer[state.tokens_written++] = token_new(TokenEquals, NULL);				
					state.first_token = 0;
				}else {
					write_and_realloc(
						&state.tmp_text_buf, 
						&state.tmp_text_size, 
						&state.text_written, 
						'='
					);
				}
				break;
			}

			case '&': {
				if(state.in_quotes){
					write_and_realloc(
						&state.tmp_text_buf, 
						&state.tmp_text_size, 
						&state.text_written, 
						'&'
					);	
					break;
				}
				if(state.text_written > 0) {
					finalise_and_write_token(
						&state.tmp_text_buf,
						&state.tmp_text_size,
						&state.text_written,
						token_buffer,
						&state.tokens_written	
					);
				}

				// two consecutive ampersands has its own token
				if(input[idx + 1] == '&'){
					token_buffer[state.tokens_written++] = token_new(TokenDblAmpersand, NULL);
					++idx;
				}else {
					token_buffer[state.tokens_written++] = token_new(TokenAmpersand, NULL);
				}

				state.first_token = 0;
				break;
			}
			case '#': {
				if(state.in_quotes){
					write_and_realloc(
						&state.tmp_text_buf, 
						&state.tmp_text_size, 
						&state.text_written, 
						'#'
					);	
					break;
				}
				if(state.text_written > 0) {
					finalise_and_write_token(
						&state.tmp_text_buf,
						&state.tmp_text_size,
						&state.text_written,
						token_buffer,
						&state.tokens_written	
					);
				}
				// hahahaha fucking destroy EVERYTHING
				while(input[idx] != '\0' && input[idx] != '\n') {
					++idx;
					continue;			
				}
				break;
			}			
			
			default: {
				dustbunny_debug("defaulting");
				write_and_realloc(
					&state.tmp_text_buf, 
					&state.tmp_text_size, 
					&state.text_written, 
					input[idx]
				);
				break;
			}
		}
	}
	if(state.text_written > 0) {
		dustbunny_debug("cleaning up untokenised text");
		finalise_and_write_token(
			&state.tmp_text_buf,
			&state.tmp_text_size,
			&state.text_written,
			token_buffer,
			&state.tokens_written	
		);
	}

	free(state.tmp_text_buf);
	
	token_buffer[state.tokens_written] = NULL;
	dustbunny_debug("Tokens written:%i",state.tokens_written);
	return 0;
} 
