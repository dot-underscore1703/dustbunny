#include <stdio.h>
#include <stdlib.h>
#include "headers/input.h"
#include "headers/command.h"

int main() {
    char *buffer_ptr = malloc(10);
    UserInput user_input = {buffer_ptr,10,0};

    int input_length;

    while((printf(": ") != 0) && (input_length = get_input_line(&user_input)) >= 0) {
        user_input.used = 0;
        //printf("%i\n",input_length);
        //printf("Input: %s\n", user_input.buffer);
        char **tokens = tokenise(user_input.buffer);
        if(execute(tokens) == -1){
            break;
        }
    }

    free(user_input.buffer);
    fprintf(stderr,"dustbunny: goodbye!\n");
    return 0;
}
