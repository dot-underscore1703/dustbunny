#ifndef INPUT_H_
#define INPUT_H_

#include <stddef.h>
typedef struct {
    char *buffer;
    size_t capacity;
    int used;
} UserInput;

int get_input_line(UserInput *user_input);

#endif