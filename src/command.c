#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include "headers/memory.h"
#include "headers/version.h"

#define CMD_IS(x) (strcmp(argv[0],x) == 0)

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

    return tokens;
}

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
                
            }
        }
    }
}

int execute(char **argv) {
    struct stat path_stat;
    pid_t pid, wpid;

    int status;

    if(argv[0] == NULL) {
        return 1;
    }

    if (stat(argv[0], &path_stat) == 0 && S_ISDIR(path_stat.st_mode)) {
        if (argv[1] != NULL) {
            printf("dustbunny: Too many arguments\n");
        } else {
            if (chdir(argv[0]) != 0) {
                perror("dustbunny: chdir");
            }
        }
        return 0; 

    }else if(CMD_IS("cd")){
        printf("dustbunny: cd does not exist. run 'help' for more info.");
        return 0;
    }else if(CMD_IS("help")){
        printf(
            "DUSTBUNNY %i.%i.%i\n\tCopyright (c) %i %s, %s\n\nBuilt-in commands:\n\thelp\n\texit\n\t<path to directory>\n\nRather than a standard 'cd <path>', dustbunny simply uses <path>.\n\nView the repo at %s.\n",
            DUSTBUNNY_VERSION_MAJOR,
            DUSTBUNNY_VERSION_MINOR,
            DUSTBUNNY_VERSION_PATCH,
            DUSTBUNNY_RELEASE_YEAR,
            DUSTBUNNY_AUTHOR,
            DUSTBUNNY_LICENSE,
            DUSTBUNNY_REPO
        );
        return 0;
    }else if(CMD_IS("exit")) {
        return -1;
    }

    pid = fork();
    if(pid == 0) {
        //printf("argv[0] = [%s]\n", argv[0]);
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