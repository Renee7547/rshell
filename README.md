# CS 100-rshell-hw0

## Features
This program is a simple command shell called rshell.
1. It will print a command prompt.
2. It can read in a command on one line
3. It can process connectors: ';', '||', '&&'


## How to use
run the following commands
```
$ git clone https://github.com/Renee7547/rshell.git
$ cd rshell
$ git checkout hw0
$ make
$ bin/rshell`
```
## Issues
Unlike the real bash shell that continue to ask for input after connectors, 
the rshell will treat this case as the command following connectors is null and skip to the next loop.

## Bugs have been solved
1. Problem with  fgets() function: 
	A newline character is considered a valid character by it and included in the string copied to str. So I should convert "\n" to "\0".
2. String has no end:
	In the strcpy() function, we should copy "strlen() + 1" character into command to include '\o' at the end.
3. Segmentation fault:
	In the parse() function, we should put the str.push_back() before the strtok() function, for the last string would be NULL.

