all: rshell
			mkdir bin
			mv ./rshell ./bin/rshell
			mv ./cp ./bin/cp
rshell: ./src/main.cpp
	g++ -Wall -Werror -ansi -pedantic -o rshell ./src/main.cpp
cp: ./src/main.cpp
	g++ -Wall -Werror -ansi -pedantic -o cp ./src/main.cpp

clean:
	rm -rf bin
