#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct Pair{
    int first;
    int second;
};


int check_add(int hour, int current_hour[])
{
    for(int i = 0; i < 24; ++i)
    {
        if(abs(hour - current_hour[i]) < 3)
        {
            return 1;
        }
    }
    return 0;
}

int main()
{
    char instruct[128];
    struct Pair plan[24];
    for(int i = 0; i < 24; ++i)
    {
        plan[i].first = -24;
        plan[i].second = 0;
    }
    int current_cal = 0;
    int current_hour[24];
    for(int i = 0; i < 24; ++i)
    {
        current_hour[i] = -24;
    }
    printf("$ ");
    fgets(instruct, 128, stdin);
    while(1){
        if(instruct[0] != '\n')
        {
            if(strcmp(instruct, "quit\n") == 0)
            {
                break;
            }
            else
            {
                char *action = strtok(instruct, " \n");
                // printf("action is: %s\n", action);
                if(strcmp(action, "add") == 0)
                {
                    char *cal = strtok(NULL, " ");
                    char *time = strtok(NULL, " ");
                    // printf("\n the cal is: %s, the time is %s", cal, time);
                    int time_pos = atoi(time);
                    int cal_int = atoi(cal);
                    if(check_add(time_pos, current_hour) == 0)
                    {
                        if(2501 > current_cal + cal_int)
                        {
                            plan[time_pos].first = time_pos;
                            plan[time_pos].second = cal_int;
                            current_hour[time_pos] = time_pos;
                            current_cal += cal_int;
                            // printf("Now the current cal is %d\n", current_cal);
                        }
                        else
                        {
                            printf("Error: Caloric intake exceeds the 2500 calories limit.\n");
                        }
                    }
                    else
                    {
                        printf("Error: Cannot have multiple meals in less than 3 hours.\n");
                    }
                }
                else if(strcmp(action, "replace") == 0)
                {
                    char *cal = strtok(NULL, " ");
                    char *time = strtok(NULL, " ");
                    // printf("the cal is: %s, the time is %s", cal, time);
                    int time_pos = atoi(time);
                    int cal_int = atoi(cal);
                    if(current_hour[time_pos] != -24)
                    {
                        if(2501 > current_cal - plan[time_pos].second + cal_int)
                        {
                            current_cal -= plan[time_pos].second;
                            plan[time_pos].second = cal_int;
                            current_cal += cal_int;
                            // printf("Now the current cal is %d\n", current_cal);
                        }
                        else
                        {
                            printf("Error: Caloric intake exceeds the 2500 calories limit.\n");
                        }
                    }
                    else
                    {
                        printf("Error: Meal does not exist in the record.\n");
                    }
                }
                else if(strcmp(action, "printplan") == 0)
                {
                    for(int i = 0; i < 24; ++i)
                    {
                        if(plan[i].first != -24)
                        {
                            printf("%d %d\n", plan[i].first, plan[i].second);
                        }
                    }
                }
                
            }
        }
        printf("$ ");
        fgets(instruct, 128, stdin);
    }
    return 0;
}
