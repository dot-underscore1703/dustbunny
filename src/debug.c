#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "libdustbunny/debug.h"
#include "libdustbunny/libdustbunny.h"

// if 1, debug using dustbunny_debug 
int is_debug = 0;

void dustbunny_debug(char *fmt, ...) {
	if(is_debug) {
		va_list ap;
							/*		 -1 because of terminator		 							*/
		size_t size_needed = (sizeof(program_name) - 1) + (sizeof((": DEBUG: ") - 1) + (sizeof(fmt) - 1)) + 2; // +2 for newline and terminator
		char *format_buffer = malloc(size_needed);

		snprintf(format_buffer, size_needed, "%s: DEBUG: %s\n", program_name, fmt);
		
		va_start(ap,fmt);
		vfprintf(stderr, format_buffer,ap);
		va_end(ap);
	}
}
