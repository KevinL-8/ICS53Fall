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


void quit()
{
    exit(0);
}

void parseline(char *source, char *dest[128])
{
    char * copy[strlen(source)+1];
    strcpy(*copy, source);
    char * action = strtok(*copy, " \n\t");
    int i = 0;
    while(action != NULL)
    {
        dest[i] = action;
        ++i;
        action = strtok(NULL, " \n\t");
    }
}

int main()
{
    char instruct[128];
    while(1)
    {
        printf("prompt> ");
        char * argv[128];
        fgets(instruct, 128, stdin);
        instruct[strlen(instruct)-1] = '\0';
        printf("this is what i get: %s\n", instruct);
        parseline(instruct, argv);
        if(strcmp(argv[0], "cd") == 0){
            if(chdir(argv[1]) == 0){
                printf("Changed directory to %s\n", argv[1]);
            }else{
                perror("chdir");
            }
        }
        break;
    }
    return 0;
}