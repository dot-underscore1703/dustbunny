#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "headers/version.h"
#include "headers/command.h"

void help(char **argv) {
    printf(
        "DUSTBUNNY %i.%i.%i\n\t"
        "Copyright (c) %i %s, %s\n\n"
        "Built-in commands:\n\t"
        "help                   - print help and information about dustbunny\n\t"
        "exit/quit              - exit dustbunny\n\t"
        "why <command>          - print debug info on how the shell worked\n\t"
        "<path to directory>    - change current directory (like cd)\n\n"
        "Rather than a standard 'cd <path>', dustbunny simply uses <path>.\n\n"
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
                printf("dustbunny: change working dir to '%s'\n",argv[0]);
            }
        }
    }
    if (get_argc(argv) > 1) {
        printf("dustbunny: Too many arguments\n");
    } else {
        if (chdir(argv[0]) != 0) {
            perror("dustbunny: chdir");
        }else{
            printf("dustbunny: change working dir to '%s'\n",argv[0]);
        }
    }
}
