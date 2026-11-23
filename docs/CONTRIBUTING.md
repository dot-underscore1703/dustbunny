# Contributing to Dustbunny.
First and fore-most, Dustbunny is a learning project. I am using it to learn C and how Unix shells work, as how to create/parse my own scripting language.
For this reason, if your contribution is a feature, it will likely be rejected.
Feature requests with no implementation, however are fine.
What is really valued are bug fixes, and other code-improvements.

## Setting up.
Note: Dustbunny depends on GNU libreadline*. Since I do not know your OS/package manager, you will need to figure out how you install libreadline.
Note: Dustbunny uses the HelpMake build system. If you have not heard of HelpMake before, you can view it at https://github.com/Gag91/HelpMake/
	Although, Dustbunny is also configured to use CMake, though the CMakeLists.txt file may not be kept up to date every update. If you need CMakeLists.txt updated, open a PR.

Clone dustbunny:
```sh
git clone https://github.com/dot-underscore1703/dustbunny
cd dustbunny

# OPTIONAL: make build directories and build dustbunny
hm -b
```

## Contribution Guidelines
- Your commit message must follow Conventional Commits.
- Dustbunny operates on Semantic Versioning, so keep that in mind.
- Pull requests are not to go onto the master branch. Please submit pull requests to the dev branch instead.
- AI usage, in code or documentation, is disallowed in any way or form in regards to public contributions.

*as of version 0.4.0, may change in future, if this file has not been updated please verify this info.
