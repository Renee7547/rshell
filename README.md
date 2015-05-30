# CS 100 - signals - hw3

## Features
### This is a C++ program properly handle 
1. the cd command: cd <PATH>, cd, cd -
2. prompt displays the current working dir
3. ^C
4. ^Z, fg, bg
5. home folder to "~"

## Folders
signals.script is in tests folder.

## How to use
Run the following commands
```
$ git clone https://github.com/Renee7547/rshell.git
$ cd rshell
$ git checkout hw3
$ make
$ bin/rshell
```
## Test cases
They are in rshell/tests

## Issues
1. In real bash, we run "cat", "^Z", "bg", "pwd"(any command) in order, 
the "pwd"(any cmd) run without "cat" running;
But in my program, "cat" and "pwd"(any command) run at the same time.

## Bugs have been solved
1. when multiple processes were sent to the background, 
we should use "fg" to run them one by one. 
So we should reserve the pid(s) to a list, and the stoped pid(s) to another list.
2. when "bg" was used, I could still run "^C" and "^Z" to modify the bg process, 
while they are not supposed to influence the background process. 
	So I ignore SIGINT and SIGTSTP in child process.
