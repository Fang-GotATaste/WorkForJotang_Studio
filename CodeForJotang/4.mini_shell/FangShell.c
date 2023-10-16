#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#define TOK_DELIM " \t\r\n="
// 全局变量
char cmdin[256];
char *tokens[128];
int tokenindex = 0;

char *aliaslist[128];
char *aliasnamelist[128];
int aliasindex = 0;
// int cmdNum, varNum;
// char envVar[MAX_VAR_NUM][MAX_PATH_LENGTH];
//  内建命令的函数指针列表化
char *commandlist[] = {"cd", "help", "exit", "alias", "kill", "pwd", "unalias", "export", "unset"};
int (*commandtofind[])() =
    {
        &cd, &help, &exit, &alias, &kill, &pwd, &unalias, &export, &unset}

int cd(char *path) // 接受一个字符串地址；这里c指针和数组在参数里怎么用一开始把我弄晕了
{
    if (chdir(path) == 0)
    {
        return 1;
    }
    else
    {
        perror("Error in directory changing");
        return 0;
    }
}
int help()
{
    printf("THERE ARE USABLE INNER COMMAND:\n");
    int commandNum = sizeof(commandlist) / sizeof(commandlist[0]);
    for (int i = 0; i < commandNum; i++)
    {
        printf("%s\n", commandlist[i]);
    }
}
int exit()
{
    STATUS = 0;
}
int alias(char *alias_name, char *alias)
{
    if (alias_name == NULL || alias_name[0] == '\0')
    {
        for (int i = 0; i <= aliasindex; i++)
        {
            printf("%s='%s'\n", aliasnamelist, aliaslist);
        }
    }
    else
    {
        aliasnamelist[aliasindex] = alias_name;
        aliaslist[aliasindex] = alias;
        aliasindex++;
    }
}
int unalias(char *alias_name)
{
    if (alias_name == NULL || alias_name[0] == '\0')
    {
        perror("No alias input");
        return 0;
    }
    else
    {
        for (int i = 0; i < aliasindex; i++)
        {
            if (alias_name == aliasnamelist[i])
            {
                for (int j = i; j < aliasindex - 1; j++)
                {
                    aliaslist[j] = aliaslist[j + 1];
                    aliasnamelist[j] = aliasnamelist[j + 1];
                }
                aliasindex--;
            }
        }
    }
    return 1;
}
int kill()
{
}
int pwd()
{
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        printf("%s\n", cwd);
    }
    else
    {
        perror("Error for search current path");
    }
}
int export(char *varname, char *var)
{
    char *varstring = (char *)malloc(strlen(varname) + strlen(var) + 2); // 加2是为了包含等号和结束符'\0'
    sprintf(varstring, "%s=%s", varname, var);
    if (putenv(varstring) != 0)
    {
        perror("Error");
        free(varstring);
        return 0;
    }
    free(varstring);
    return 1;
}
int unset(char *varname)
{
    if (unsetenv(varname) != 0)
    {
        perror("Error");
        return 0;
    }
    printf("Unset environment variable: %s\n", varname);
    return 1;
}

// 命令读取
int read()
{
    /*原教程的输入逻辑
      fflush(stdin);
      fgets(cmdin, 256, stdin);*/
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
    fgets(cmdin, 256, stdin);
    int i;
    for (i = 0; cmdin[i] != '\n'; i++)
        ;
    cmdin[i] = '\0'; // 去掉读入的换行符
}
// 参数分割
int split(char *cmdin)
{
    // 要求能以多种符号作为分隔符，多个空格和空格夹住的符号，识别引号包住的字符串,解析管道符和重定向分支
    char *token;
    token = strtok(cmdin, TOK_DELIM);
    for (int i = 0; i < aliasindex; i++)
    {
        if (token == aliasnamelist[i])
        {
            split(aliaslist[i]);
        }
    }
    while (token != NUll)
    {
        //  参数分割时进行别名的替换
        for (int i = 0; i < aliasindex; i++)
        {
            if (token == aliasnamelist[i])
            {
                split(aliaslist[i]);
                break;
            }
        }
        else tokens[tokenindex] = token;
        tokenindex++;
        token = strtok(cmdin, TOK_DELIM);
    }
    token[tokenindex] = NULL;
}
// 执行判断
int exectue(char **args)
{
    //基本输入判断
    if (args[0] == NULL || args[0][0] == '\0')
    {
        perror("incorrect args input in exectue period");
    }
    //先判断是否有重定向
    for(int i=0;i<tokenindex;i++)
    {
        
    }
    // 如果是内置命令，函数数组中寻找命令
    int commandNum = sizeof(commandlist) / sizeof(commandlist[0]);
    for (int i = 0; i < commandNum; i++)
    {
        if ()
    }
    // 如果是外置命令，进行调用
    else
    {
    }
}
// 程序循环
void loop()
{
    int *command;
    int STATUS = 1;
    do
    {
        // 自定义命令提示符
        char prompt[] = "[FangShell]" + getpwuid((getuid())) + '@' + gethostname() + ':' + getcwd() + (geteuid() == 0 ? '#' : '$' +);
        printf("%s", prompt);

        // 读取命令
        read();
        //

        //?  args[0] = cmdin;  ?
        split();
        exectue();
        tokenindex=0;
        strcpy(cmdin,NULL);
        for(int i=0;i<128;i++)
        {
            strcpy(token[i],NULL);
        }
    } while (STATUS);
}

int main()
{
    void loop();
    return 0;
}