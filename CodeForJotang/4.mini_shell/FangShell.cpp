#include <iostream>
#include <string>
#include <sys/types.h>
#include <unistd.h>
using namespace std; // for nearly all objects we use are in STL

#define DELIM " \t\r\n" // the condition of spilting the string

// commands bulit inside the shell
string commandlist[] = {"cd", "help", "exit", "alias", "kill", "pwd"};
int (*commandtofind[])()
{
}
int cd()
{
}
int help()
{
}
int exit()
{
}
int alias()
{
}
int kill()
{
}
int pwd()
{
}

// read and parse
string aline;
string command;
string tokens[25];
void stringsplit(string str, string delim, string (&targetstr)[])
{
    int num = 0;
    istringstream ins(str);
    string token;
    while (getline(ins, token, ' '))
    {
        targetstr[num].push_back(token);
        num++;
    }
    // should be release after execute
}

// execute
int execute(string command, string args[])
{
}
int launch(string args[])
{
}

void loop() //  basic lifetime
{
    int STATUS; // save the condition of loop in a variable
    do          // keep(?) the prompt while active
    {
        string prompt = "[FangShell]"+getpwuid((getuid()))+'@'+gethostname()+':'+getcwd() +(geteuid()==0?'#':'$'+);
        cout << prompt < < < < endl;
    } while (STATUS);
    stringsplit(aline, DELIM, tokens);
    execute(tokens[0], tokens);
}

int main()
{
    // initialize
    loop();
    // quit
    return 0;
}