#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <vector>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>

using namespace std;

#define MAXSIZE 1000

bool result = false;
const int PIPE_READ = 0;
const int PIPE_WRITE = 1;

//pid_t lastChild = 0;
vector<pid_t> pid;
vector<pid_t> pid_stp;
//bool childExists = false;
struct sigaction act;

// use the structure to record the filename, the type of the redirection before it,
// and the file descriptor
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

// get input
void getInput(char str[]);
// format the input by adding spaces beside symbols
void format (char command[], int &flagP);
// execute the single command, 
// when there is pipe symbols, recursively execute the commands
void execute (char command[], int save_stdin = -1);
// tell whether the redirection symbol has digit num before
void digitCmd (string str);
// parse the formated input into commands
void parse (char *cmd[], char command[], vector<struct redir*> &files, int &flag);
// determine what to do with different types of redirection
void redirCmd (const vector<struct redir*> &files);

void prompt ()
{
	char hostname[MAXSIZE];
	char *username;
	char path[MAXSIZE];

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
	if (NULL == getcwd(path, 1024))
	{
		cerr << "ERROR: getcwd(). " << endl;
		exit(1);
	}
	char *home;
	if (NULL == (home = getenv("HOME")))
	{
		cerr << "ERROR: getenv(). " << endl;
		exit(1);
	}
	string pathS = path;
	// avoid the case : cd /
	if (0 == pathS.find(home))
		pathS.replace(pathS.find(home), strlen(home), "~");
	cout << username << "%" << hostname << ":" << pathS << "$";
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
void format (char command[], int &flagP)
{
	flagP = 0;
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
		if (command[i] == '\'' || command[i] == '\"')
		{
			for (int j = i; j < len; ++j)
			{
				command[j] = command[j+1];
			}
			--len;
			--i;
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
	// 0, 1, 2.., <, <<<, >, >>, |
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
			else if (command[i] == '|')
				len1(command, len, i);
	//		else if (command[i] == '\"' || command[i] == '\'')
	//			len1(command, len, i);
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
			else if (command[i] == '|')
				len1(command, len, i);
	//		else if (command[i] == '\"' || command[i] == '\'')
	//			len1(command, len, i);
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
	// delete the spaces at the end
	len = strlen(command);
	while (command[len-1] == ' ')
	{
		command[len-1] = '\0';
		-- len;
	}

	for (int i = 0; command[i+2] != '\0'; ++i)
	{
		if(command[i] == '|' && command[i+2] == '|')
		{
			flagP = 1;
		}
	}
}

void cd (char *cmd[], int &flagCD)
{
	flagCD = 0;
	if (strcmp(cmd[0], "cd") == 0)
	{
		flagCD = 1;
		char *home;
		char newDir[MAXSIZE];
		char currDir[MAXSIZE];

		if (NULL == getcwd(currDir, 1024))
		{
			perror("ERROR: getcwd(). ");
			exit(1);
		}
		if (-1 == setenv("PWD", currDir, 1))
		{
			perror("ERROR: setenv(). ");
			exit(1);
		}
		/*
		if (NULL == (currDir = getenv("PWD")))
			cerr << "ERROR: getenv(). " << endl;
			*/
		// HOME
		if (cmd[1] == NULL || strcmp(cmd[1], "~") == 0)
		{
			if (NULL == (home = getenv("HOME")))
				cerr << "ERROR: getenv." << endl;
			strcpy(newDir, home);
		}
		// OLDPWD
		else if (strcmp(cmd[1], "-") == 0)
		{
		//	if (NULL == (oldDir = getenv("OLDPWD")))
		//		cerr << "ERROR: getenv(). " << endl;
			strcpy(newDir, getenv("OLDPWD"));
			cout  << newDir << endl;
		}
		// PATH
		else
		{
			//strcpy(newDir, currDir);
			//strcat(newDir, "/");
			//strcat(newDir, cmd[1]);
			strcpy(newDir, cmd[1]);
		}
		if (-1 == chdir(newDir))
		{
			perror("ERROR: chdir(). ");
			return;
		}
		if(-1 == setenv("OLDPWD", currDir, 1))
		{
			cerr << "ERROR: setenv(). " << endl;
			exit(1);
		}
		if (-1 == setenv("PWD", newDir, 1))
		{
			cerr << "ERROR: setenv(). " << endl;
			exit(1);
		}
	}
}

void execute (char command[], int save_stdin)
{
	char *cmd[MAXSIZE] = {0};
	char *lhs, *rhs;
	unsigned i = 0;
	int flag = 0;
	int flagCD = 0;
	//int flagFG = 0;

	for (i = 0; i < strlen(command); ++i)
	{
		if (command[i] == '|' && i != 0 && i != strlen(command)-1)
		{
			lhs = command; 
			command[i] = '\0'; // split the command line
			rhs = command + i + 1;

			int fd[2];
			if(-1 == pipe(fd))
			{
				perror("ERROR: pipe(). ");
				exit(1);
			}
			pid_t pid;
			if(-1 == (pid = fork()))
			{
				perror("ERROR: fork(). ");
				exit(1);
			}
			else if(pid == 0) // child process
			{
				if(-1 == dup2(fd[PIPE_WRITE], 1))
				{
					perror("ERROR: dup2(). ");
					exit(1);
				}	
				if(-1 == close(fd[PIPE_READ]))
				{
					perror("ERROR: close(). ");
					exit(1);
				}
				execute(lhs, save_stdin);
				exit(0); // prevent zombie process
			}
			else // parent
			{
				// only restore the stdin in the first command
				if (save_stdin == -1)
				{
					if(-1 == (save_stdin = dup(0)))
					{
						perror("ERROR: dup(). ");
						exit(1);
					}
				}	
				//close the write end of the pipe in the parent
				if(-1 == close(fd[PIPE_WRITE]))
				{
					perror("ERROR: close(). ");
					exit(1);
				}
				// make stdin the read end of the pipe
				if(-1 == dup2(fd[PIPE_READ], 0))
				{
					perror("ERROR: dup2(). ");
					exit(1);
				}
				execute(rhs, save_stdin);
				if(-1 == dup2(save_stdin, 0)) // restore stdin
				{
					perror("ERROR: dup2(). ");
					exit(1);
				}
				// close PIPE_READ after each fork
				// otherwise the child process cannot exit(*important)
				// when a large file comes into the pipe
				if(-1 == close(fd[PIPE_READ]))
				{
					perror("ERROR: close(). ");
					exit(1);
				}
				// wait for all the cmd work
				if(-1 == wait(0))
				{
					perror("ERROR: wait(). ");
					exit(1);
				}
				return;
			}
		}
	}
	vector<struct redir*> files;
	parse(cmd, command, files, flag);
	// cd
	cd(cmd, flagCD);
	if (flagCD == 1)
		return;

	// fg
	if (0 == strcmp(cmd[0], "fg"))
	{
		int status;
		int wreturn;
		if (!pid_stp.empty())
		{
			if (-1 == kill(pid_stp.back(), SIGCONT))
			{
				perror("ERROR: kill(). ");
				exit(1);
			}
			// !!!IMPORTANT!!!
			pid.push_back(pid_stp.back());
			do
			{
				wreturn = waitpid(pid_stp.back(), &status, WUNTRACED);
			} while(-1 == wreturn && errno == EINTR);
			if (-1 == wreturn)
			{
				perror("ERROR: wait(). ");
				exit(1);
			}
			pid.pop_back();
			pid_stp.pop_back();
		}
		else 
			cerr << "No suspended jobs." << endl;
		return;
	}

	// bg
	if (0 == strcmp(cmd[0], "bg"))
	{	
		if (!pid_stp.empty())
		{
			if (-1 == kill(pid_stp.back(), SIGCONT))
			{
				perror("ERROR: kill(). ");
				exit(1);
			}
			//pid_stp.pop_back();
		}
		else
			cerr << "No suspended jobs." << endl;
		return;
	}

	if(flag == 1)
	{
		cerr << "Syntax error." << endl;
		return;
	}

//	childExists = true;	
	pid_t wpid;
	//int status;
	if ((wpid = fork()) < 0)
	{
		perror("ERROR: forking child process failed\n");
		result = false;
		exit(1);
	}
	else if (wpid == 0)
	{
		// ignore: don't kill the suspended process
		act.sa_handler = SIG_IGN;
		if (-1 == sigaction(SIGINT, &act, NULL))
		{
			perror("ERROR: sigaction(). ");
			exit(1);
		}
	//	act.sa_handler = SIG_IGN;
		
		if (-1 == sigaction(SIGTSTP, &act, NULL))
		{
			perror("ERROR: sigaction(). ");
			exit(1);
		}
		redirCmd(files);
		if (execvp(*cmd, cmd) < 0)
		{
			perror ("ERROR: exec failed\n");
			result = false;
			exit(1);
		}
	}
	else
	{
		pid.push_back(wpid);
		int wreturn;
		int status;
		do 
		{
			//wait(&status);
			wreturn = waitpid(pid.back(), &status, WUNTRACED);
		} while (wreturn == -1 && errno == EINTR);
		if (wreturn == -1)
		{
			perror("ERROR: wait(). ");
			exit(1);
		}
		pid.pop_back();
		if (WEXITSTATUS(status) == 0)
		{
			result = true;
		}
		else if (WEXITSTATUS(status) == 1)
		{
			result = false;
		}
	}
	//lastChild = pid;
	return;
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

void parse (char *cmd[], char command[], vector<struct redir*> &files, int &flag)
{
	flag = 0;
	vector<string> str;
	char * token;
	char *saveptr;
	string temp;
	char * get[MAXSIZE];
	struct redir* temp1 = 0;

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

	int j = 0;
	int fd;
	int type;
	for (unsigned i = 0; i < str.size()-1; ++i)
	{
		if((str.at(i)=="<"||str.at(i)=="<<<"||str.at(i)==">"||str.at(i)==">>"||digitCmd(str.at(i), type))
				&& (str.at(i+1)=="<"||str.at(i+1)=="<<<"||str.at(i+1)==">"||str.at(i+1)==">>"||digitCmd(str.at(i+1), type)))
		{
		//	cerr << "syntax error. " << endl;
		//	exit(1);
			flag = 1;
		}
	}

	for(unsigned i = 0; i < str.size(); ++i)
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
	/***** test ******
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
	*/	
//	execute(cmd);
}

void redirCmd (const vector<struct redir*> &files)
{
	if(files.size() == 0)
		return;
	for(unsigned i = 0; i < files.size(); ++i)
	{
		int fd;
		int fd1[2];
		int len1 = strlen(files.at(i)->filename);
		//int len2 = len1+1;
		char temp[MAXSIZE];
		switch(files.at(i)->type)
		{
			case 0: // >
				fd = open(files.at(i)->filename, O_CREAT|O_RDWR|O_TRUNC, S_IRUSR|S_IWUSR);
				if(-1 == fd)
				{
					perror("ERROR: open(). 0");
					exit(1);
				}
				if(-1 == dup2(fd, files.at(i)->fd))
				{
					perror("ERROR: dup2(). 0");
					exit(1);
				}
				break;
			case 1: // >>
				fd = open(files.at(i)->filename, O_CREAT|O_RDWR|O_APPEND, S_IRUSR|S_IWUSR);
				if(-1 == fd)
				{
					perror("ERROR: open(). 1");
					exit(1);
				}
				if(-1 == dup2(fd, files.at(i)->fd))
				{
					perror("ERROR: dup2(). 1");
					exit(1);
				}
				break;
			case 2: // <
				fd = open(files.at(i)->filename, O_RDONLY);
				if(-1 == fd)
				{
					perror("ERROR: open(). 2");
					exit(1);
				}
				if(-1 == dup2(fd, files.at(i)->fd))
				{
					perror("ERROR: dup2(). 2");
				}
				break;
			case 3: // <<<
				strcpy(temp, files.at(i)->filename);
				strcat(temp, "\n");
				if(-1 == pipe(fd1))
				{
					perror("ERROR: pipe(). ");
					exit(1);
				}
				if(-1 == write(fd1[PIPE_WRITE], temp, len1+1))
				{
					perror("ERROR: write(). ");
					exit(1);
				}
				if(-1 == dup2(fd1[PIPE_READ], 0))
				{
					perror("ERROR: dup2(). ");
					exit(1);
				}
				if(-1 == close(fd1[PIPE_WRITE]))
				{
					perror("ERROR: close(). ");
					exit(1);
				}
				break;
		}
	}
}

static void hdl(int sig, siginfo_t *siginfo, void *context)
{
	if (sig == SIGINT)
	{
		if (!pid.empty())
		{
			if (-1 == kill(pid.back(), SIGKILL))
			{
				perror("ERROR: kill(). ");
				exit(1);
			}
		}
		cout << endl;
	}
	else if (sig == SIGTSTP)
	{
		if (!pid.empty())
		{
			if (-1 == kill(pid.back(), SIGSTOP))
			{
				perror("ERROR: kill(). ");
				exit(1);
			}
			pid_stp.push_back(pid.back());
			/*
			int status;
			wait(&status);
			if (-1 == status)
			{
				perror("ERROR: wait(). ");
				exit(1);
			}
			*/
		}
		cout << endl;
	}
}

int main (int argc, char **argv)
{

	//char *cmd[MAXSIZE];
	vector<struct redir*> files;
	int flagP = 0;
	memset(&act, '\0', sizeof(act));
	act.sa_sigaction = &hdl;
	act.sa_flags = SA_SIGINFO;

	if (sigaction(SIGINT, &act, NULL) < 0)
	{
		perror("ERROR: sigaction(). ");
		exit(1);
	}
	if (sigaction(SIGTSTP, &act, NULL) < 0)
	{
		perror("ERROR: sigaction(). ");
		exit(1);
	}

	//int flag;
	while (1)
	{
		prompt();
		char command[MAXSIZE];
		memset(command, '\0', MAXSIZE);

		getInput(command);
		format(command, flagP);
		
		if(flagP == 1)
		{
			cerr << "ERROR: syntax error." << endl;
			continue;
		}

		if (command[strlen(command)-1] == '>' || command[strlen(command)-1] == '<')
		{
			cerr << "No input afetr redirection. " << endl;
			continue;
		}
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
		
		execute(command);
		//childExists = false;
		/***** test ******
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
		*/
		// delete all the elements in cmd[] and files
		//memset(cmd, '\0', MAXSIZE);
		int len = files.size();
		for (int i = 0; i < len; ++i)
		{
			if (files.at(i) != 0) {
				delete files.at(i);
				files.at(i) = 0;
			}
		}
		files.clear();
	}
	return 0;
}
