# CS 100 - rshell - hw1

## Features
* This is a C++ program that implements a subset of the GNU ls command.
* supports -l, -a, and -R optional flags
* and the ability to pass in zero, one, or many files

## Folders
"test" and "test1" are for testing.
ls.script is in tests folder.

## How to use
Run the following commands
```
$ git clone https://github.com/Renee7547/rshell.git
$ cd rshell
$ git checkout hw1
$ make
$ bin/rshell`
```
## Test cases
They are in rshell/tests

## Issues
1. There are some issues with recursively sorting(-R).
	I will fix it as soon as possible.
2. Unlike the real bash, the files are listed ascending from left to right.

## Bugs have been solved
1. Unable to print out "total" for -l:
	Then I modified the process(), try to deal with it in two cases(-a, and no -a). 
	For those without -a, I just eliminating them from the path list.
2. Some small bugs:
	Like segmentation fault, opendir() error, and recognizing file or folder.
		
