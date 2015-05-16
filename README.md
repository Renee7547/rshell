# CS 100 - rshell - hw2

## Features
* This is a C++ program properly handle 
	1. input redirection <
	2. output redirection > and >>
	3. file descriptors (extra credits)

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

## Bugs have been solved
1. new cmd and files with old parts, then we need to set them to NULL after each while loop;
2. pipe: the save_stdin should be only restored in the first command.
3. we should close PIPE_READ after each fork, otherwise the child process cannot exit normally when the file is really large.
