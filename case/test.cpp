#include <iostream>
#include <string.h>

using namespace std;

int main()
{
	char cmd[20][20];
	memset(cmd, '\0', 20);
	for (int i = 0; i < 1000; ++i)
	{
		cout << cmd[i] << endl;
	}
	strcpy(cmd[0], "hello");
	strcpy(cmd[1], "thank you");
	cout << cmd[0] << endl;
	return 0;
}
		
