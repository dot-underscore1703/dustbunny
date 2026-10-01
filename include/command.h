#include "tokens.h"

#ifndef COMMAND_H_
#define COMMAND_H_

char **split_input(char *line);
int parse_tokens(Token **tokens);
int execute(char **argv);
int get_argc(char **argv);


#endif
