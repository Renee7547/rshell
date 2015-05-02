#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "Timer.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>

using namespace std;

bool file_exist(char *name)
{
	struct stat buf;
	return (stat(name, &buf) == 0);
}

void method1(char *in, char *out)
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

void method2(char *in, char *out)
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
	}																				if(-1 == close(fdnew))
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

void method3(char *in, char *out)
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
	}																				if(-1 == close(fdnew))
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

int main (int argc, char **argv)
{
	Timer t;
	double wc;
	double ut;
	double st;
	if (argc == 3)
	{
		if (file_exist(argv[2]))
		{
			cout << "File already exists" << endl;
			exit(1);
		}
		else
			method3(argv[1], argv[2]);
	}
	else if (argc == 4)
	{
		if (file_exist(argv[2]))
		{
			cout << "File already exists" << endl;
			exit(1);
		}
		else
		{
			t.start();
			method1(argv[1], argv[2]);
			t.elapsedWallclockTime(wc);
			t.elapsedUserTime(ut);
			t.elapsedSystemTime(st);
			cout << "wc: " << wc << " ut: " << ut << " st: " << st << endl;
			
			t.start();
			method2(argv[1], argv[2]);
			t.elapsedWallclockTime(wc);
			t.elapsedUserTime(ut);
			t.elapsedSystemTime(st);
			cout << "wc: " << wc << " ut: " << ut << " st: " << st << endl;

			t.start();
			method3(argv[1], argv[2]);
			t.elapsedWallclockTime(wc);
			t.elapsedUserTime(ut);
			t.elapsedSystemTime(st);
			cout << "wc: " << wc << " ut: " << ut << " st: " << st << endl;
	
		}
	}
	else 
	{
		cout << "please type in 3 or 4 arguments";
	}
	
	return 0;
}

