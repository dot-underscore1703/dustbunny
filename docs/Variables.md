# Variables Documentation
## Variable structure
```c
typedef struct ShellVariable {
	char *key;
	char *value;
	uint8_t exported;
} ShellVariable;
```
Shell variables in Dustbunny are stored in these structs.
- key
	The key of the shell variable, or its "name". The key is the same string that the variables index is derived from.
- value
	The value of the shell variable is the string data the variable stores. The value will always be a string, and it is up to the program using the variable to parse it.
- exported
	An exported variable is a variable that will be passed to the shells child processes.
	It is stored as an integer, where 0 means it is not exported and stays local to the shell, and 1 means its exported and will be passed on.

Variables are stored in a hashmap with the hashing function being FNV-1a.
## Variable operation
### init_shellenv
```c
void init_shellenv(char **envp)
```
init_shellenv: Allocate space for the shell environment and import environment variables. 
Takes a char**.
The envp parameter is the environment pointer that carries the environment variables passed to the program, and is given to the main function as a parameter.

init_shellenv will allocate memory for the shell variables and import all the environment variables passed via the envp parameter to the shell environment.

Returns nothing.

### add_shellvar
```c
int add_shellvar(char *key, char* value, uint8_t overwrite, uint8_t export)
```
add_shellvar: Add a shell variable to the environment, hash the key and store it.
Takes a char*, char*, uint8_t and a uint8_t in that order.
The key param is the key of the variable which it is identified by and is where the index is derived from.
The value param is the string data of the variable.
The overwrite param tells the function whether to overwrite an existing variable if it already exists.
	- 1 if yes, overwrite the variable
	- 0 if no, do not overwrite the existing variable
The export param tells the function whether the created variable should be exported, putting it into the child processes environments.

add_shellvar will grow the shell variables buffer and rehash all the keys if it is over 70% usage.
It will then insert the key into the shell variables buffer.

Returns 0 on success, 1 on failure.

Fails if the internal insert_key function call fails.

### export_shellvar
```c
int export_shellvar(char *key)
```
export_shellvar: Sets shell variables to exported state
Takes a char*
The key param is the string key used to find the target variable in the hashmap.

export_shellvar will find the index of the target variable based on the key value provided to it, and switch the found variables export field to 1.

Returns 0 on success, 1 on failure.

Fails if either of the internal find_slot or export_key functions fail.

### rem_shellvar
```c
int rem_shellvar(char *key)
```
remove_shellvar: remove a shell variable
Takes a char*
The key param is the string key used to find the target variable in the hashmap.

rem_shellvar will remove the variable
im tired. documentation is hard.

Returns 0 on success 1 on failure.

Fails if the internal remove_key function call returns 1.

### get_shellvar
```c
char *get_shellvar(char *key)
```
get_shellvar: Get the value of a shell variable.
Takes a char*
The key param is the string key used to find the target variable in the hashmap.

get_shellvar will find the index of the target variable based on the provided key, and return that variables string value.

Returns a char* string on success, NULL on failure.

Fails if the internal find_slot function call fails.
