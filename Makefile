all: rshell ls
			mkdir bin
			mv ./rshell ./bin/rshell
			mv ./ls ./bin/ls
rshell: ./src/main.cpp
	g++ -Wall -Werror -ansi -pedantic -o rshell ./src/main.cpp
ls: ./src/ls.cpp
	g++ -Wall -Werror -ansi -pedantic -o ls ./src/ls.cpp
cp: ./src/cp.cpp
	g++ -Wall -Werror -ansi -pedantic -o cp ./src/cp.cpp
clean:
	rm -rf bin
