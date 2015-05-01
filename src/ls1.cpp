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
		cout << " " << setw(2) << s.st_nlink << " "
			<< getpwuid(s.st_uid)->pw_name << " "
			<< getgrgid(s.st_gid)->gr_name << " "
			<< setw(5) << s.st_size << " ";
		strftime(time, sizeof(time), "%b %d %H:%M", localtime(&s.st_mtime));
		cout << time << " ";
	}
	else
	{
		perror("There was an error with stat. ");
		exit(1);
	}
}

void printRecur(char *dirname)
{
	DIR *dirp;
	struct dirent *entry;
	char *temp;
	if (NULL == (dirp = opendir(dirname)))
	{
		perror("There was an error with opendir(). ");
		exit(1);
	}
	//const char *d_name;
	errno = 0;
	while (NULL != (entry = readdir(dirp)))
	{
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
			continue;
		if (entry->d_type & DT_DIR)
		{
			cout << entry->d_name << "####:" << endl;
			
			strcpy(temp, dirname);
			strcat(temp, "/");
			strcat(temp, entry->d_name);
			printRecur(temp);
		}
		else
			cout << entry->d_name << endl;
	}
	if (-1 == closedir(dirp))
	{
		perror("There was an error with closedir(). ");
		exit(1);
	}
}

void printF(char *dirname)
{
	struct stat buf;
	char name[NAME_MAX+1];

	// get filename from dirname
	int j = 0;
	for (int i = 0; i < strlen(dirname); ++i)
	{
		if (dirname[i] == '/')
		{
			j = 0;
			continue;
		}
		name[j] = dirname[i];
		++j;
	}
	name[j] = '\0';


}



// preprocess the filenames
void process(struct stat s, char *dirname, char name[][NAME_MAX+1])
{
	DIR *dirp;	// the directory
	
	int count = 0;	// record the file num
	// at most 256 files
	char filenames[256][PATH_MAX+1];
	char nodot[256][PATH_MAX+1];
	char temp[PATH_MAX+1];
	
	int total = 0;

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
	if(-1 == closedir(dirp))
	{
		perror("There was an error with closedir(). ");
		exit(1);
	}
	if (count > 256)
	{
		perror("Too many files in this dir");
		exit(1);
	}
	
	int len = strlen(dirname);
	// get all the filename in the dir
	//dirp = opendir(dirname);
	if(NULL == (dirp = opendir(dirname)))
	{
		perror("There was an error with opendir(). ");
		exit(1);
	}

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
		for (int j = 0; j < count - 1 - i; ++j)
		{
			if (strcmp(filenames[j], filenames[j+1]) > 0)
			{
				// assign temp with a[j+1]
				
				strcpy(temp, filenames[j+1]);
				temp[strlen(filenames[j+1])] = '\0';
				// assign a[j] with a[j+1]
				strcpy(filenames[j+1], filenames[j]);
				filenames[j+1][strlen(filenames[j])] = '\0';
				// assign a[j] with temp
				strcpy(filenames[j], temp);
				filenames[j][strlen(temp)] = '\0';
			}
		}
	}

	//char name[256][NAME_MAX+1];
	int start = 0;
	for (int i = 0; i < count; ++i)
	{
		for (int j = 0; j < strlen(filenames[i]); ++j)
		{
			if (filenames[i][j] == '/')
			{
				start = 0;
				continue;
			}
			name[i][start] = filenames[i][j];
			++start;
		}
		name[i][start] = '\0';
	}
	// print total for -l
	int i = 0;
	if (flagL)
	{
		if (flagA)
		{
			total = 0;
			for (i = 0; i < count; ++i)
			{
				if (-1 == stat(filenames[i], &s))
				{
					perror("There was an error with stat. ");
					exit(1);
				}
				total += s.st_blocks;
			}
			cout << "total " << total/2 << endl;
			for (i = 0; i < count; ++i)
			{
				//printF(filenames[i]);
			}
		}
		else
		{	
			total = 0;
			char nodot[256][NAME_MAX+1];
			int j = 0;
			int count2 = 0;
			for (i = 0; i < count; ++i)
			{
				if (-1 == stat(filenames[i], &s))
				{
					perror("There was an error with stat1. ");
					exit(0);
				}
				if (name[i][0] != '.')
				{
					strcpy(nodot[j], filenames[i]);
					++j;
					++ count2;
					//total += s.st_blocks;
				}
			}
			for (i = 0; i < count2; ++i)
			{
				if (-1 == stat(nodot[i], &s))
				{
					perror("There was an error with stat2. ");
					exit(1);
				}
				total += s.st_blocks;
			}
			cout << "total " << total/2 << endl;
			for (i = 0; i < count2; ++i)
			{
				printF(nodot[i]);
		
			}
		}
	}
	else
	{
		for (i = 0; i < count - 1; ++i)
		{
			printF(filenames[i]);
		}
		printF(filenames[i]);
		cout << endl;
	}
	closedir(dirp);
}

int main(int argc, char** argv)
{
	char path[PATH_MAX+1];
	int num = 0;	// record the num of "-"
	struct stat buf;
	char name[256][NAME_MAX+1];

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
				if (argv[i][j] == 'l')
				{
					flagL = true;
				}
				if (argv[i][j] == 'R')
				{
					flagR = true;
				}
				else if (argv[i][j] != 'a' && argv[i][j] != 'l' && argv[i][j] != 'R')
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
		process(buf, path, name);
		return 0;
	}
	
	// if contains filename
	else
	{

		int k = 1;
		while (k  < argc)
		{
			if (argv[k][0] == '-')
			{
				++k;
				continue;
			}
			else
			{
				strcpy(path, argv[k]);
				// tell whether it's a filename
				if (stat(path, &buf) == -1)
				{
					perror("There was an error with stat. ");
					exit(1);
				}

				// if the getting path is a dir
				if (S_ISDIR(buf.st_mode))
				{
					// process the input filenames
					if (path[strlen(argv[k]) - 1] != '/')
					{
						path[strlen(argv[k])] = '/';
						path[strlen(argv[k]) + 1] = '\0';
					}
					else
					{
						path[strlen(argv[k])] = '\0';
					}
					process(buf, path, name);
					++k;
				}
				// if the getting path is not a dir
				else
				{
					process(buf, path, name);
					++k;
				}
			}
		}
	}
	return 0;
}
