#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

commandlist[] = {"cd", "help", "exit", "alias", "kill", "pwd"};
int (*commandtofind[])() =
    {
        &cd, &help, &exit, &alias, &kill, &pwd}
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
    int STATUS;
    do
    {
        // 自定义命令提示符
        char prompt[] = "[FangShell]" + getpwuid((getuid())) + '@' + gethostname() + ':' + getcwd() + (geteuid() == 0 ? '#' : '$' +);
        printf("%s", prompt);

        /*原教程的输入逻辑
        fflush(stdin);
        fgets(cmdin, 256, stdin);*/
        int c;
        while ((c = getchar()) != '\n' && c != EOF)
            ;
        fgets(cmdin, 256, stdin);

        int i;
        for (i = 0; cmd[i] != '\n'; i++)
            ;
        cmd[i] = '\0'; // 去掉读入的换行符

        args[0] = cmd;
        
    } while (STATUS);
}

int main()
{
    char cmdin[256];
    char *args[128];
    void loop();
    return 0;
}