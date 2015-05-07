#include <iostream>
#include <fstream>
#include <vector>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <cstdlib>
#include <errno.h>
#include <string>
#include <sys/wait.h>

using namespace std;

int main(int argc, char** argv)
{
	if( argc > 4 )
	{
		cerr << "Incorrect amount of arguments" << endl;
		exit(1);
	}

	if( argc == 1)
	{
		cerr << "No arguments" << endl;
		exit(1);
	}

	if(argc == 2)
	{
		cerr << "No destination path has been passed in" << endl;
		exit(1);
	}

	struct stat s;
	struct stat d;

	bool destination = false;

	if(stat(argv[1], &s) == -1)
	{
		perror("stat. ");
		exit(1);
	}

	if(stat(argv[2], &d) == -1)
	{
		destination = true;
	}

	string source = argv[1];
	string destin = argv[2];
	
	int result;
	result = access(argv[2], F_OK);
	if( result == 0 && !S_ISDIR(d.st_mode))
	{
		cerr << "File " << argv[2] << " exists already" << endl;
		exit(1);
	}

	if(S_ISDIR(d.st_mode))
	{
		destin = destin + '/' + source;
	
		if(link(source.c_str(), destin.c_str()) == -1)
		{
			perror("link. ");
		}
		if(unlink(source.c_str()) == -1)
		{
			perror("unlink. ");
		}
	}
	
	else if(destination)
	{
		if(link(source.c_str(), destin.c_str()) == -1)
		{
			perror("link. ");
			exit(1);
		}	
		if(unlink(source.c_str()) == -1)
		{
			perror("unlink. ");
			exit(1);
		}			
	}
}
