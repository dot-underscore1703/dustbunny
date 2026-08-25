#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "headers/version.h"
#include "headers/command.h"

int main(int argc, char **argv) {
    if(argc < 2){
        char *input;
        while(1) {
            input = readline(": ");
            add_history(input);
            char **tokens = tokenise(input);
            if(execute(tokens) == -1){
                break;
            }
            free(input);
            free(tokens);
        }

        fprintf(stderr,"dustbunny: goodbye!\n");
    } else {
        if(strcmp(argv[1],"--version") == 0){
            printf(
                "Dustbunny, version %i.%i.%i\n"
                "Copyright (c) %i %s.\n"
                "License %s\n\n"
                "Read the full license at %s\n"
                "You can view the Dustbunny repo at %s\n",
                DUSTBUNNY_VERSION_MAJOR,
                DUSTBUNNY_VERSION_MINOR,
                DUSTBUNNY_VERSION_PATCH,
                DUSTBUNNY_RELEASE_YEAR,
                DUSTBUNNY_AUTHOR,
                DUSTBUNNY_LICENSE,
                DUSTBUNNY_LICENSE_URL,
                DUSTBUNNY_REPO
            );
        }
    }
    return 0;
}
