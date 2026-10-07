# Dustbunny changelog
## 0.4.0
### Updates
- Changed long args to short args. E.g --version -> -v --help -> -h 
- Updated help command to show specific help on builtin commands.

### New
- Added a builtin echo command.
- Added a builtin pwd command.
- Added -d arg to enable debugging.
- Added -t arg to print tokens after tokenisation without using them.
- Added -c arg to input command via arg for early login shell support.

### Fixes
- Fixed memory bug where argument buffer size and the amount of arguments would not grow when it reached the limit.
- Fixed bug where extra bytes would be sent alongside input.
- Fixed character 'u' on banner printed in the help screen.

### Internal
- Totally updated tokeniser to actually produce tokens rather than strings of text.
- Added token preprocessor.
- Added token parser to parse tokens into an actual syntax tree.

## 0.3.0
- Removed 'why' built-in shell command.
- Fixed a bug for built-in cd command where arguments were not passed correctly.
- Added single-quoted argument support.
- Added --version arg to print information on software version.


## 0.2.0
- Added 'why' built-in shell command.
- Switched to libreadinput library, history and cursor control now exist properly.
- Added support for expanding the '~' character to the users home directory.
