#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <map>
#include <string.h>
#include <vector>
#include <time.h>
#include <grp.h>
#include <pwd.h>
#include <iomanip>

#define MAXROWLEN 80 // the maximum length of a row

int g_leave_len = MAXROWLEN; // the length left, for aligning
int g_maxlen; // store the maximum len of filename

using namespace std;

// list files in directory called dirname
void printNoflag(const char *dirname)
{	
	DIR *dirp;	// the directory
	if(NULL == (dirp = opendir(dirname)))
	{
		perror("There was an error with opendir(). ");
		exit(1);
	}
	struct dirent *filespecs; // each entry
	errno = 0;
	while(NULL != (filespecs = readdir(dirp)))
	{
		cout << filespecs->d_name << " ";
	}
	if(errno != 0)
	{
		perror("There was an error with readdir(). ");
		exit(1);
	}
	cout << endl;
	if(-1 == closedir(dirp))
	{
		perror("There was an error with closedir(). ");
		exit(1);
	}
}

// print out one filename
void printSingle(const char *dirname)
{
	int len;
	// if the current line cannot hold the filename, then go to the next line
	if (g_leave_len < g_maxlen)
	{
		cout << endl;
		g_leave_len = MAXROWLEN;
	}
	len = strlen(dirname);
	len = g_maxlen - len;
	cout << dirname;
	for (int i = 0; i < len; ++i)
	{
		cout << " ";
	}
	cout << " ";
	// leave two spaces
	g_leave_len -= g_maxlen + 2;
}

void printAll(const char * dirname)
{
	DIR *dirp;	// the directory
	if(NULL == (dirp = opendir(dirname)))
	{
		perror("There was an error with opendir(). ");
		exit(1);
	}
	struct dirent *filespecs; // each entry
	errno = 0;
	while(NULL != (filespecs = readdir(dirp)))
	{
		cout << filespecs->d_name << " ";
	}
	if(errno != 0)
	{
		perror("There was an error with readdir(). ");
		exit(1);
	}
	cout << endl;
	if(-1 == closedir(dirp))
	{
		perror("There was an error with closedir(). ");
		exit(1);
	}
}

void printLong(const char *dirname)
{
	struct stat s;
	char time[32]; // store time

	if (stat(dirname, &s) == 0)
	{
		if (S_ISDIR(s.st_mode)) cout << "d"; else cout << "-";
		if (s.st_mode & S_IRUSR) cout << "r"; else cout << "-";
		if (s.st_mode & S_IWUSR) cout << "w"; else cout << "-";
		if (s.st_mode & S_IXUSR) cout << "x"; else cout << "-";
		if (s.st_mode & S_IRGRP) cout << "r"; else cout << "-";
		if (s.st_mode & S_IWGRP) cout << "w"; else cout << "-";
		if (s.st_mode & S_IXGRP) cout << "x"; else cout << "-";
		if (s.st_mode & S_IROTH) cout << "r"; else cout << "-";
		if (s.st_mode & S_IWOTH) cout << "w"; else cout << "-";
		if (s.st_mode & S_IXOTH) cout << "x"; else cout << "-";
		cout << " " << s.st_nlink << " "
			<< getpwuid(s.st_uid)->pw_name << " "
			<< getgrgid(s.st_gid)->gr_name << " "
			<< setw(5) << s.st_size << " ";
		//strcpy(time, ctime(&s.st_mtime));
		//time[strlen(time) - 1] = '\0'; // delete \n
		strftime(time, sizeof(time), "%b %d %H:%M", localtime(&s.st_mtime));
		cout << time << " ";
		//cout << dirname << endl;
	}
	else
	{
		perror("There was an error with stat. ");
	}
}

void printRecur(char *dirname)
{

}

void printF(char flag, const char *dirname)
{
	unsigned int i;
	int j;
	//struct stat buf;
	char name[NAME_MAX+1];
	for (i = 0, j = 0; i < strlen(dirname); ++i)
	{
		if (dirname[i] == '/')
		{
			j = 0;
			continue;
		}
		name[j++] = dirname[i];
	}
	name[j] = '\0';
	
	switch(flag)
	{
		case '0':	// no flag
			if (name[0] != '.')
			{
				printSingle(name);
			}
			break;
		case 'a':	// flag -a
			printSingle(name);
			break;
		case 'l':	// flag -l
			// eliminate dot
			if (name[0] != '.')
			{
				printLong(name);
				cout << name << endl;
			}
			break;
		case 'm': // flag -a and -l
			printLong(name);
			cout << name << endl;
			break;
		case 'r':	// flag -R
			printRecur(name);
			break;
		default:
			break;
	}
}


int main(int argc, char** argv)
{
	char path[PATH_MAX];
	char flag = '0';

	if (argc == 1)
	{
		strcpy(path, "./");	// current path
		path[2] = '\0';	// add null to the end
		printF('0', path);
	}
	
	else
	{
		for (int i = 1; i < argc; ++i)
		{
			if (argv[i][0] == '-')
			{
				for (unsigned int j = 1; j < strlen(argv[i]); ++j)
				{
					if (argv[i][j] == 'a')
					{
						flag = 'a';
					}
					else if (argv[i][j] == 'l')
					{
						flag = 'l';
					}
					else if (argv[i][j] == 'R')
					{
						flag = 'r';
					}
					else
					{
						cout << "invalid parameter: " << argv[i][j] << endl;
						exit(1);
					}
				}
			}
			else
			{
				
			}
		}

	}
	
	return 0;
}
