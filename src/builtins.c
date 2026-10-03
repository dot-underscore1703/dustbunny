// ====================================
// BUILTINS.C - DEFINE BUILTIN COMMANDS
// ====================================

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "version.h"

size_t __get_argc(char **argv) {
	size_t i = 0;
	while(*argv != NULL) {
		++i;
		++*argv;
	}
	*argv -= i;
	return i;
}

/// Builtin command for printing help screen and usage, alongside other details like version, license etc.
void builtins_help(char **argv) {
	if(__get_argc(argv) > 1){
		if(strcmp(argv[1],"help") == 0) {
			printf("print help and information about dustbunny or a specific command");
		} else if(strcmp(argv[1],"exit") == 0) {
			printf("exit dustbunny\n");

		} else if(strcmp(argv[1],"quit") == 0) {
			printf("exit dustbunny\n");

		} else if(strcmp(argv[1],"cd") == 0) {
			printf(
				"change current directory\n"
				"you may also omit the 'cd' and simply type a directory path, dustbunny will put you there anyways\n"
			);
			
		}else if(strcmp(argv[1],"echo") == 0){
			printf("print args\n");

		}else if(strcmp(argv[1],"pwd") == 0){
			printf("print working directory\n");
		}
		return;
	}
	printf(
		"_____█________________█___█_____________________________\n" 
		"_____█________________█___█_____________________________\n"
		"__████__█__█___███__████__████___█__█__███___███___█__█_\n" 
		"_█___█__█__█__██______█___█___█__█__█__█__█__█__█__█__█_\n" 
		"_█___█__█__█____█_____█___█___█__█__█__█__█__█__█__█__█_\n" 
		"__████___███__████____█___████____███__█__█__█__█___███_\n" 
		"______________________________________________________█_\n" 
		"___________________________________________________█__█_\n" 
		"____________________________________________________███_\n\n" 
		"DUSTBUNNY %i.%i.%i\n"
		"Copyright (c) %i Rory Lane, MIT\n\n"
		"Built-in commands:\n\t"
		"help <builtin>			- print help and information about dustbunny or a specific command\n\t"
		"exit/quit				- exit dustbunny\n\t"
		"<path to directory>	- change current directory (like cd)\n\t"
		"cd <path to directory>	- change current directory (this is cd)\n\t"
		"echo <args>			- print args\n\t"
		"pwd					- print working directory\n\n"
		"View the repo at https://github.com/dot-underscore1703/dustbunny.\n",
		DUSTBUNNY_VERSION_MAJOR,
		DUSTBUNNY_VERSION_MINOR,
		DUSTBUNNY_VERSION_PATCH,
		
		DUSTBUNNY_RELEASE_YEAR
	);
}

/// Builtin command for changing of directories. The user may either simply enter a path to a directory, or prefix the path with the 'cd' command.
void builtins_cd(char **argv) {
	if(strcmp(argv[0], "cd") == 0){
		if (__get_argc(argv) > 2) {
			fprintf(stderr,"dustbunny: Too many arguments\n");
		} else {
			if (chdir(argv[1]) != 0) {
				perror("dustbunny: chdir");
			}else{
				printf("dustbunny: new working dir: '%s'\n",argv[1]);
			}
		}
		return;
	}
	if (__get_argc(argv) > 1) {
		fprintf(stderr,"dustbunny: Too many arguments\n");
	} else {
		if (chdir(argv[0]) != 0) {
			perror("dustbunny: chdir");
		}else{
			printf("dustbunny: new working dir: '%s'\n",argv[0]);
		}
	}
}

/// Builtin command for echoing (printing) strings.
void builtins_echo(char **argv){
	while(*argv != NULL) {
		printf("%s",*argv);
		++argv;
	}
}

/// Builtin command for printing working directory.
void builtins_pwd(){
	char *pwd = getcwd(NULL,32);	
	if(pwd != NULL){
		printf("%s\n",pwd);
		free(pwd);
	} else {
		perror("dustbunny");
	}
	return;	
}
