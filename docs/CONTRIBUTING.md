# Contributing to Dustbunny.
First and fore-most, Dustbunny is a learning project. I am using it to learn C and how Unix shells work, as how to create/parse my own scripting language.
For this reason, if your contribution is a feature, it will likely be rejected.
Feature requests with no implementation, however are fine.
What is really valued are bug fixes, and other code-improvements.

## Setting up.
Dustbunny depends on a library called libdustbunny. libdustbunny is crucial to dustbunny, as it provides required types, debug functions, etc.
It also depends on GNU libreadline*.

Since there are many ways to install libreadline, I suggest you find what method works for your OS/package manager.
As for libdustbunny, the following shell script should work:
```sh
git clone https://github.com/dot-underscore1703/libdustbunny
cd libdustbunny
cmake -S . -B build
cmake --build build
cmake --install build # may need to be ran with sudo
```
and then you want to clone dustbunny:
```sh
git clone https://github.com/dot-underscore1703/dustbunny
cd dustbunny

# OPTIONAL: make build directories and build dustbunny
cmake -B build
cmake --build build
```

## Contribution Guidelines
- Your commit message must follow Conventional Commits.
- Dustbunny operates on Semantic Versioning, so keep that in mind.

*as of version 0.4.0, may change in future, if this file has not been updated please verify this info.
