#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/syscall.h>

int count = 0;
int totalPatternCount = 0;
int lineRead = 0;
int patternCountArray[50];
int arrIndex = 0;
char * PATTERN = NULL;
pthread_mutex_t threadlock;

void * searchFile (void*arg);

int main (int argc, char*argv[]){

/* Error Checking Arguments */

    if(argc < 3){
        perror("NEED MORE FILES");
        exit(1);
    }

    if(argc > 50){
        perror("TOO MANY FILES");
        exit(1);
    }

/* Declarations */

    pthread_mutex_init(&threadlock,NULL);
    count = argc - 2;
    PATTERN = argv[1];
    char *fileName[count];
    int result;
    patternCountArray[count];

    
/* Filename Array */

    for(int i = 0; i < count; i++){
        fileName[i] = (char*) malloc(512);
        fileName[i][0] = '\0';
        strcpy(fileName[i], argv[i+2]);
        
    }

    pthread_t child[count];

/* Creating threads for each argument */

    for(int i = 0; i < count; i++){ 
       result = pthread_create(&child[i], NULL, searchFile, (void*)fileName[i]); /* Threads are made and run search */
       if (result!=0){ 
           perror("THREAD NOT CREATED"); 
           exit(1);
        }
    }    

/* Joining the threads */

    for(int i = 0; i < count; i++){
        result = pthread_join(child[i], NULL); /* Threads stop */
        if (result!=0){ 
           perror("THREAD DIDNT JOIN"); 
           exit(1);
        }
    }

/* Printing thread with value */

    for(int i = 0; i < count; i++){
        pthread_mutex_lock(&threadlock); 
        printf("\n[main] Thread #%ld returned with value: %d\n",child[i], patternCountArray[i]);
        pthread_mutex_unlock(&threadlock);
    }

/* Printing finale */

        pthread_mutex_lock(&threadlock); 
        printf("\n[main] Total of %d matched lines among total of %d lines scanned\n", totalPatternCount, lineRead);
        pthread_mutex_unlock(&threadlock);
    
}

/*Input: fileName Processing: It opens the fileName and counts all occurences of PATTERN in the file. Then printing out where they are and how many matched
Output: Void*/

void * searchFile (void*arg){

/* Declarations and file opening */
    
    char * fileName = (char*)arg;
    char * line = (char*) malloc(1024);
    FILE * file = fopen(fileName, "r");
    int patternCount = 0;
    
    if(!file){
       
        pthread_mutex_lock(&threadlock);
        perror("NO FILE FOUND");
        pthread_mutex_unlock(&threadlock);
        
    } else {

        while((fgets(line, 1024, file))){

             /* Adds to global variable and sets up pointer */
            
            lineRead++;
            char * lPoint = line;
            
            while((lPoint = strstr(lPoint, PATTERN)) != NULL){

            /* Prints where occurences are and adds to global variable */

                pthread_mutex_lock(&threadlock);  
                printf("\n%s : %s", fileName, line);
                totalPatternCount++;
                patternCount++; 
                pthread_mutex_unlock(&threadlock); 
                lPoint++;

            }  
          }

/* Prints and keeps track of in an array of patternCounts. Then free Line */

        pthread_mutex_lock(&threadlock);
        patternCountArray[arrIndex] = patternCount;
        arrIndex++;
        printf("\n--%s has %d matched lines\n", fileName, patternCount);  /* Print and free */
        pthread_mutex_unlock(&threadlock); 
        free(line); 
        fclose(file);  

    }
}