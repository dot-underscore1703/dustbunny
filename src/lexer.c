#include <stdlib.h>
#include <string.h>

#include "tokens.h"
#include "debug.h"

#define NO_QUOTES 0
#define DOUBLE_QUOTES 1
#define SINGLE_QUOTES 2

// TODO: Messy code, fix it.

int __write_and_realloc(
	char **buffer, 
	size_t *buf_size, 
	size_t *amount_written, 
	char to_write
) {
	dustbunny_debug("appending char '%c' to data",to_write);
	
	if(*amount_written + 1 > *buf_size) {
		dustbunny_debug("buffer cant support more data, reallocating");

		// avoid memory leak by using temporary pointer
		void *tmp = realloc(buffer, *buf_size *= 2);
		if(!tmp) {
			// ya probably fucked but lets not make assumptions.
			dustbunny_debug("reallocation failed");
			return 1;
		}

		// we have confirmed the temporary is safe to use so we give back to buffer and discard pointer
		buffer = tmp;
		tmp = NULL;
	}

	*buffer[(*amount_written)++] = to_write;
	return 0;
}

size_t __finalise_and_write_token(
	char **text_buffer,
	size_t *text_buf_size,
	size_t *text_written,
	 
	Token **token_buffer,
	size_t *tokens_written
) {
	dustbunny_debug("finalising for token write.");
	
	// null-terminate token and write to list
	__write_and_realloc(
		text_buffer,
		text_buf_size,
		text_written,
		'\0'
	);
	
	token_buffer[(*tokens_written)++] = token_new(TokenText, strdup(*text_buffer));

	// clean up and reset temporaries to be reused.
	*text_buf_size = 32;
	*text_written = 0;
					
	free(text_buffer);
	text_buffer = malloc(*text_buf_size);
}

size_t tokenise_input(Token **token_buffer, size_t buffer_len, char *input) {
	dustbunny_debug("tokenising input...");

	int in_quotes = NO_QUOTES;

	if(!token_buffer || buffer_len < 1) {
		return 1;
	}

	size_t tokens_written = 0;

	// We only want TokenEquals tokens if we are on the first arg
	int first_token = 1;

	// Allocate space to temporarily hold the values for text tokens.
	size_t tmp_text_size = 32;
	char *tmp_text_buf = malloc(tmp_text_size);
	size_t text_written = 0;
	
	for(size_t idx = 0; input[idx] != '\0'; ++idx) {
		switch(input[idx]) {
			case ' ': {
				dustbunny_debug("found space");
			
				if(text_written > 0) {
					__finalise_and_write_token(
						&tmp_text_buf,
						&tmp_text_size,
						&text_written,
						token_buffer,
						&tokens_written	
					);
				}

				token_buffer[tokens_written++] = 
					token_new(TokenSpace, NULL);

				break;
			}
			case '\n': {
				dustbunny_debug("found newline");
				
				if(text_written > 0) {
					__finalise_and_write_token(
						&tmp_text_buf,
						&tmp_text_size,
						&text_written,
						token_buffer,
						&tokens_written	
					);
				}

				token_buffer[tokens_written++] = token_new(TokenNewline, NULL);
				break;
			}
			case '\'': {
				dustbunny_debug("found quote");
			
				if(in_quotes == DOUBLE_QUOTES) {
					__write_and_realloc(
						&tmp_text_buf,
						&tmp_text_size,
						&text_written,
						'\''
					);
					break;
				}else if(in_quotes == SINGLE_QUOTES) {
					in_quotes = NO_QUOTES;
				}else if(in_quotes == NO_QUOTES) {
					in_quotes = SINGLE_QUOTES;
				}

				if(text_written > 0){
					__finalise_and_write_token(
						&tmp_text_buf,
						&tmp_text_size,
						&text_written,
						token_buffer,
						&tokens_written
					);
				}
				
				token_buffer[tokens_written++] =
					token_new(TokenQuote,NULL);

				break;
			}
			case '"': {
				dustbunny_debug("found dblquote");
			
				if(in_quotes == SINGLE_QUOTES) {
					__write_and_realloc(
						&tmp_text_buf,
						&tmp_text_size,
						&text_written,
						'"'
					);
					break;
				}else if(in_quotes == DOUBLE_QUOTES) {
					in_quotes = NO_QUOTES;
				}else if(in_quotes == NO_QUOTES) {
					in_quotes = DOUBLE_QUOTES;
				}

				if(text_written > 0){
					__finalise_and_write_token(
						&tmp_text_buf,
						&tmp_text_size,
						&text_written,
						token_buffer,
						&tokens_written
					);
				}
				
				token_buffer[tokens_written++] =
					token_new(TokenDblQuote,NULL);

				break;
			}
			case '\\': {
				dustbunny_debug("found escape");
			
				// We shouldn't escape chars if in single quotes.
				char to_write;
				if(in_quotes == SINGLE_QUOTES){
					to_write = '\\';
				}else {
					to_write = input[++idx];
				}
				
				__write_and_realloc(
					&tmp_text_buf, 
					&tmp_text_size, 
					&text_written, 
					to_write
				);
				break;
			}
			case '|': {
				dustbunny_debug("found pipe");
				
				if(text_written > 0) {
					__finalise_and_write_token(
						&tmp_text_buf,
						&tmp_text_size,
						&text_written,
						token_buffer,
						&tokens_written	
					);
				}

				// two consecutive pipes has its own token
				if(input[idx + 1] == '|'){
					token_buffer[tokens_written++] = token_new(TokenDblPipe, NULL);
					++idx;
				}else {
					token_buffer[tokens_written++] = token_new(TokenPipe, NULL);
				}

				first_token = 0;
				break;
			}
			case '<': {
				dustbunny_debug("found bracketin");
				
				if(text_written > 0) {
					__finalise_and_write_token(
						&tmp_text_buf,
						&tmp_text_size,
						&text_written,
						token_buffer,
						&tokens_written	
					);
				}

				token_buffer[tokens_written++] = token_new(TokenBracketIn, NULL);				

				first_token = 0;
				break;
			}
			case '>': {
				dustbunny_debug("found bracketout");
				if(text_written > 0) {
					__finalise_and_write_token(
						&tmp_text_buf,
						&tmp_text_size,
						&text_written,
						token_buffer,
						&tokens_written	
					);
				}

				token_buffer[tokens_written++] = token_new(TokenBracketOut, NULL);				

				first_token = 0;
				break;
			}
			case '=': {
				dustbunny_debug("found equals");
				if(first_token){
					__finalise_and_write_token(
						&tmp_text_buf,
						&tmp_text_size,
						&text_written,
						token_buffer,
						&tokens_written	
					);

					token_buffer[tokens_written++] = token_new(TokenEquals, NULL);				
					first_token = 0;
				}else {
					__write_and_realloc(
						&tmp_text_buf, 
						&tmp_text_size, 
						&text_written, 
						'='
					);
				}
				break;
			}

			case '&': {
				if(text_written > 0) {
					__finalise_and_write_token(
						&tmp_text_buf,
						&tmp_text_size,
						&text_written,
						token_buffer,
						&tokens_written	
					);
				}

				// two consecutive ampersands has its own token
				if(input[idx + 1] == '&'){
					token_buffer[tokens_written++] = token_new(TokenDblAmpersand, NULL);
					++idx;
				}else {
					token_buffer[tokens_written++] = token_new(TokenAmpersand, NULL);
				}

				first_token = 0;
				break;
			}			
			
			default: {
				dustbunny_debug("defaulting");
				__write_and_realloc(
					&tmp_text_buf, 
					&tmp_text_size, 
					&text_written, 
					input[idx]
				);
				break;
			}
		}
	}
	if(text_written > 0) {
		dustbunny_debug("cleaning up untokenised text");
		__finalise_and_write_token(
			&tmp_text_buf,
			&tmp_text_size,
			&text_written,
			token_buffer,
			&tokens_written	
		);
	}
	
	token_buffer[tokens_written++] = NULL;
	dustbunny_debug("Tokens written:%i",tokens_written);
	return tokens_written;
} 

