// =======================================================
// MAIN.C - HANDLE ARGS, READ INPUT TO TOKENISE AND LAUNCH
// =======================================================

#include <stdio.h>	// printf()
#include <stdlib.h>	// strcmp(), free()
#include <stdbool.h>
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
#include "preproc.h"
//#include "parser.h"
//#include "interpreter.h"

// DUSTBUNNY - HOBBY LINUX SHELL
// https://github.com/dot-underscore1703/dustbunny

struct ShellState {
	char* program_name;			// Store the name the program was invoked with.
	bool print_only;			// If true, print the list of tokens instead of parsing/executing them.
	
	int opt;					// Store arg parsing option.
	bool interactive_input;		// Are we reading user input or has our command been passed via args?

	char *user_input;			// Read user input into this variable.
};

const char *dustbunny_version_info =
	"Dustbunny version " DUSTBUNNY_VERSION_MAJOR "." DUSTBUNNY_VERSION_MINOR "." DUSTBUNNY_VERSION_PATCH "\n"
	"Copyright (c)     " DUSTBUNNY_RELEASE_YEAR " Rory Lane.\n"
	"License           MIT\n"
	"Compiled on       " __DATE__  " at " __TIME__ ".\n\n"
	"You can view the Dustbunny repo at https://github.com/dot-underscore1703/dustbunny\n";

const char *dustbunny_usage = 
	"dustbunny [OPTIONS]\n\n"
	
	"Options:\n"
	"     -v, --version               Print version information and then exit."
	"     -c, --command <COMMAND>     Execute a command from args rather than interactive input.\n"
	"     -d, --debug                 Enter debug mode and enable extreme logging (very verbose, will clutter terminal).\n"
	"     -h, --help                  Print usage information and then exit.\n"
	"     -t, --tokens                Instead of parsing and executing input, lex into tokens and print the tokens.\n";
	     
char *dustbunny_program_name = NULL;

/// The entry point of program. Self explainable.
int main(int argc, char **argv) {
	struct ShellState dustbunny_state = {
		argv[0],
		false,
		0,
		true,
		NULL
	};

	// "Export" the program name so other functions can use it.
	dustbunny_program_name = dustbunny_state.program_name;

	static struct option long_options[] = {
		{"version", no_argument,		0, 'v'},
		{"debug",	no_argument,		0, 'd'},
		{"tokens",	no_argument,		0, 't'},
		{"command",	required_argument,	0, 'c'},
		{"help",	no_argument,		0, 'h'}
	};
	
	while((dustbunny_state.opt = getopt_long(argc,argv,"vdtc:h", long_options, NULL)) != -1) {
		switch(dustbunny_state.opt) {
			case 'v': {
				printf(dustbunny_version_info);
				exit(EXIT_SUCCESS);		
				break;
			}
			case 'd': {
				is_debug = 1;
				dustbunny_debug("debug prints activated with -d");
				break;
			}
			case 't': {
				dustbunny_state.print_only = true;
				dustbunny_debug("show tokens only activated with -t");
				break;
			}
			case 'c': {
				dustbunny_state.interactive_input = false;

				if((dustbunny_state.user_input = optarg) == NULL){
					exit(EXIT_FAILURE);
				} 
				
				break;			
			}
			case 'h': {
				printf(dustbunny_usage);
				exit(EXIT_SUCCESS);
				break;
			}
		}
	}
  	
  	for (;;) {
  		if(dustbunny_state.interactive_input == true) {
  			dustbunny_state.user_input = readline(": ");
  		}
  		
    	dustbunny_debug("input: %s", dustbunny_state.user_input);
    	if (dustbunny_state.user_input == NULL) {
      		break;
    	}

    	// Buffer for holding tokens from input.
    	Token **tokbuf = malloc(sizeof(Token*) * 128);

    	tokenise_input(tokbuf, 128, dustbunny_state.user_input);

		Token **processed = preprocess_tokens(tokbuf);
    	
 		if(dustbunny_state.print_only == true){
 			printf("BEFORE PRE-PROCESSING:\n");
			for(size_t idx = 0; tokbuf[idx] != NULL; ++idx) {
				char typebuf[20];
				Token *token = tokbuf[idx];
				
				if(token->type == TokenText) {
					printf("\t%s(%s)\n",token_type_as_str(typebuf, 20, token), token->value);
				}else {
					printf("\t%s()\n",token_type_as_str(typebuf, 20, token));
				}
			}
			printf("AFTER PRE-PROCESSING:\n");
			for(size_t idx = 0; processed[idx] != NULL; ++idx){
				char typebuf[20];
				Token *token = processed[idx];

				if(token->type == TokenText) {
					printf("\t%s(%s)\n",token_type_as_str(typebuf, 20, token), token->value);
				}else {
					printf("\t%s()\n",token_type_as_str(typebuf, 20, token));
				}
			
				token_destroy(token);
			}
			
		}

		if(dustbunny_state.interactive_input == false) {
		  	exit(EXIT_SUCCESS);
		}else{	
    		free(dustbunny_state.user_input);
    		dustbunny_state.user_input = NULL;
    	}
  	}

  	fprintf(stderr, "%s: goodbye!\n",dustbunny_program_name);

  	exit(EXIT_SUCCESS);
  	return 0;
}
