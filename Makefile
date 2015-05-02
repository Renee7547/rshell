all: rshell ls
			mkdir bin
			mv ./rshell ./bin/rshell
<<<<<<< HEAD
			mv ./ls ./bin/ls
rshell: ./src/main.cpp
	g++ -Wall -Werror -ansi -pedantic -o rshell ./src/main.cpp
ls: ./src/ls.cpp
	g++ -Wall -Werror -ansi -pedantic -o ls ./src/ls.cpp
=======
			mv ./cp ./bin/cp
rshell: ./src/main.cpp
	g++ -Wall -Werror -ansi -pedantic -o rshell ./src/main.cpp
cp: ./src/cp.cpp
	g++ -Wall -Werror -ansi -pedantic -o cp ./src/cp.cpp

>>>>>>> 725c05e5dcfb1adc520a015a75bbc62eb989c0e9
clean:
	rm -rf bin
