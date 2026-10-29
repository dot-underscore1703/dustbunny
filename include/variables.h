#ifndef VARIABLES_H
#define VARIABLES_H

void init_shellenv();
int add_shellvar(char *key, char *value, int overwrite);
int rem_shellvar(char *key);
int export_shellvar(char *key);
char *get_shellvar(char *key);

#endif
