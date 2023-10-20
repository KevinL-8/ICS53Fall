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



int parseline(char *source, char *dest[128])
{
    // printf("into the paresline");
    char * saveptr;
    char * action = strtok_r(source, " \n\t", &saveptr);
    int i = 0;
    while(action != NULL)
    {
        // printf("into the while loop in the parseline");
        dest[i++] = action;
        action = strtok_r(NULL, " \n\t", &saveptr);
    }
    dest[i] = NULL;
    if(strcmp(dest[i-1], "&") == 0){
        return 1;
    }
    else{
        return 0;
    }
}

void quit(){
    exit(0);
}

void waiting4pid(pid_t processID){
    int waitCondition = WUNTRACED | WCONTINUED;
    int currentState;
    pid_t childpid;
    childpid = waitpid(processID, &currentState, waitCondition);
    if(WIFSIGNALED(currentState)){
        printf("\n currentState = Child Exited!\n");
    }
    if(WIFSTOPPED(currentState)){
        printf("\n currentState = Child stopped!\n");
    }
}

void eval(char * instruct) {
    char * argv[128];
    int bg;
    pid_t pid;
    // printf("here");
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
        quit();
    }
    else
    {
        if((pid = fork()) == 0){
            if(execvp(argv[0], argv) < 0){
                if(execv(argv[0], argv) < 0){
                    perror("execv");
                    exit(0);  
                }
            }
        }
        else{
            waiting4pid(pid);
        }
    // }else if(bg == 0){
    //     int status;
    //     if(waitpid(pid, &status, 0) < 0){
    //         printf("%s: Command not found.\n", argv[0]);
    //     }
    }
}

int main()
{
    int proc_id[1000];
    while(1)
    {
        char instruct[128];
        printf("prompt> ");
        fgets(instruct, 128, stdin);
        if(feof(stdin)){
            exit(0);
        }
        if(instruct[0] != '\n'){
            eval(instruct);
        }
    }
    return 0;
}