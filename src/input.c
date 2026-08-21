#include <stdio.h>
#include <stdlib.h>
#include "headers/memory.h"

typedef struct {
    char *buffer;
    size_t capacity;
    int used;
} UserInput;

int get_input_line(UserInput *user_input);
int add_char_to_buffer(UserInput *user_input, char to_add);
int double_buffer_capacity(UserInput *user_input);

/// Get a line of input to a buffer, return the length of the buffer and dynamically allocate more memory if needed.
int get_input_line(UserInput *user_input) {

    // Initialise where we store the character from getchar()
    char character;

    //printf("old capacity: %zu\n",user_input->capacity);

    // Just check the buffer before we continue.
    chk_alloc(user_input->buffer);

    int i;

    // Loop over chars from getchar() until newline or EOF
    for(i = 0; (character = getchar()) != '\n' && character != EOF; i = user_input->used) {
        //printf("Adding next char...\n");
        add_char_to_buffer(user_input, character);
    }
    user_input->buffer[user_input->used] = '\0';

    //printf("new capacity: %zu\n",user_input->capacity);

    //printf("Used: %i\n",user_input->used);
    //printf("\nInput: %s\n",user_input->buffer);
    return user_input->used;
}

int add_char_to_buffer(
    UserInput *user_input,
    char to_add
){
    //printf("Char: %c\n",to_add);
    int next_index = user_input->used;
    // printf("Next idx: %i\n",next_index);

    if(user_input->capacity < next_index) {
        void *tmp_ptr = user_input->buffer;
        double_buffer_size(&tmp_ptr, &user_input->capacity);
        user_input->buffer = tmp_ptr;
    }
    user_input->buffer[next_index] = to_add;
    //printf("Added: %c\n",user_input->buffer[next_index]);
    ++user_input->used;
    //printf("Success!\n");
    return 0;
}