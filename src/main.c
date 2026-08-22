#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "headers/command.h"

int main() {
    char *input;
    while(1) {
        input = readline(": ");
        char **tokens = tokenise(input);
        add_history(input);
        if(execute(tokens) == -1){
            break;
        }
    }

    //free(buffer_ptr);
    fprintf(stderr,"dustbunny: goodbye!\n");
    return 0;
}
