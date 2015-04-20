// three flags
// ls, ls -a, ls -l, ls -R, ls -lR, ls -l -R, ls -R -l,
// ls filename filename cmd (cmd can be in any position)
// begin with "#"
// no input: "\n" or " "
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

using namespace std;

#define MAXSIZE 1000
char command[MAXSIZE];
// "result" stores the boolean value that whether the command execute
bool result = false;

// output prompt within each loop
void prompt ()
{
	char hostname[MAXSIZE];
    	char *username;

	bzero(hostname, MAXSIZE);

	if (NULL == (username = getlogin()))
	{
		perror("ERROR: get username failure!");
		exit(-1);
	}
	if (-1 == gethostname(hostname, MAXSIZE))
	{
		perror("ERROR: get hostname failure!");
		exit(-1);
	}

    cout << username << "%" << hostname << ">>"; // prompt
}

void getInput (char str[])
{

	// get input
	if (!fgets(str, MAXSIZE, stdin))
	{
		return;
	}

	int len1 = strlen(str);
	// when there is no input
	if (str[len1 - 1] == '\n')
	{
		str[len1 - 1] = '\0';
	}
}

// format the input with extra '\t' and ' ' to standard form
void format ()
{
	string temp;

	int len = strlen(command);
	
	for (int i = 0; i < len; ++i)
	{
		if (command[i] == '#')
		{
			command[i] = '\0';
			break;
		}
	}

	// replace tabs by spaces
	for (int i = 0; i < len; ++i)
	{
		if (command[i] == '\t')
		{
			command[i] = ' ';
		}
	}	
	
	// add ' ' between characters and ';'. i.e. convert "a;b" to "a, b"
	for (int i = 0; i < len; ++i)
	{
		if (command[i] == ';')
		{
			// add two spaces around ';'
			len += 2;
			for (int j = len - 1; j > i + 2; --j)
			{
				command[j] = command[j - 2];
			}
			command[i + 2] = ' ';
			command[i + 1] = command[i];
			command[i] = ' ';
			i += 2;
		}
	}
	
	// add ' ' between characters and '&&'
	for (int i = 0; i < len - 1; ++i)
	{
		if (command[i] == '&' && command[i+1] == '&')
		{
			len += 2;;
			for (int j = len - 1; j > i + 3; --j)
			{
				command[j] = command[j - 2];
			}
			command[i + 3] = ' ';
			command[i + 2] = command[i + 1];
			command[i + 1] = command[i];
			command[i] = ' ';
			i += 3;
		}
	}

	// add ' ' between characters and '||'
	for (int i = 0; i < len; ++i)
	{
		if (command[i] == '|' && command[i+1] == '|')
		{
			len += 2;;
			for (int j = len - 1; j > i + 3; --j)
			{
				command[j] = command[j - 2];
			}
			command[i + 3] = ' ';
			command[i + 2] = command[i + 1];
			command[i + 1] = command[i];
			command[i] = ' ';
			i += 3;
		}
	}

	//remove all extra " ". i.e. reduce two continuous ' ' to one
	for (int i = 0; i < len; ++i)
	{
		if (command[i] == ' ')
		{
			if (command[i + 1] == ' ')
			{
				for (int j = i; j < len; ++j)
				{
					command[j] = command[j + 1];
				}
				-- i;
				-- len;
			}
		}
	}
	// remove all the spaces at the begining.
	while (command[0] == ' ')
	{
		for (int i = 0; i < len; ++i)
		{
			command[i] = command[i + 1];
		}
	}
}

// execute one command and record the execution result
void execute (char * cmd[])
{
	pid_t pid;
	int status;
	if ((pid = fork()) < 0)
	{
		perror ("ERROR: forking child process failed\n");
		result = false;
		exit(1);
	}
	// child process
	else if (pid == 0)
	{
		if (execvp(*cmd, cmd) < 0)
		{
			perror ("ERROR: exec failed\n");
			result = false;
			exit(1);
		}
	}
	else // parent process
	{
		// result = true;
		if (-1 == waitpid(pid, &status, 0)) // wait for the child process to finish
		{
			perror("ERROR: wait error\n");
			result = false;
			exit (1);
		}
		if (WEXITSTATUS(status) == 0)	
		{
			result = true;
		}
		else if (WEXITSTATUS(status) == 1)
		{
			result = false;
		}
	}
}

// parse the char array into string vector
// state the logic of "&&", "||" and ";"
void parse(char * cmd[])
{
	vector<string> str;
	char * token;
	char * saveptr; // value return parameter used by strtok_r() to record progress
	string temp;
	char * get[MAXSIZE];
	// initialize the flag as false,
	// "false" means we should not jump the next command
	// "true" means we should ignore the next command
	bool jumpflag = false;
	
	token = strtok_r(command, " ", &saveptr);
	int m = 0;
	get[m] = token;
	temp = token;
	
	while(token != NULL)
	{
		++ m;
		temp = token; // convert token to string type
		str.push_back(temp);
		token = strtok_r(NULL, " ", &saveptr);
		get[m] = token;
	}
		
	// distinguish the connectors and commands
	for (unsigned i = 0; i < str.size(); ++i)
	{
		// connectors
		if (str.at(i) == ";")
		{
			jumpflag = false;
		}
		else if (str.at(i) == "&&")
		{
			if (result)
			{
				jumpflag = false;
			}
			else 
			{
				jumpflag = true;
			}
		}
		else if (str.at(i) == "||")
		{
			if (result)
			{
				jumpflag = true;
			}
			else
			{
				jumpflag = false;
			}
		}
		else
		{
			cmd[0] = get[i];
			int j;
			for (i = i + 1, j = 1; i < str.size(); ++i)
			{
				if (str.at(i) == "&&" || str.at(i) == "||" || str.at(i) == ";")
				{
					break;
				}
				else
				{
					cmd[j] = get[i];
					++ j;			
				}
			}
			if (!jumpflag)
			{
				execute(cmd);
			}
			-- i;
			for (int i = 0; i < j; ++i)
			{
				cmd[i] = NULL;
			}
		}
	}
	return;
}


int main(int argc, const char ** argv)
{
	char *cmd[MAXSIZE];
	while (1)
	{
		prompt();
		getInput(command);
		format();
		// solve the segmentation fault
		// when there is no input or the input contains only spaces
		// the current  while loop ends
		// and continues the next loop
		if (command[0] == '\0')
		{
			continue;
		}
		// when the user types "exit", the program exit
		if (strncmp(command, "exit", 4) == 0)
		{
			if (command[4] == '\0' || command[4] == ' ')
			{
				cout << "Process ended." << endl;
				exit(0);
			}
		}

		parse(cmd);
	} 
    return 0;
}
