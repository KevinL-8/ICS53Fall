#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include <signal.h>
#include <fcntl.h>



void parseline(char *source, char *dest[128])
{
    char * saveptr;
    char * action = strtok_r(source, " \n\t", &saveptr);
    int i = 0;
    while(action != NULL)
    {
        dest[i] = action;
        ++i;
        action = strtok_r(NULL, " \n\t", &saveptr);
    }
}

void eval(char * instruct) {
    char * argv[128];
    instruct[strlen(instruct)] = '\0';
    parseline(instruct, argv);
    if(strcmp(argv[0], "cd") == 0){
        if(chdir(argv[1]) == 0){
            printf("Changed directory to %s\n", argv[1]);
        }else{
            perror("chdir");
        }
    }
    else if(strcmp(argv[0], "pwd") == 0){
        char buf[1024];
        if(getcwd(buf, sizeof(buf)) != NULL){
            printf("%s\n", buf);
        }else{
            perror("getcwd error");
        }
    }
    else if(strcmp(argv[0], "quit") == 0){
        exit(0);
    }
}

int main()
{
    char instruct[128];
    while(1)
    {
        printf("prompt> ");
        fgets(instruct, 128, stdin);
        if(feof(stdin)){
            exit(0);
        }

        eval(instruct);
    }
    return 0;
}