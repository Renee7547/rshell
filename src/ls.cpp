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

unsigned int g_leave_len = MAXROWLEN; // the length left, for aligning
unsigned int g_maxlen; // store the maximum len of filename

bool flagA = false;
bool flagL = false;
bool flagR = false;

using namespace std;

// print out one filename
// make the filenames align in column
void printSingle(char *dirname)
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
	// output the extra spaces
	for (int i = 0; i < len; ++i)
	{
		cout << " ";
	}
	cout << " ";
	// leave two spaces
	g_leave_len -= g_maxlen + 2;
}

void printLong(struct stat s, char *dirname)
{
	//struct stat s;
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
		strftime(time, sizeof(time), "%b %d %H:%M", localtime(&s.st_mtime));
		cout << time << " ";
	}
	else
	{
		perror("There was an error with stat. ");
	}
}

void printRecur(char *dirname)
{

}

void printF(char *dirname)
{
	unsigned int i;
	int j;
	struct stat buf;
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
	
	if (!(flagL||flagA||flagR))
	{
		if (name[0] != '.')
		{
			printSingle(name);
		}
	}

	if (flagL)
	{
		
	}

	if (flagA)
	{

	}
	
	if (flagR)
	{

	}

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
				printLong(buf, name);
				cout << name << endl;
			}
			break;
		case 'm': // flag -a and -l
			printLong(buf, name);
			cout << name << endl;
			break;
		case 'r':	// flag -R
			printRecur(name);
			break;
		default:
	
}

// preprocess the filenames
void file_sort(char flag, char *dirname)
{
	DIR *dirp;	// the directory
	
	int count = 0;	// record the file num
	// at most 256 files
	char filenames[256][PATH_MAX+1];
	char temp[PATH_MAX+1];

	// get the longest filename
	if(NULL == (dirp = opendir(dirname)))
	{
		perror("There was an error with opendir(). ");
		exit(1);
	}
	struct dirent *filespecs; // each entry
	errno = 0;
	while(NULL != (filespecs = readdir(dirp)))
	{
		// set g_maxlen as the longest filename
		if (g_maxlen < strlen(filespecs->d_name))
			g_maxlen = strlen(filespecs->d_name);
		++count;
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

	if (count > 256)
	{
		perror("Too many files in this dir");
	}
	
	int len = strlen(dirname);
	// get all the filename in the dir
	dirp = opendir(dirname);
	for (int i = 0; i < count; ++i)
	{
		filespecs = readdir(dirp);
		strncpy(filenames[i], dirname, len);
		// add null to the end of filenames[i]
		filenames[i][len] = '\0';
		strcat(filenames[i], filespecs->d_name);
		filenames[i][len + strlen(filespecs->d_name)] = '\0';
	}

	// sort the filenames, use bubble-sort
	for (int i = 0; i < count - 1; ++i)
	{
		for (int j = 0; j < count - 1; ++j)
		{
			if (strcmp(filenames[j], filenames[j+1]) > 0)
			{
				strcpy(temp, filenames[j + 1]);
				temp[strlen(filenames[j + 1])] = '\0';
				strcpy(filenames[j + 1], filenames[j]);
				filenames[j + 1][strlen(filenames[j])] = '\0';
				strcpy(filenames[j], temp);
				filenames[j][strlen(temp)] = '\0';
			}
		}
	}
	for (int i = 0; i < count; ++i)
	{
		printF(flag, filenames[i]);
	}	
	closedir(dirp);
}


int main(int argc, char** argv)
{
	char path[PATH_MAX+1];
	int num = 0;	// record the num of "-"
	struct stat buf;

	for (int i = 1; i < argc; ++i)
	{
		if (argv[i][0] == '-')
		{
			++ num;
			for (unsigned int j = 1; j < strlen(argv[i]); ++j)
			{
				if (argv[i][j] == 'a')
				{
					flagA = true;
				}
				else if (argv[i][j] == 'l')
				{
					flagL = true;
				}
				else if (argv[i][j] == 'R')
				{
					flagR = true;
				}
				else
				{
					cout << "invalid parameter: " << argv[i][j] << endl;
					exit(1);
				}
			}
		}
	}
	// if there is no filename, then print out the info of curr dir
	if ((num + 1) == argc)
	{
		strcpy(path, "./");	// current folder
		path[2] = '\0';
		fileSort(path);
		return 0;
	}
	
	// if contains filename
	else
	{
		int k = 1;
		do
		{
			if (argv[k][0] == '-')
			{
				++k;
				continue;
			}
			else
			{
				strcpy(path, argv[k]);
				if (stat(path, &buf) == -1)
				{
					perror("There was an error with stat. ");
				}
				if (S_ISDIR(buf.st_mode))
				{
					if (path[strlen(argv[k]) - 1] != '/')
					{
						path[strlen(argv[k])] = '/';
						path[strlen(argv[k])+1] = '\0';
					}
					else
					{
						path[strlen(argv[k])] = '\0';
					}
					fileSort(flag, path);
				}
				else
				{
					file_sort(flag, path);
					++k;
				}
			}
		}	while(k < argc);
	}
	return 0;
}
