#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "headers/version.h"
#include "headers/command.h"

void help(char **argv) {
    printf(
        "_____█________________█___█_____________________________\n" 
        "_____█________________█___█_____________________________\n"
        "__████__█__█___███__████__████___█__█__███___███___█__█_\n" 
        "_█___█__█__█__██______█___█___█__█__█__█__█__█__█__█__█_\n" 
        "_█___█__█__█____█_____█___█___█__█__█__█__█__█__█__█__█_\n" 
        "__████__███___████____█___████___███___█__█__█__█___███_\n" 
        "______________________________________________________█_\n" 
        "___________________________________________________█__█_\n" 
        "____________________________________________________███_\n\n" 
        "DUSTBUNNY %i.%i.%i\n"
        "Copyright (c) %i %s, %s\n\n"
        "Built-in commands:\n\t"
        "help                   - print help and information about dustbunny\n\t"
        "exit/quit              - exit dustbunny\n\t"
        "<path to directory>    - change current directory (like cd)\n\t"
        "cd <path to directory> - change current directory (this is cd)\n\n"
        "View the repo at %s.\n",
        DUSTBUNNY_VERSION_MAJOR,
        DUSTBUNNY_VERSION_MINOR,
        DUSTBUNNY_VERSION_PATCH,
        DUSTBUNNY_RELEASE_YEAR,
        DUSTBUNNY_AUTHOR,
        DUSTBUNNY_LICENSE,
        DUSTBUNNY_REPO
    );
}

void cd(char **argv) {
    if(strcmp(argv[0], "cd") == 0){
        if (get_argc(argv) > 2) {
            printf("dustbunny: Too many arguments\n");
        } else {
            if (chdir(argv[1]) != 0) {
                perror("dustbunny: chdir");
            }else{
                printf("dustbunny: new working dir: '%s'\n",argv[1]);
            }
        }
        return;
    }
    if (get_argc(argv) > 1) {
        printf("dustbunny: Too many arguments\n");
    } else {
        if (chdir(argv[0]) != 0) {
            perror("dustbunny: chdir");
        }else{
            printf("dustbunny: new working dir: '%s'\n",argv[0]);
        }
    }
}
