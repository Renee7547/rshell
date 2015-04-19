all: rshell
			mkdir bin
			mv ./rshell ./bin/rshell
rshell: ./src/main.cpp
	g++ -Wall -Werror -ansi -pedantic -o rshell ./src/main.cpp
