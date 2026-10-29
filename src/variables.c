// Bad code is back and this time it is NOT fucking around.

// This handles the storage of the shell variables.
// For efficiency, variables are stored in a hash table using a FNV hashing algorithm.
// If collisions occur, it will move the key to the next available slot to the right.
// If no available slots exist on the right, it will go back to the start and search left.
// As a last resort, it will use linear searching on collisions.

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "debug.h"

// We can't just use strings because we still need a way to store the string.
// You could argue that we do store the string because we store the hash,
// but in the event of a collission the string may not be stored at that index.
// which is why we use this struct, it also allows use to save if its exported or not
typedef struct ShellVariable {
	char *key;
	char *value;
	int exported;
} ShellVariable;

// macros used in hashing
#define OFFSET 0x811C9DC5
#define PRIME 0x01000193

/// Hashing function, used for hashing keys to find a suitable index for them. FNV.
static uint32_t hash_key(char *key) {
	uint32_t hash = OFFSET;

	while(*key != '\0'){
		hash ^= (uint8_t)*key++;
		hash *= PRIME;
	}
	
	return hash;
}

size_t environment_size; // set size with init_shellenv()
ShellVariable **shell_variables;
size_t stored_variables = 0;

/// Find a slot based on the type.
/// e for empty slot, t for taken slot
/// searching for taken slots will return the index where the variable is stored
/// searching for empty slots will return the index of the nearest empty slot to a key.
/// will return -1 on failure
static int find_slot(char type, char *key, uint32_t *output, int return_match) {
	uint32_t idx = hash_key(key) % environment_size;

	// dont modify idx, it makes searching for free spots faster if we have to reverse direction and move back
	uint32_t new_idx = idx;

	// Controls which way across we are looking for a free spot.
	// 1 = look right, otherwise look left. We will only look left if there is no available spots to the right. 
	int direction = 1;
	
	if(type == 'e'){
		// find the next empty slot to the right, if none exist, go back to the start and search left
		if(!return_match){
			while(shell_variables[new_idx]){
				if(direction == 1) {
					if(new_idx + 1 < environment_size){
						++new_idx;
					}else {
						// as stated before, changing direction will now make us look leftwards
						direction = 0;
						new_idx = idx; // instead of looking through all of the spaces we know arent free, go back to where we started.
					}
				}else {
					if(new_idx > 0) {
						--new_idx;
					}else {
						dustbunny_debug("i checked everywhere theres nowhere to put this variable. how did this happen.");
						return 1;					
					}
				}
			}
		}
		// new_idx should now definitely point to a free space.
	}else if(type == 't') {
		if(!return_match){
			while(strcmp(shell_variables[new_idx]->key, key) != 0){
				if(direction == 1){
					if(new_idx + 1 < environment_size){
						++new_idx;
					}else {
						direction = 0;
						new_idx = idx;
					}
				}else {
					if(new_idx > 0){
						--new_idx;
					}else {
						dustbunny_debug("too tired to write debug messages but basically theres no key");
						return 1;
					}
				}
			}
		}
		// new_idx should now definitely point to the variable
	}

	*output = new_idx;
	return 0;
}

/// Export keys into the environment
static int export_key(uint32_t idx) {
	if(setenv(
		shell_variables[idx]->key, 
		shell_variables[idx]->value,
		1
	)){
		return 1;
	}

	shell_variables[idx]->exported = 1;
	
	return 0;
}

/// Insert a key with key and value. Will overwrite existing values if overwrite is truthy.
static int insert_key(char *key, char *value, int overwrite, int exported) {
	dustbunny_debug("inserting new shell variable key");

	ShellVariable *new_var = malloc(sizeof(ShellVariable));

	if(!new_var){
		dustbunny_debug("error allocating space for shell variable");
		return 1;
	}

	new_var->key = key;
	new_var->value = value;
	new_var->exported = exported;

	// this will give us the index of the closest empty slot next to key
	uint32_t idx;
	if(find_slot('e',key, &idx,overwrite) == 1) {
		dustbunny_debug("error looking for slot");
		return 1;
	}
	
	
	shell_variables[idx] = new_var;

	return 0;
}

/// Remove a key from the shell variables array. Returns 1 if failure to find slot. 0 on success.
static int remove_key(char *key) {
	// notice the difference between this call and the call in the function above
	uint32_t idx;
	if(find_slot('t',key, &idx,0) == 1) {
		dustbunny_debug("error finding key to remove");
		return 1;
	}

	free(shell_variables[idx]);
	shell_variables[idx] = NULL;

	return 0;
}

/// Rehash all keys. I hope.
static int rehash_keys() {
	dustbunny_debug("rehashing keys (im scared)");
	for(size_t i = 0; i < environment_size; ++i) {
		if(!shell_variables[i]) continue;
		char *key = shell_variables[i]->key;
		char *value = shell_variables[i]->value;
		int is_exported = shell_variables[i]->exported;

		// this probably doesnt work.
		// infact thinking of the potential bug has demotivated me.
		remove_key(key);

		insert_key(key, value, 1, is_exported);
	}
	return 0;	
}

/// Allocate space for the shell environment.
void init_shellenv(char **envp) {
	 shell_variables = malloc(sizeof(ShellVariable*) * environment_size);

	while(*envp != NULL){
		if(insert_key(strtok(*envp, "="), strtok(NULL, "="), 1, 1)){
			dustbunny_debug("failure inserting environment key to shell environment");
		}		
	}
	 
	return;
}

/// Add a shell variable to the environment, hash the key and store it.
int add_shellvar(char *key, char* value, int overwrite, int export) {
	dustbunny_debug("adding shell variable %s with value of %s",key,value);	

	// this math gives us the usage as a percentage
	size_t env_usage = (stored_variables / environment_size) * 100;

	if(env_usage >= 70) {
		dustbunny_debug("shell variable buffer exceeded 70% usage, growing.");
		
		ShellVariable **tmp = realloc(shell_variables, environment_size *= 2);
		if(!tmp){
			dustbunny_debug("critical shell variable reallocation failure");
			return 1;
		}

		shell_variables = tmp;
		tmp = NULL;
		rehash_keys();
	}

	if(insert_key(key, value, overwrite, export)){
		dustbunny_debug("error inserting key");
		return 1;
	}

	return 0;
}

int export_shellvar(char *key) {
	dustbunny_debug("exporting shellvar");

	uint32_t idx;

	if(find_slot('t', key, &idx, 0)) {
		dustbunny_debug("error finding var to export");
		return 1;
	}

	if(export_key(idx)){
		dustbunny_debug("error exporting variable");
		return 1;
	}

	return 0;
}

/// remove a shell variable.
int rem_shellvar(char *key){
	dustbunny_debug("removing shell var");

	// what does this function even need to do bro why does it exist.
	// dont they do this in libc. i'll be fine, right?

	remove_key(key);	
}

/// Get the value of a shell variable.
char *get_shellvar(char *key) {
	dustbunny_debug("finding value for %s shell variable", key);


	uint32_t idx;
	if(find_slot('t', key, &idx, 0)){
		dustbunny_debug("error looking up slot");
	}

	return shell_variables[idx]->value;
}
// Little bits like this is why I love programming.
// Until I don't, of course.
