ll: rshell
			mkdir bin
			cp ./rshell ./bin/rshell
rshell: ./src/rshell.cpp
	g++ -Wall -Werror -ansi -pedantic -o rshell ./src/main.cpp
