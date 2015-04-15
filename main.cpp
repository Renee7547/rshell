#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/mman.h>

using namespace std;

int main(int argc, const char ** argv) {
    
    char host[100];
    char *user;
    //char *save;
    
    while (1)
    {
        user = getlogin(); // get user name
        gethostname(host, 100); // get host name
        cout << *user << "@" << host << "$"; // prompt
        
        
        
    }
    
    
    return 0;
}
