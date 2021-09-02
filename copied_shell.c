#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

void interpreter()
{
    char input[256];
    int i;
    char dir[PATH_MAX + 1];
    char *argv[256];
    int argc = 0;
    char *token;
    
    if (getcwd(dir, PATH_MAX + 1) == NULL)
    {
        //error occured
        exit(0);
    }
    
    
    printf("[shell:%s]$ ", dir);
    
    // input
    fgets(input, 256, stdin);
    if (strlen(input) == 0)
    {
        exit(0);
    }
    
    // remove last endline
    input[strlen(input) - 1] = 0;
    
    
    if (strcmp(input, "") == 0)
    {
        return;
    }
    
    //special case
    if(strcmp(input, "exit") == 0){
        printf("Exiting Shell...")
        exit(0);
    }
    
    // divide into tokens
    token = strtok(input, " ");
    while (token && argc < 255)
    {
        argv[argc++] = token;
        token = strtok(NULL, " ");
    }
    argv[argc] = 0;
    
    // fork
    pid_t forknum = fork();
    if (forknum != 0)
    {
        // parent
        int status;
        
        // wait for child to finish
        waitpid(forknum, &status, WUNTRACED);
    }
    else
    {
        // signals
        signal(SIGINT, SIG_DFL);
        signal(SIGTERM, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        
        // environment
        setenv("PATH", "/bin:/usr/bin:.", 1);
        
        // execute
        execvp(argv[0], argv);
        
        // code is here it means there was an error in the commands
        if (errno == ENOENT)
        {
            printf("%s: command not found\n", argv[0]);
        }
        else
        {
            printf("%s: unknown error\n", argv[0]);
        }
        
        // end the child process
        exit(0);
    }
}

int main()
{
    // signal ignore.
    signal(SIGINT, SIG_IGN);
    signal(SIGTERM, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    
    
    while (1)
    {
        interpreter();
    }
}
