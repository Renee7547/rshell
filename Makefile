all: rshell
			mkdir bin
			mv ./rshell ./bin/rshell
			mv ./cp ./bin/cp
rshell: ./src/main.cpp
	g++ -Wall -Werror -ansi -pedantic -o rshell ./src/main.cpp
cp: ./src/cp.cpp
	g++ -Wall -Werror -ansi -pedantic -o cp ./src/cp.cpp

clean:
	rm -rf bin
