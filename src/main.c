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
	
	if (argc >= 2) {
		if (strcmp(argv[1], "--debug") == 0) {
			is_debug = 1;
      		dustbunny_debug("debug mode activated with --debug");
    	} else if (strcmp(argv[1], "--version") == 0) {
      		printf(
      			"Dustbunny version %i.%i.%i\n"
				"Copyright (c)     %i Rory Lane.\n"
				"License           MIT\n\n"
				"You can view the Dustbunny repo at https://github.com/dot-underscore1703/dustbunny\n",
           		DUSTBUNNY_VERSION_MAJOR, DUSTBUNNY_VERSION_MINOR, DUSTBUNNY_VERSION_PATCH, 
				DUSTBUNNY_RELEASE_YEAR
			);
      		return 0;
    	}
 	}
  	
  	while (1) {
		// Use GNU libreadline for line reading, like bash!
    	char *user_input = readline(": ");
    	dustbunny_debug("input: %s", user_input);
    	if (user_input == NULL) {
      		break;
    	}

    	// Buffer for holding tokens from input.
    	Token **tokbuf = malloc(sizeof(Token*) * 128);

    	tokenise_input(tokbuf, 128, user_input);

				

    	free(user_input);
    	user_input = NULL;
  	}

  	fprintf(stderr, "%s: goodbye!\n",dustbunny_program_name);

  	exit(EXIT_SUCCESS);
  	return 0;
}
