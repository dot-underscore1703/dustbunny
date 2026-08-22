#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include "headers/memory.h"
#include "headers/builtins.h"

#define CMD_IS(x) (strcmp(argv[0],x) == 0)
#define WHY(x) if(is_why == 1){ printf("dustbunny: %s\n",x); }

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
                char *tmp_arg = malloc(
                    strlen(home_directory) + strlen(argv[i])
                );

                snprintf(tmp_arg, strlen(home_directory) + strlen(argv[i]),
                    "%s%s", home_directory, argv[i] + 1);

                argv[i] = tmp_arg;
            }
        }
    }
}

char **tokenise(char *line) {
    size_t bufsize = 16, position = 0;
    char **tokens = malloc(bufsize * sizeof(char *));

    char *token;

    chk_alloc(tokens);

    token = strtok(line, "\t\r\n\a ");
    while(token != NULL) {
        tokens[position] = token;
        ++position;

        if(position >= bufsize) {
            void *tmp_ptr = tokens;
            double_buffer_size(&tmp_ptr, &bufsize);
            tokens = tmp_ptr;
        }

        token = strtok(NULL, "\t\r\n\a ");
    }
    tokens[position] = NULL;

    expand_args(tokens);

    return tokens;
}

int launch(char **argv, int is_why) {
    pid_t pid, wpid;

    int status;

    WHY("forking process");
    pid = fork();
    WHY("process forked")
    if(pid == 0) {
        WHY("executing command");
        if(execvp(argv[0],argv) == -1) {
            perror("dustbunny");
        }
        exit(EXIT_FAILURE);
    } else if(pid < 0) {
        perror("dustbunny");
    } else {
        WHY("waiting pid")
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    WHY("done")
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
    }else if(CMD_IS("why")) {
        launch(argv + 1, 1);
        return 0;
    }else if(CMD_IS("exit") || CMD_IS("quit")) {
        return -1;
    }

    launch(argv, 0);

    return 0;
}