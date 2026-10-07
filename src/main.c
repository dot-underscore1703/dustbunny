// =======================================================
// MAIN.C - HANDLE ARGS, READ INPUT TO TOKENISE AND LAUNCH
// =======================================================

#include <stdio.h>	// printf()
#include <stdlib.h>	// strcmp(), free()
#include <unistd.h>
#include <getopt.h>

#include <readline/history.h> 	// add_history()
#include <readline/readline.h>	// readline()

#include "dustbunny.h"
#include "version.h"		// DUSTBUNNY_VERSION_XXXXX
#include "tokens.h"			// Token
#include "nodes.h"			// Node
#include "debug.h"			// is_debug, dustbunny_debug()

#include "lexer.h"
//#include "parser.h"
//#include "interpreter.h"

// DUSTBUNNY - HOBBY LINUX SHELL
// https://github.com/dot-underscore1703/dustbunny

char *dustbunny_program_name = NULL;

/// The entry point of program. Self explainable.
int main(int argc, char **argv) {
	dustbunny_program_name = argv[0];

	// Print the list of tokens instead of parsing/executing them.
	int print_only = 0;
	int opt;

	// 1 for readline(), 0 for argv1
	int input_method = 1;
	char *user_input;

	while((opt = getopt(argc,argv,"vdtc:")) != -1) {
		switch(opt) {
			case 'v': {
				printf(
      				"Dustbunny version %i.%i.%i\n"
					"Copyright (c)     %i Rory Lane.\n"
					"License           MIT\n\n"
					"You can view the Dustbunny repo at https://github.com/dot-underscore1703/dustbunny\n",
           			DUSTBUNNY_VERSION_MAJOR, DUSTBUNNY_VERSION_MINOR, DUSTBUNNY_VERSION_PATCH, 
					DUSTBUNNY_RELEASE_YEAR
				);
				exit(EXIT_SUCCESS);		
				break;
			}
			case 'd': {
				is_debug = 1;
				dustbunny_debug("debug mode activated with --debug");
				break;
			}
			case 't': {
				print_only = 1;
				break;
			}
			case 'c': {
				input_method = 0;

				if(optarg == NULL){
					exit(EXIT_FAILURE);
				} 
				
				break;			
			}
			default: {
				// Use GNU libreadline for line reading, like bash!
				input_method = 1;
				break;
			}
		}
	}
  	
  	while (1) {
  		if(!input_method) {
  			user_input = argv[1];
  		}else {
  			user_input = readline(": ");
  		}
  		
    	dustbunny_debug("input: %s", user_input);
    	if (user_input == NULL) {
      		break;
    	}

    	// Buffer for holding tokens from input.
    	Token **tokbuf = malloc(sizeof(Token*) * 128);

    	tokenise_input(tokbuf, 128, user_input);
 		if(print_only){
			for(size_t idx = 0; tokbuf[idx] != NULL; ++idx) {
				char typebuf[20];
				Token *token = tokbuf[idx];
				
				if(token->type == TokenText) {
					printf("%s(%s)\n",token_type_as_str(typebuf, 20, token), token->value);
				}else {
					printf("%s()\n",token_type_as_str(typebuf, 20, token));
				}
			
				//token_destroy(token);
			}
		}

		if(input_method == 2) {
		  	break;
		}	

    	free(user_input);
    	user_input = NULL;
  	}

  	fprintf(stderr, "%s: goodbye!\n",dustbunny_program_name);

  	exit(EXIT_SUCCESS);
  	return 0;
}
