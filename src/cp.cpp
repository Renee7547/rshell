#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>

using namespace std;

bool exists(const char* file)
{
	struct stat buffer;
	/*int check = stat(file, &buffer);
	if(check == -1)
	{
		perror("stat");
		_exit(1);
	}*/
	return (stat (file, &buffer) == 0);
}

void method1(char* in, char* out)
{
	ifstream inf(in);
	ofstream outf(out);	
	
	
	char c;
	while (inf.get(c))
	{
		outf.put(c);
	}
	inf.close();
}

void method2(char* in, char* out)
{
	int fdnew;
	int fdold;
	if(-1 == (fdnew = open(out, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR)))
	{
		perror("open()");
		_exit(1);
	}
	if(-1 == (fdold = open(in, O_RDONLY)))
	{
		perror("open()");
		_exit(1);
	}

	int sz;
	char c[1];
	if(-1 == (sz = read(fdold, c, sizeof(c))))
	{
		perror("read()");
		_exit(1);
	}

	while(sz > 0)
	{
		if(-1 == write(fdnew, c, sz))
		{
			perror("write()");
			_exit(1);
		}
		if(-1 == (sz = read(fdold, c, sizeof(c))))
		{
			perror("read()");
			_exit(1);
		}
	}
	
	if(-1 == close(fdnew))
	{
		perror("close()");
		_exit(1);
	}
	if(-1 == close(fdold))
	{
		perror("close()");
		_exit(1);
	}
}

void method3(char* in, char* out)
{
	int fdnew;
	int fdold;
	if(-1 == (fdnew = open(out, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR)))
	{
		perror("open()");
		_exit(1);
	}
	if(-1 == (fdold = open(in, O_RDONLY)))
	{
		perror("open()");
		_exit(1);
	}

	int sz;
	char c[BUFSIZ];
	if(-1 == (sz = read(fdold, c, sizeof(c))))
	{
		perror("read()");
		_exit(1);
	}

	while(sz > 0)
	{
		if(-1 == write(fdnew, c, sz))
		{
			perror("write()");
			_exit(1);
		}
		if(-1 == (sz = read(fdold, c, sizeof(c))))
		{
			perror("read()");
			_exit(1);
		}
	}
	
	if(-1 == close(fdnew))
	{
		perror("close()");
		_exit(1);
	}
	if(-1 == close(fdold))
	{
		perror("close()");
		_exit(1);
	}
}

int main(int argc, char *argv[])
{
	if(argc < 3 || argc > 4)
	{
		cout << "wrong number of arguments" << endl;
	}
	
	if(exists(argv[2]))
	{
		cout << "ERROR: output file already exists" << endl;
		exit(1);
	}
	method2(argv[1], argv[2]);	
	
	return 0;
}
