#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(void){
    struct process{
        char name[10];
        int totalTime;
        int runningTime;
        int requests[10];
        int requestCount;
    };
    struct process processes[10];
    char tempLine[101];
    FILE *fl;
    fl = fopen("input.txt", "r");
    int n = 0;
    while (fgets(tempLine, 50, fl)){ 
        int count = sscanf(tempLine, "%s %d %d %d %d %d %d %d %d %d %d %d", processes[n].name, &processes[n].totalTime, 
        processes[n].requests, processes[n].requests + 1, processes[n].requests + 2, processes[n].requests + 3, processes[n].requests + 4,
        processes[n].requests + 5, processes[n].requests + 6, processes[n].requests + 7, processes[n].requests + 8, processes[n].requests + 9);
        processes[n].requestCount = count - 2;
        processes[n].runningTime = 0;
        n += 1;
    }
    
    fclose(fl);
    int cpuTime = 0;
    int quanta = 1;
    int allFinished = 0;
    while (allFinished != 1){
        allFinished = 1;
        for (int i = 0; i < n;i++){
            int blocked = 0;
            struct process proc = processes[i];
            if (proc.totalTime == proc.runningTime){continue;}
            else{allFinished = 0;}
            for (int x = 0; x < proc.requestCount;x++){
                if (proc.requests[x] == quanta){
                    cpuTime += 5;
                    processes[i].runningTime += 5;
                    blocked = 1;
                    if (proc.totalTime == processes[i].runningTime){
                        printf("%s completed at time %d\n", proc.name, cpuTime);
                    }
                    break;
                }
            }
            if (blocked == 1){continue;}
            if (processes[i].runningTime > proc.totalTime -10){
                cpuTime += proc.totalTime - processes[i].runningTime;
                processes[i].runningTime = proc.totalTime;
                printf("%s completed at time %d\n", proc.name, cpuTime);
                continue;
            }
            processes[i].runningTime += 10;
            cpuTime += 10;
            if (proc.totalTime == processes[i].runningTime){
                printf("%s completed at time %d\n", proc.name, cpuTime);
            }
        }
        quanta += 1;

            
    }


}