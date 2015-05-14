#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <vector>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
using namespace std;

#define MAXSIZE 1000

bool result = false;

struct redir
{
	char *filename;
	int type; // 0.>, 1.>>, 2.<, 3.<<<
	int fd; // STDIN_FILENO: 0; STDOUT_FILENO: 1; STDERR_FILENO: 2
	redir(const char *filename, const int type, const int fd)
	{
		this->filename = new char[strlen(filename)+1];
		strcpy(this->filename, filename);
		this->type = type;
		this->fd = fd;
	}
	~redir()
	{
		delete[] this->filename;
	}
};

void getInput(char str[]);
void format (char command[]);
void execute (char *cmd[]);
void digitCmd (string str);
void parse (char *cmd[], char command[], vector<struct redir*> &files);
//void redirCmd (const vector<struct redir*> &files);

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
	cout << username << "%" << hostname << "$";
}

void getInput (char str[])
{
	if (!fgets(str, MAXSIZE, stdin))
	{
		return;
	}
	int len1 = strlen(str);
	if (str[len1 - 1] == '\n')
	{
		str[len1 - 1] = '\0';
	}
	//strncat(str, "\0", 1);
}
void len1(char command[], int &len, int &i)
{
	len += 2;
	command[len] = '\0';
	for (int j = len-1; j > i+2; --j)
		command[j] = command[j-2];
	command[i+2] = ' ';
	command[i+1] = command[i];
	command[i] = ' ';
	i += 2;
}
void len2(char command[], int &len, int &i)
{
	len += 2;
	command[len] = '\0';
	for (int j = len-1; j > i+3; --j)
		command[j] = command[j-2];
	command[i+3] = ' ';
	command[i+2] = command[i+1];
	command[i+1] = command[i];
	command[i] = ' ';
	i += 3;
}
void len3(char command[], int &len, int &i)
{
	len += 2;
	command[len] = '\0';
	for (int j = len-1; j > i+4; --j)
		command[j] = command[j-2];
	command[i+4] = ' ';
	command[i+3] = command[i+2];
	command[i+2] = command[i+1];
	command[i+1] = command[i];
	command[i] = ' ';
	i += 4;
}
void len4(char command[], int &len, int &i)
{
	len += 2;
	command[len] = '\0';
	for (int j = len-1; j > i+5; --j)
		command[j] = command[j-2];
	command[i+5] = ' ';
	command[i+4] = command[i+3];
	command[i+3] = command[i+2];
	command[i+2] = command[i+1];
	command[i+1] = command[i];
	command[i] = ' ';
	i += 5;
}

// delete the extra spaces
void format (char command[])
{
	//char newcmd[MAXSIZE];
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
	len = strlen(command);
	for (int i = 0; i < len; ++i)
	{
		if (command[i] == '\t')
		{
			command[i] = ' ';
		}
	}
	// 0.., 1.., 2.., <, <<<, >, >>, |
	for (int i = 0; i < len; ++i)
	{
		if (i == 0)
		{
			if (isdigit(command[i]))
			{
				if (command[i+1] == '<'&&command[i+2] != '<')
					len2(command, len, i);
				else if (command[i+1] == '<'&&command[i+2] == '<'&&command[i+3] == '<')
					len4(command, len, i);
				else if (command[i+1] == '>'&&command[i+2] != '>')
					len2(command, len, i);
				else if (command[i+1] == '>'&&command[i+2] == '>')
					len3(command, len, i);
			}
			else if (command[i] == '<'&&command[i+1] != '<')
				len1(command, len, i);
			else if (command[i] == '<'&&command[i+1] == '<'&&command[i+2] == '<')
				len3(command, len, i);
			else if (command[i] == '>'&&command[i+1] != '>')
				len1(command, len, i);
			else if (command[i] == '>'&&command[i+1] == '>')
				len2(command, len, i);
			else if (command[i] == '|'&&command[i+1] != '|')
				len1(command, len, i);
		}
		else
		{
			if (isdigit(command[i])&&command[i-1] == ' ')
			{
				if (command[i+1] == '<'&&command[i+2] != '<')
					len2(command, len, i);
				else if (command[i+1] == '<'&&command[i+2] == '<'&&command[i+3] == '<')
					len4(command, len, i);
				else if (command[i+1] == '>'&&command[i+2] != '>')
					len2(command, len, i);
				else if (command[i+1] == '>'&&command[i+2] == '>')
					len3(command, len, i);
			}
			else if (command[i] == '<'&&command[i+1] != '<')
				len1(command, len, i);
			else if (command[i] == '<'&&command[i+1] == '<'&&command[i+2] == '<')
				len3(command, len, i);
			else if (command[i] == '>'&&command[i+1] != '>')
				len1(command, len, i);
			else if (command[i] == '>'&&command[i+1] == '>')
				len2(command, len, i);
			else if (command[i] == '|'&&command[i+1] != '|')
				len1(command, len, i);
		}
	}
	// delete the extra spaces
	
	for (int i = 0; i < len; ++i)
	{
		if (command[i] == ' ')
		{
			if (command[i+1] == ' ')
			{
				for (int j = i; j < len; ++j)
				{
					command[j] = command[j+1];
				}
				--i;
				--len;
			}
		}
	}
	// delete the spaces at the beginning
	len = strlen(command);
	while (command[0] == ' ')
	{
		for (int i = 0; i < len; ++i)
		{
			command[i] = command[i+1];
		}
	}
}

void execute (char *cmd[])
{
	pid_t pid;
	int status;
	vector<struct redir*> files;
	if ((pid = fork()) < 0)
	{
		perror("ERROR: forking child process failed\n");
		result = false;
		exit(1);
	}
	else if (pid == 0)
	{
		//redirCmd(files);
		if (execvp(*cmd, cmd) < 0)
		{
			perror ("ERROR: exec failed\n");
			result = false;
			exit(1);
		}
	}
	else
	{
		if (-1 == waitpid(pid, &status, 0))
		{
			perror("ERROR: wait error\n");
			result = false;
			exit(1);
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

// whether the symbol is like "0>"
bool digitCmd(string str, int &type)
{
	if (isdigit(str.at(0)))
	{
		str.erase(str.begin());
		if(str == ">"){
			type = 0;
			return true;
		}
		else if(str == ">>"){
			type = 1;
			return true;
		}
		else if(str == "<"){
			type = 2;
			return true;
		}
		else if(str == "<<<"){
			type = 3;
			return true;
		}
	}
	return false;
}

void parse (char *cmd[], char command[], vector<struct redir*> &files)
{
	vector<string> str;
	char * token;
	char *saveptr;
	string temp;
	char * get[MAXSIZE];
	struct redir* temp1;

	token = strtok_r(command, " ", &saveptr);
	int m = 0;
	get[m] = token;
	temp = token;

	while (token != NULL)
	{
		++m;
		temp = token; // convert token to string type
		str.push_back(temp);
		token = strtok_r(NULL, " ", &saveptr);
		get[m] = token;
	}
	//execute(cmd);
	int j = 0;
	int fd;
	int type;
	
	for(unsigned i = 0; i < str.size(); ++i)
	{	
		if(i < str.size() - 1)
		{
			if(str.at(i) == ">") 
			{
				// fd = STDOUT_FILENO;
				temp1 = new struct redir(get[i+1], 0, 1);
				files.push_back(temp1);
				++i;
			}
			else if(str.at(i) == ">>") 
			{
				// fd = STDOUT_FILENO;
				temp1 = new struct redir(get[i+1], 1, 1);
				files.push_back(temp1);
				++i;
			}
			else if(str.at(i) == "<") 
			{
				// fd = STDIN_FILENO;
				temp1 = new struct redir(get[i+1], 2, 0);
				files.push_back(temp1);
				++i;
			}
			else if(str.at(i) == "<<<") 
			{
				//	fd = STDIN_FILENO;
				temp1 = new struct redir(get[i+1], 3, 0);
				files.push_back(temp1);
				++i;
			}
			else if(digitCmd(str.at(i), type))
			{
				fd = str.at(i)[0] - '0';
				temp1 = new struct redir(get[i+1], type, fd);
				files.push_back(temp1);
				++i;
			}
			else
			{
				cmd[j] = get[i];
				++j;
			}
		}
		else if(i == str.size()-1)
		{
			if(str.at(i) == ">" || str.at(i) == ">>" || str.at(i) == "<" 
					|| str.at(i) == "<<<" || digitCmd(str.at(i), type))
			{
				cerr << "ERROR: No input afetr symbol" << endl;
				return;
			}
			else
			{
				cmd[j] = get[i];
			}
		}
	}
	//execute(cmd);
}
/*
void redirCmd (const vector<struct redir*> &files)
{
	if(files.size() == 0)
		return;
	for(unsigned i = 0; i < files.size(); ++i)
	{
		int fd;
		switch(files.at(i)->type)
		{
			case 0:
				fd = open(files.at(i)->filename, O_CREAT|O_RDWR|O_TRUNC, S_IRUSR|S_IWUSR);
				continue;
			case 1:
				continue;
			case 2:
				continue;
			case 3:
				continue;
		}
	}
}
*/
int main (int argc, char **argv)
{
	char *cmd[MAXSIZE];
	vector<struct redir*> files;
	while (1)
	{
		prompt();
		char command[MAXSIZE];
		memset(command, '\0', MAXSIZE);
		//for (int i = 0; i < MAXSIZE; ++i)
		//{
		//	command[i] = '\0';
		//}
		getInput(command);
		format(command);
		if (command[0] == '\0')
			continue;
		if (strncmp(command, "exit", 4) == 0)
		{
			if (command[4] == '\0' || command[4] == ' ')
			{
				cout << "Process ended." << endl;
				exit(0);
			}
		}
		//cout << command;
		parse(cmd, command, files);
		
		for (int i = 0; cmd[i] != '\0'; ++i)
		{
			cout << i  << " " << cmd[i] << endl;
		}
		cout << endl;
		for (int i = 0; i < files.size(); ++i)
		{
			cout << i << " " << files.at(i)->filename 
			<< " " << files.at(i)->type 
			<< " " << files.at(i)->fd << endl;
		}

		int len = files.size();
		for (int i = 0; i < len; ++i)
		{
			files.pop_back();
		}
	}
	return 0;
}
