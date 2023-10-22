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

pid_t pid;
pid_t frpid;
int job_index;
struct Job{
    int job_id;
    pid_t pid;
    int status;
    char cmline[128];
    int bg;
    int finished;
};
struct Job jobs[1000];


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

pid_t waiting4pid(pid_t processID){
    // printf("Child pid: %d", processID);
    int waitCondition = WUNTRACED | WCONTINUED;
    int currentState;
    pid_t childpid;
    for(int i = 0; i < job_index; ++i){
        if(jobs[i].pid == processID && jobs[i].finished == -1){
            printf("Here is the returned child pid in wait function: %d\n", processID);
            childpid = waitpid(processID, &currentState, waitCondition);
            jobs[i].status = 0;
            jobs[i].finished += 1;
        }
    }
    if(WIFEXITED(currentState)){
        printf("\n currentState = child exited normally!\n");
    }
    if(WIFSIGNALED(currentState)){
        printf("\n currentState = Child exited with signal!\n");
    }
    if(WIFSTOPPED(currentState)){
        printf("\n currentState = Child stopped!\n");
    }
    return childpid;
}

void quit(){
    exit(0);
}

void sigint_handler(int sig_num){
    for(int i = 0; i < job_index; ++i){
        if(jobs[i].pid == pid && jobs[i].bg == 0 && jobs[i].status == 1){
            printf("Killing process\n");
            kill(pid, SIGINT);
            jobs[i].status = 0;
            jobs[i].finished += 1;
        }
    }
    // if(pid > 0 && frgpid == getpgid(pid)){
    //     printf("The front group id was: %d, and we get: %d from pid: %d", frgpid, getpgid(pid), pid); 
    //     kill(pid, SIGINT);
    //     for(int i = 0; i < job_index; ++i){
    //         if(jobs[i].pid == pid){
    //             jobs[i].status = 0;
    //         }
    //     }
    // }
}

void sigchld_handler(int sig_num){
    if(frpid != pid){
        int currentState;
        pid_t childpid;
        childpid = waitpid(-1, &currentState, WNOHANG);
        printf("Here is the returned child pid in sigchld handler: %d", childpid);
        if(childpid > 0){
            for(int i = 0; i < job_index; ++i){
                if(jobs[i].pid == childpid){
                    jobs[i].status = 0;
                    jobs[i].finished += 1;
                }
            }
            if(WIFEXITED(currentState)){
                printf("\n currentState = child exited normally!\n");
            }
            if(WIFSIGNALED(currentState)){
                printf("\n currentState = Child exited with signal!\n");
            }
            if(WIFSTOPPED(currentState)){
                printf("\n currentState = Child stopped!\n");
            }
        }
    }
    // if(frgpid != getpgid(pid)){
    //     printf("The front group id was: %d, and we get: %d from pid: %d", frgpid, getpgid(pid), pid);
    //     waiting4pid(pid);
    //     for(int i = 0; i < job_index; ++i){
    //     if(jobs[i].pid == pid){
    //         jobs[i].status = 0;
    //     }
    // }
    // }
}

void sigtstp_handler(int sig_num){
    for(int i = 0; i < job_index; ++i){
        if(jobs[i].pid == pid && jobs[i].bg == 0 && jobs[i].status == 1){
            printf("Suspended process\n");
            kill(pid, SIGTSTP);
            jobs[i].status -= 1;
        }
    }
}

void eval(char * instruct){
    char  * argv[128];
    int bg;
    // instruct[strlen(instruct)] = '\0';
    bg = parseline(instruct, argv);
    // printf("Here is the instruct received: %s\n", instruct);
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
    else if(strcmp(argv[0], "jobs") == 0){
        for(int i = 0; i < job_index-1; ++i){
            if(jobs[i].finished < 1){
                if(jobs[i].status == 1){
                    printf("[%d] (%d) Running %s\n", jobs[i].job_id, jobs[i].pid, jobs[i].cmline);
                }
                else if(jobs[i].status == 0){
                    printf("[%d] (%d) Stopped %s\n", jobs[i].job_id, jobs[i].pid, jobs[i].cmline); 
                }
            }
        }
    }
    else if(bg == 0)
    {
        if((frpid = pid = fork()) == 0){
            if(execvp(argv[0], argv) < 0){
                if(execv(argv[0], argv) < 0){
                    perror("execv");
                    exit(0);  
                }
            }
        }
        else{
            // frgpid = getpgid(pid);
            // printf("The front gpid is set as: %d\n", frgpid);
            struct Job * job = &jobs[job_index - 1];
            job->job_id = job_index;
            job->pid = pid;
            job->status = 1;
            strcpy(job->cmline, instruct);
            job->bg = 0;
            job->finished -= 1;
            job_index += 1;
            waiting4pid(pid);
        }
    }
    else{
        if((pid = fork()) == 0){
            if(setpgid(pid, pid) < 0){
                perror("setgid");
                exit(0);
            }
            if(execvp(argv[0], argv) < 0){
                if(execv(argv[0], argv) < 0){
                    perror("execv");
                    exit(0);  
                }
            }
        }
        else{
            // printf("The background job group is: %d", getpgid(pid));
            struct Job * job = &jobs[job_index - 1];
            job->job_id = job_index;
            job->pid = pid;
            job->status = 1;
            strcpy(job->cmline, instruct);
            job->bg = 1;
            job->finished -= 1;
            job_index += 1;
        }
    }
}

int main()
{
    pid = 0;
    job_index = 1;
    frpid = 0;
    while(1)
    {
        signal(SIGINT, sigint_handler);
        signal(SIGCHLD, sigchld_handler);
        signal(SIGTSTP, sigtstp_handler);
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