#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include "headers/builtins.h"

#define CMD_IS(x) (strcmp(argv[0],x) == 0)

int get_argc(char **argv){
    if(argv == NULL){ 
        return 0; 
    }
    int i = 0;
    while(argv[i] != NULL) ++i;

    return i;
}

void expand_args(char **argv) {
    for(int i = 0; i < get_argc(argv); ++i) {
        switch(argv[i][0]) {
            case '~': {
                char *home_directory = getenv("HOME");
                if(home_directory == NULL) {
                    printf("dustbunny: could not get home directory\n");
                    return;
                }
                size_t new_len = strlen(home_directory) + strlen(argv[i] + 1) + 1;

                char *expanded_path = malloc(new_len);

                strcpy(expanded_path, home_directory);
                strcat(expanded_path, argv[i] + 1);
                expanded_path[new_len] = '\0';

                argv[i] = expanded_path;
            }
        }
    }
}

char **tokenise(char *line) {
    size_t max_tokens = 16;
    size_t max_token_len = 128;

    char **tokens = malloc(max_tokens * sizeof(char *));
    size_t num_tokens = 0;

    char *token = malloc(max_token_len * sizeof(char));
    size_t token_len = 0;

    size_t input_len = strlen(line);

    int in_quote = 0;

    for(size_t i = 0; i < input_len; ++i) {
        switch(line[i]) {
            case '\\': {
                token[token_len] = line[++i];
                ++token_len;
                break;
            }

            case '\'': {
                if(in_quote == 1){
                    in_quote = 0;
                } else {
                    in_quote = 1;
                }
                break;
            }

            case ' ': {
                if(in_quote == 1){
                    token[token_len] = line[i];
                    ++token_len;
                } else if(line[i - 1] != ' '){
                    token[token_len] = '\0';
                    tokens[num_tokens] = token;
                    token = malloc(max_token_len * sizeof(char));
                    ++num_tokens;
                    token_len = 0;
                }
                break;
            }
            default: {
                token[token_len] = line[i];
                ++token_len;
                break;
            }
        }
    }
    
    tokens[num_tokens] = token;
    ++num_tokens;

    tokens[num_tokens] = NULL;

    expand_args(tokens);

    return tokens;
}

int launch(char **argv) {
    pid_t pid, wpid;

    int status;

    pid = fork();
    if(pid == 0) {
        if(execvp(argv[0],argv) == -1) {
            perror("dustbunny");
        }
        exit(EXIT_FAILURE);
    } else if(pid < 0) {
        perror("dustbunny");
    } else {
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 0;
}

int execute(char **argv) {
    struct stat path_stat;

    if(argv[0] == NULL) {
        return 1;
    }

    if (stat(argv[0], &path_stat) == 0 && S_ISDIR(path_stat.st_mode)) {
        cd(argv);
        return 0; 
    }else if(CMD_IS("cd")){
        cd(argv);
        return 0;
    }else if(CMD_IS("help")){
        help(argv);
        return 0;
    }else if(CMD_IS("exit") || CMD_IS("quit")) {
        return -1;
    }

    launch(argv);

    return 0;
}