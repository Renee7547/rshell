# CS 100 - rshell - hw2

## Features
### This is a C++ program properly handle 
1. input redirection <
2. output redirection > and >>
3. pipe |
4. <<< (extra credits)
5. file descriptors (extra credits)

## Folders
piping.script is in tests folder.

## How to use
Run the following commands
```
$ git clone https://github.com/Renee7547/rshell.git
$ cd rshell
$ git checkout hw2
$ make
$ bin/rshell
```
## Test cases
They are in rshell/tests

## Issues
1. This program would ignore any quotes " or ' in the input line.
2. connectors function has been deleted, so two consecutive '|' whould be considered as syntax error.
3. for extra credits <<<: file descriptor whould not work for '<<<'.
4. any numbers before redirection whould be considered as file descriptor.

## Bugs have been solved
1. new cmd and files with old parts, then we need to set them to NULL after each while loop;
2. pipe: the save_stdin should be only restored in the first command.
3. we should close PIPE_READ after each fork, otherwise the child process cannot exit normally when the file is really large.
4. syntax error detected: consecutive pipe and redirection symbols whould be considered as an error.
