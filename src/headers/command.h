#ifndef PARSING_H_
#define PARSING_H_

char **tokenise(char *line);
int execute(char **argv);
int get_argc(char **argv);

#endif