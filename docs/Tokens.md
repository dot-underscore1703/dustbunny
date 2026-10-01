# Tokens Documentation
## Token structure
```c
typedef struct Token {
  TokenType type;
  char *value;
} Token;
```
A token in libdustbunny contains two simple fields.
The first is the type, which contains the type of the token so the parser knows how to use it when building the AST.
The first is the value, a text string that contains important values for the parser such as arguments for the argv array or filepaths.

## Token operations
### token_new
```c
Token *token_new(TokenType type, char *value)
```
token_new: Create new token
Takes a TokenType and a char*.
The type parameter is the type of token you wish to create.
The value parameter is a string value you wish to be associated with the token.

token_new will allocate memory for a new token and properly set its fields to the values provided.
token_new will return a pointer to the new token.

### token_destroy
```c
int token_destroy(Token *token)
```
token_destroy: Destory the token at the pointer.
Take a Token*.
The token parameter is a pointer to the token you wish to destroy.

token_destroy will call free() on the token.
Note:	If the tokens value was dynamically allocated, you must manually call free() on it yourself before destroying the token, or you end up with a variable with no pointer. 
		Otherwise it should just go out of scope.

Returns 1 on failure, 0 on success.
Fails if the Token pointer provided is NULL.

### token_type_as_str
```c
char *token_type_as_str(char *buffer, size_t buffer_len, Token *token)
```
token_type_as_str: Get the type of the Token* passed to the function as a string.
Takes a char*, size_t and a Token*
The buffer parameter is the buffer you wish to store the string in.
The buffer_len parameter is the size of the buffer.
The token parameter is a pointer you wish to get the stringified type of.

token_type_as_str can return a string up to 15 bytes in length (including terminator). For this reason, it will check that buffer_len > 15 regardless of what it will actually use.
For this reason, you should make sure that the buffer you give it atleast 15 bytes in length.

Returns a char* string on success, NULL on failure.
Fails if buffer_len is less than 15.
