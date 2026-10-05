#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "debug.h"
#include "dustbunny.h"

// if 1, debug using dustbunny_debug 
int is_debug = 0;

void dustbunny_debug(char *fmt, ...) {
	if(is_debug) {
		va_list ap;

		// +1 for terminator.
		size_t size_needed = (strlen(dustbunny_program_name) + strlen(": DEBUG: \n") + strlen(fmt)) + 1;
		char *format_buffer = malloc(size_needed);

		// Copy the formatted debug message to a buffer
		snprintf(format_buffer, size_needed, "%s: DEBUG: %s\n", dustbunny_program_name, fmt);

		va_start(ap,fmt);
		vfprintf(stderr, format_buffer,ap);
		va_end(ap);

		free(format_buffer);
		format_buffer = NULL;
	}
}
