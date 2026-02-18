#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char *argv[]){
     
    struct page{
        int processId;
        int pageNum;
        int lastAccessed;
    };

    struct io{
        int quant;
        int delay;
    };

    struct process{
        char *name;
        int totalTime;
        int runningTime;
        int requestCount;
        int pageTable[4];
        struct io *requests;
    };

    struct page *RAM[16];
    struct page *VRAM[32];
    for (int x = 0;x<16;x++){
        RAM[x] = NULL;
    }

    FILE *fl;
    fl = fopen(argv[1], "r");
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
        
        for (int k = 0;k<4;k++){
            processes[i].pageTable[k] = 99;
            VRAM[4*i + k] = malloc(sizeof(struct page));
            (*VRAM[4*i +k]).lastAccessed = 0;
            (*VRAM[4*i +k]).processId = i;
            (*VRAM[4*i +k]).pageNum = k;
        }
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
        i ++;
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
    
                        free(processes[i].name);
                        free(processes[i].requests);
                        processes[i].name = NULL;
                        processes[i].requests = NULL;
                        break;
                    }
                    else{
                        cpuTime += proc.requests[x].delay;
                        processes[i].runningTime += proc.requests[x].delay;
                        blocked = 1;
                    
                    int next = 0;
                    int full = 1;
                    for (int k = 0; k<4;k++){
                        int ramPos = proc.pageTable[k];
                        if (ramPos != 99){
                            next = (k+1)%4;
                        }
                        else{
                            full = 0;
                        }
                    }
                    
                    if (full==0){
                        (*VRAM[4 * i + next]).lastAccessed = cpuTime;
                        int ramFull = 1;
                        int k = 0;
                        while (k<16){
                            if (RAM[k] == NULL){
                                ramFull = 0;
                                break;
                            }
                            k++;
                        }
                        if (ramFull == 0){
                            RAM[k] = VRAM[4 * i + next];
                            processes[i].pageTable[next] = k;
                        }
                        else{
                            //local search first
                            int minIndex = 9;
                            int minVal = 0;
                            for (int k =0;k<4;k++){
                                struct page temp = *VRAM[4*i+k];
                                if ((temp.lastAccessed < minVal || minIndex == 9) && k != next && proc.pageTable[k] != 99){
                                    minVal = temp.lastAccessed;
                                    minIndex = k;
                                }
                            }
                            if (minIndex != 9){
                                int prevInd = processes[i].pageTable[minIndex];
                                RAM[prevInd] = VRAM[4*i + next];
                                processes[i].pageTable[minIndex] = 99;
                                processes[i].pageTable[next] = prevInd;
                                break;
                            }
                            //then global search otherwise
                            minIndex = 0;
                            minVal = (*RAM[0]).lastAccessed ;
                            for (int k = 0;k<16;k++){
                                if (RAM[k] == NULL){
                                    continue;
                                }
                                if ((*RAM[k]).lastAccessed < minVal){
                                    minVal = (*RAM[k]).lastAccessed;
                                    minIndex = k;
                                }
                            }
                            struct page prev = *RAM[minIndex];
                            processes[prev.processId].pageTable[prev.pageNum] = 99;
                            RAM[minIndex] = VRAM[4 * i + next];
                            processes[i].pageTable[next] = minIndex;
                        }
                        }   
                }   
                }
            }
            if (blocked == 1){continue;}
            if (diff <= 10){
                cpuTime += diff;
                processes[i].runningTime += diff;
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
    FILE *out;
    out = fopen(argv[2], "w");
    fclose(out);
    out = fopen(argv[2], "a");
    fprintf(out, "Page Table:\n");
    for (int i =0;i<lineNum;i++){
        struct process proc = processes[i];
        fprintf(out, "Process %d: %d %d %d %d\n", i, proc.pageTable[0], proc.pageTable[1], proc.pageTable[2], proc.pageTable[3]);
        
    }
    fprintf(out, "\nRAM:\n");
    for (int i=0;i<16;i++){
        fprintf(out, "Frame %d: %d, %d, %d\n", i, (*RAM[i]).processId, (*RAM[i]).pageNum, (*RAM[i]).lastAccessed);
        free(RAM[i]);
        RAM[i] = NULL;
    }
    fclose(out);
}