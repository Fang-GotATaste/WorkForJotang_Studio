#include <sys/types.h>
#include <unistd.h>


string commandlist[] = {"cd", "help", "exit", "alias", "kill", "pwd"};
int (*commandtofind[])()=
{
    &cd,&help,&exit,&alias,&kill,&pwd
}
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
void loop()
{   
    
}


int main()
{
    void loop();
    return 0;
}