# CS 100 - rshell - hw0

## Features
* This is a basic command shell.
* It will print a command prompt.
* It can read in a command on one line.
* It can process connectors: ';', '||', '&&'.
* A built-in command of 'exit' which exits my shell.
* Anything that appears after a `#` character should be considered a comment.



## How to use
Run the following commands
```
$ git clone https://github.com/Renee7547/rshell.git
$ cd rshell
$ git checkout hw0
$ make
$ bin/rshell`
```
## Test cases
###case 1
```
' ' '\n'
ls
pwd
bin/rshell
     ls
	 pwd
ls -a
ls -l
ls    -a  -l      -R
ls -R
ls -lR
ls -l -R
ls -R -l
ls -a -a -a -a -a -a -a -a -a -a -a -a    -a
ls filename filename cmd // cmd can be at any position
```
###case 2
```
ls || pwd
abcd || pwd
ls && pwd
abcd && pwd
ls; pwd
abcd; pwd
pwd; ls; ls || ls && ls ||| ls ; pwd||sdad &&adasdf ; ls
ls -l||cat    ; rm -rf *
```
###case 3
```
exit
exit -flag daffsg
ls -l exit
ls -l; exit
###case 4
ls -l # this is a comment
/#ls comment fdfsdfa
ls -l# this is a comment here
```
###extra test cases
```
|||||||||||;;;;;;;;;;;;;;;;;;;;;&&&&&&&&&&&&&&&&&&&&
&&&&&
;;;;;
      ;ls
	  &&ls
	  ||pwd
1dferytyrtgeufhisdjnfksdjnfksdjnfruhfiweruhfiewrhufieruhfeiruhfieurh
```

## Issues
1. Unlike the real bash shell that continue to ask for input after connectors, 
the rshell will treat this case as the command following connectors is null and skip to the next loop.
2. This program cannot run commands in current directory.
It can run bin/rshell, but cannot run rshell from within rshell. 


## Bugs have been solved
1. Problem with  fgets() function: 
	A newline character is considered a valid character by it and included in the string copied to str. So I should convert "\n" to "\0".
2. String has no end:
	In the strcpy() function, we should copy "strlen() + 1" character into command to include '\o' at the end.
3. Segmentation fault:
	In the parse() function, we should put the str.push_back() before the strtok() function, for the last string would be NULL.

