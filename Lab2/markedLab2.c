#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(void){
    struct io{
        int quant;
        int delay;
    };
    struct process{
        char *name;
        int totalTime;
        int runningTime;
        struct io *requests;
        int requestCount;
    };

    FILE *fl;
    fl = fopen("input", "r");
    struct process *processes = malloc(sizeof(struct process));

    int i = 0;
    while (!feof(fl)){
        char c = fgetc(fl);
        int argPos = 0;
        int namePos = 0;
        int x = 0;
        char *totTime = malloc(sizeof(char));
        processes = realloc(processes, (i+1)*sizeof(struct process));
        processes[i].name = malloc(sizeof(char));
        processes[i].runningTime = 0;
        processes[i].requests = malloc(sizeof(struct io));
        while (c != '\n' && !feof(fl)){
            if (c == ' '){
                argPos += 1;
                c = fgetc(fl);
                continue;
            }
            switch (argPos){
            case 0:
                while (c != ' ' && c != '\n' && !feof(fl) && c != '\r'){
                    namePos += 1;
                    processes[i].name = realloc(processes[i].name, (namePos)*sizeof(char));
                    processes[i].name[namePos-1] = c;
                    c = fgetc(fl);
                }
                processes[i].name = realloc(processes[i].name, (namePos+1)*sizeof(char));
                processes[i].name[namePos] = '\0';
                break;
            case 1:
                x = 0;
                while (c != ' ' && !feof(fl) && c != '\n'){
                    totTime[x] = c;
                    x += 1;
                    totTime = realloc(totTime, (x+1)*sizeof(char));
                    c = fgetc(fl);
                }
                totTime = realloc(totTime, (x+2)*sizeof(char));
                totTime[x] = '\0';
                processes[i].totalTime = atoi(totTime);
                free(totTime);
                totTime = NULL;
                break;
            default:
                processes[i].requests = realloc(processes[i].requests, (argPos-1)*sizeof(struct io));
                char *num = malloc(sizeof(char));
                x = 0;
                while (c != ' ' && !feof(fl) && c != '\n'){
                    num[x] = c;
                    x += 1;
                    num = realloc(num, (x+1)*sizeof(char));
                    c = fgetc(fl);
                }
                num = realloc(num, (x+2)*sizeof(char));
                num[x] = '\0';
                if (argPos%2 == 0){
                    processes[i].requests[(argPos-2)/2].quant = atoi(num);
                }
                else{
                    processes[i].requests[(argPos-2)/2].delay = atoi(num);
                }
                free(num);
                num = NULL;
            }
        }
        processes[i].requestCount = (argPos -1)/2;
        i += 1;
    }   
    
    fclose(fl);
    int lineNum = i;
    int cpuTime = 0;
    int quanta = 1;
    int allFinished = 0;

    while (allFinished != 1){
        allFinished = 1;
        for (int i =0;i<lineNum;i++){
            int blocked = 0;
            struct process proc = processes[i];
            int diff = proc.totalTime - proc.runningTime;
            if (diff == 0){continue;}
            else {allFinished = 0;}
            for (int x = 0;x<proc.requestCount;x++){
                if (proc.requests[x].quant == quanta){
                    if (diff <= proc.requests[x].delay){
                        cpuTime += diff;
                        processes[i].runningTime += diff;
                        printf("%s completed at time %d\n", proc.name, cpuTime);
                        free(processes[i].name);
                        free(processes[i].requests);
                        processes[i].name = NULL;
                        processes[i].requests = NULL;
                    }
                    else{
                        cpuTime += proc.requests[x].delay;
                        processes[i].runningTime += proc.requests[x].delay;
                        blocked = 1;
                    }
                    break;  
                }
            }
            if (blocked == 1){continue;}
            if (diff <= 10){
                cpuTime += diff;
                processes[i].runningTime += diff;
                printf("%s completed at time %d\n", proc.name, cpuTime);
                free(processes[i].name);
                free(processes[i].requests);
                processes[i].name = NULL;
                processes[i].requests = NULL;
                continue;
            }
            else{
                cpuTime += 10;
                processes[i].runningTime += 10;  
            }
        
        }
        quanta++;
    }

}