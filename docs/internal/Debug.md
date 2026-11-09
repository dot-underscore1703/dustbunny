# Debug Documentation
## DUSTBUNNY_DEBUG
```c
void dustbunny_debug(char *fmt, ...);
```
A function for debugging.
Will check the `is_debug` variable in the debug header file, if it is 1, it will print the text, will not otherwise.
Should be similar to printf usage-wise.
## is_debug
```c
int is_debug;
```
An integer defined in the debug header file. It is used in the DUSTBUNNY_DEBUG macro.
To enable debugging, set it to 1. To disable debugging, set it to a number other than 1 (preferably 0).
It defaults to 0.
