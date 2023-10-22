#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pwd.h>
#include <sys/utsname.h>

#define TOK_DELIM " \t\r\n="
// 全局变量
int STATUS = 1;
int fdhistory;

char cmdin[256];
char *tokens[128];
int tokenindex = 0;
int papetokenindex = 0; // pipe以为是pape,发现的时候已经太多了，怕改错就不改了
char *papetokens[8][128];

char *aliaslist[128];
char *aliasnamelist[128];
int aliasindex = 0;

int fdout, fdin;
// int cmdNum, varNum;
// char envVar[MAX_VAR_NUM][MAX_PATH_LENGTH];
//  内建命令的函数指针列表化
char *commandlist[] = {"cd", "help", "myexit", "alias", /*"kill"*/"pwd", "unalias", "export", "unset"};

    char **cd(char **args) // 接受一个字符串地址；这里c指针和数组在参数里怎么用一开始把我弄晕了；应当传入解引用的字符数组
{
    char *path = args[1];
    char **result;
    if (chdir(path) == 0)//chdir和其他要调用的程序都支持相对路径，似乎并不需要对相对路径进行特殊的实现？
    {
        result[0] = path;
        return result;
    }
    else
    {
        perror("Error in directory changing");
        return 0;
    }
}
char **help(char **args)
{
    char **result = NULL;
    printf("INTENTATION SHOULD BE STRICT CONSIDERING THE CODE QUALITY\n");
    printf("THERE ARE USABLE INNER COMMAND:\n");
    int commandNum = sizeof(commandlist) / sizeof(commandlist[0]);
    for (int i = 0; i < commandNum; i++)
    {
        printf("%s\n", commandlist[i]);
    }
    return result;
}
char **myexit(char **args)
{
    char **result = NULL;
    STATUS = 0;
    return result;
}
char **alias(char **args)
{
    char *alias_name = args[1];
    char *alias;
    char *space = " ";
    for (int i = 2; i < sizeof(args) / sizeof(args[0]); i++)
    {
        strcat(alias, args[i]);
        strcat(alias, space);
    }
    char **result = NULL;
    if (alias_name == NULL || alias_name[0] == '\0')
    {
        for (int i = 0; i <= aliasindex; i++)
        {
            printf("%s='%s'\n", aliasnamelist[i], aliaslist[i]);
        }
    }
    else
    {
        aliasnamelist[aliasindex] = alias_name;
        aliaslist[aliasindex] = alias;
        aliasindex++;
    }
    return result;
}
char **unalias(char **args)
{
    char **result = NULL;
    char *alias_name = args[1];
    if (alias_name == NULL || alias_name[0] == '\0')
    {
        perror("No alias input");
        return result;
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
    return result;
}
//char **kill(char **args)
//{
//}
char **pwd(char **args)
{
    char **result = NULL;
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        result[0] = cwd;
        printf("%s\n", cwd);
    }
    else
    {
        perror("Error for search current path");
    }
    return result;
}
char **export(char **args)
{
    char **result = NULL;
    char *varname = args[1];
    char *var;
    char *space = " ";
    for (int i = 2; i < sizeof(args) / sizeof(args[0]); i++)
    {
        strcat(var, args[i]);
        strcat(var, space);
    }
    char *varstring = (char *)malloc(strlen(varname) + strlen(var) + 2); // 加2是为了包含等号和结束符'\0',这里也是全项目唯一用了malloc的地方，因为是对着教程写的
    sprintf(varstring, "%s=%s", varname, var);
    if (putenv(varstring) != 0)
    {
        perror("Error");
        free(varstring);
        return result;
    }
    free(varstring);
    return result;
}
char **unset(char **args)
{
    char **result = NULL;
    char *varname = args[1];
    if (unsetenv(varname) != 0)
    {
        perror("Error");
        return result;
    }
    printf("Unset environment variable: %s\n", varname);
    return result;
}
char **(*commandtofind[])(char **) =
    {
        &cd, &help, &myexit, &alias, /*&kill*/ &pwd, &unalias, &export, &unset};
// 命令读取

void readline()
{
    
    /*原教程的输入逻辑
      fflush(stdin);
      fgets(cmdin, 256, stdin);*/
    //int c;
    //while ((c = getchar()) != '\n' && c != EOF)
        //;
    fgets(cmdin, 256, stdin);
    write(fdhistory, cmdin, sizeof(cmdin));

    int i;
    for (i = 0; cmdin[i] != '\n'; i++)
        ;
    cmdin[i] = '\0'; // 去掉读入的换行符
}
// 参数分割
int split(char *cmdin, char **tokens, bool PAPEORNOT) // 参数从一个加到后面的三个，是在不删之前代码重写的情况下进行的，我有写屎山的潜能
{
    // 要求能以多种符号作为分隔符，多个空格和空格夹住的符号，识别引号包住的字符串,解析管道和文件重定向
    char *token;
    token = strtok(cmdin, TOK_DELIM);
    for (int i = 0; i < aliasindex; i++)
    {
        if (token == aliasnamelist[i])
        {
            split(aliaslist[i], tokens, 1);
        }
    }
    while (token != NULL)
    {
        int checkthealias=0;
        //  参数分割时进行别名的替换
        for (int i = 0; i < aliasindex; i++)
        {
            if (token == aliasnamelist[i])
            {
                 checkthealias =1;
                split(aliaslist[i], tokens, 1);
            }
        }
        if(checkthealias ==1)
        {
            checkthealias =0;
            break;
        }
        tokens[tokenindex] = token;
        tokenindex++; // wc，我甚至混用全局变量和函数参数来控制执行，函数可以复用，但是不完全可以复用
        token = strtok(cmdin, TOK_DELIM);
    }
    tokens[tokenindex] = NULL;
    // 判断记录是否有管道重定向并分割
    if (PAPEORNOT)
    {
        for (int i = 0; i < tokenindex; i++)
        {
            int index = 0;
            if (tokens[i] == "|")
            {
                papetokenindex++;
            }
            else
                papetokens[papetokenindex][index] = tokens[i];
            index++;
        }
    }
}
// 一般执行判断
char *exectue(char **args) // 这里就要返回管道的程序的返回值
{
    char *result = NULL;
    // 基本输入判断
    if (args[0] == NULL || args[0][0] == '\0')
    {
        perror("incorrect args input in exectue period");
    }

    // 如果是内置命令，函数数组中寻找命令
    int commandNum = sizeof(commandlist) / sizeof(commandlist[0]);
    for (int j = 0; j < commandNum; j++)
    {
        if (strcmp(commandlist[j], args[0]) == 0)
        {
            char **temp=(*commandtofind[j])(args);
            result =temp[0];
            return result;
             // 为了这个当时定下来的内建函数列表把所有内建函数全部改成了接受char**返回char**的鬼样子
        }
    }
    // 如果是外置程序，进行调用
    pid_t wpid;
    int status;
    int pipefd[2];
    pid_t pid = fork();
    if (pipe(pipefd) == -1) {
        perror("Error in create pipe");
        return NULL;
    }
    if (pid == -1)
    {
        perror("Error in fork the child process");
        return result;
    }
    else if (pid == 0) // 如果是子进程
    {
        // 进行管道输出
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        if (execvp(args[0], args) == -1)
            perror("Error in execvp the program");
    }
    else  // 如果是父进程
    {
        close(pipefd[1]);
        int byteNum=read(pipefd[0], result, sizeof(result));
        if(byteNum<0)
        {
            perror("Cant read from the pipe pipefd[1]");
        }
        do
        {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    // 读取管道输入并进行返回
    return result;
}
// 重定向执行判断 其中调用exectue
int rediectloop()
{
    // 判断是否有文件重定向并处理
    int FILEOUTA = 0, FILEOUTC = 0, FILEIN = 0;
    if (papetokens[0][1] == ">")
    {
        FILEOUTC = 1;
    }
    if (papetokens[0][1] == ">>")
    {
        FILEOUTA = 1;
    }
    if (papetokens[papetokenindex][1] == "<")
    {
        FILEIN = 1;
    }
    for (int i = papetokenindex; i >= 0; i--)
    {
        if (FILEOUTC)
        {
            fdout = open(papetokens[0][2], O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IXUSR);
            FILEOUTC = 0;
            if (fdout == -1)
            {
                perror("Error in open the redirect file");
                return 0;
            }
            if (dup2(fdout, STDOUT_FILENO) == -1)
            {
                perror("Error redirect to stdout");
                return 0;
            }
        }
        if (FILEOUTA)
        {
            fdout = open(papetokens[0][2], O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IXUSR);
            FILEOUTA = 0;
            if (fdout == -1)
            {
                perror("Error in open the redirect file");
                return 0;
            }
            if (dup2(fdout, STDOUT_FILENO) == -1)
            {
                perror("Error redirect to stdout");
                return 0;
            }
        }
        if (FILEIN)
        {
            char *filecontent;
            FILEIN = 0;
            fdin = open(papetokens[papetokenindex][2], O_RDWR);
            if (fdin == -1)
            {
                perror("Error in open the redirect file");
                return 0;
            }
            if (dup2(fdin, STDIN_FILENO) == -1)
            {
                perror("Error redirect to stdin");
                return 0;
            }
            scanf("%s", filecontent);
            tokenindex = 2;
            split(filecontent, papetokens[papetokenindex], 0);
        }
    }
    // 执行有管道部分
    while (papetokenindex > 0)
    {
        if (papetokens[papetokenindex][0] == NULL || papetokens[papetokenindex][0][0] == '\0')
        {
            return 0;
        }
        char *rediectback;
        // 不是在这里进行管道读入而是接受exectue的返回
        tokenindex = sizeof(papetokens[papetokenindex]) / sizeof(papetokens[papetokenindex][0]);
        split(rediectback, papetokens[papetokenindex], 0);
        rediectback = exectue(papetokens[papetokenindex]);
    }
    // 执行最后无管道的部分
    exectue(papetokens[0]);
}


// 程序循环
void loop()
{ 
    do
    {
        // 自定义命令提示符
        char *prompt = "[FangShell]";
        char temp[128];
        struct passwd *temppasswd=getpwuid((getuid()));
        strcat(prompt,temppasswd->pw_name);
        strcat(prompt,"@");
        gethostname(temp,64);
        strcat(prompt,temp);
        strcat(prompt,":");
        strcpy(temp,getcwd(NULL,0));//这里的NULL后参数应该可以是任意值？
        strcat(prompt,temp);
        //警告替换 temp= (geteuid() == 0 ? '#' : '$' );
        if(geteuid() == 0)
        {
            strcpy(temp,"#");
        }
        else strcpy(temp,"$");
        strcat(prompt,temp);
        printf("%s", prompt);

        // 读取命令执行
        readline();
        split(cmdin, tokens, 1);
        rediectloop();
        // 清空全部全局变量，关闭文件描述符
        if (fdin != 0)
        {
            close(fdin);
        }
        if (fdout != 0)
        {
            close(fdout);
        }
        fdin = 0;
        fdout = 0;
        tokenindex = 0;
        memset(cmdin,0,sizeof(cmdin));
        for (int i = 0; i < 128; i++)
        {
            memset(tokens[i],0,sizeof(tokens[i]));
        }
    } while (STATUS);
}

int main()
{
    printf("check");
    char filename[64] = "home/";
    struct passwd *temppasswd1=getpwuid((getuid()));
    strcat(filename,temppasswd1->pw_name);
    strcat(filename,"/.FangShellhistory.txt");
    // 打开命令记录文件
    fdhistory = open(filename, O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IXUSR);
    void loop();
    close(fdhistory);
    return 0;
}