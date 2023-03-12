#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>
//#include "a2_helper.h"
#include "a2_helper.h"

pthread_mutex_t lock;
pthread_mutex_t lockSem;
pthread_cond_t cond63;
pthread_cond_t cond62;
pthread_cond_t cond55;
pthread_cond_t cond61;
pthread_cond_t cond;
pthread_cond_t cond1;

int th63_start=0;
int th62_end=0;

int th55_end=0;
int th61_end=0;
int th14_start=0;
int th14end=0;
int nr_threads_p6=0;
int nr_threads_p6_2=0;
int total_threads_p6=0;

sem_t *sem51;
sem_t *sem55;


void* thread_function61(void* arg)
{


    // pthread_mutex_lock(&lock);

    // while (!th55_end)
    //         pthread_cond_wait(&cond55, &lock);
                                                
    // pthread_mutex_unlock(&lock);
   
    sem_wait(sem55);

    info(BEGIN, 6, 1);

   



    info(END,6,1);



    return NULL;

}

void* thread_function62(void* arg)
{
    
    pthread_mutex_lock(&lock);

    while (!th63_start)
            pthread_cond_wait(&cond63, &lock);
                                                
    pthread_mutex_unlock(&lock);

    info(BEGIN, 6, 2);
   
   
    info(END, 6, 2);

    return NULL;

}

void* thread_function63(void* arg)
{

    info(BEGIN, 6, 3);


    pthread_mutex_lock(&lock);

    th63_start=1;
   
    pthread_cond_signal(&cond63);

    while (!th62_end)
            pthread_cond_wait(&cond62, &lock);
    
    pthread_mutex_unlock(&lock);


    info(END,6,3);



    return NULL;

}

void* thread_function64(void* arg)
{

    info(BEGIN, 6, 4);
    info(END,6,4);



    return NULL;

}

void* thread_function_p3(void* arg)
{

    int thread_nr= *((int*) arg);

   
    
    pthread_mutex_lock(&lock);
    while (!(nr_threads_p6<4&&(thread_nr==14||th14_start==1)))      
            pthread_cond_wait(&cond, &lock);
    nr_threads_p6++;   
    total_threads_p6++;  
    printf("         ");                                
    pthread_mutex_unlock(&lock);

    info(BEGIN, 3, thread_nr);   
    nr_threads_p6_2++;
    pthread_cond_signal(&cond1);
    
    if(thread_nr==14){
        th14_start=1;
        pthread_cond_broadcast(&cond);
    }
 

  
    pthread_mutex_lock(&lock);
    while(!((nr_threads_p6_2==4&&thread_nr==14)||th14end==1))
            pthread_cond_wait(&cond1, &lock);   
    //printf("%d ending while %d running\n",thread_nr,nr_threads_p6);   
    printf("                     ");
    th14end=1;
    info(END,3,thread_nr);  
    nr_threads_p6--;  
    nr_threads_p6_2--;                 
    pthread_mutex_unlock(&lock);
    pthread_cond_signal(&cond); 
   

    // if(thread_nr==14){
    //     pthread_mutex_lock(&lock);
    //     while (nr_threads_p6!=4)
    //             pthread_cond_wait(&cond2, &lock);
    //     pthread_mutex_unlock(&lock);
    // }
    
    // pthread_mutex_lock(&lock);

    // pthread_mutex_unlock(&lock);
     

    



    return NULL;

}

void* thread_function_p5(void* arg)
{
    int thread_nr= *((int*) arg);
   // printf("\n%d\n", thread_nr);
   
    if(thread_nr==1){
           
           sem_wait(sem51);
                                                
    
    }
    

    info(BEGIN, 5, thread_nr);


    info(END,5,thread_nr);



    return NULL;

}



int main(){

    init();

    info(BEGIN, 1, 0);

    pid_t p2 = fork();

    sem51=sem_open("p5_semaphore",O_CREAT,0644,0);
    sem_close(sem51);

    sem55=sem_open("p6_semaphore",O_CREAT,0644,0);
    sem_close(sem55);



    switch (p2) {
    case -1:
        // error case
        perror("Cannot create a new child");
        exit(1);
    case 0:
        info(BEGIN, 2, 0);


        pid_t p3 = fork();
            switch (p3) {
            case -1:
                // error case
                perror("Cannot create a new child");
                exit(1);
            case 0:
                //printf("p4 : child pid = %d     parent pid = %d\n",getpid(),getppid());
                info(BEGIN, 3, 0);

                pid_t p4 = fork();
                    switch (p4) {
                    case -1:
                        // error case
                        perror("Cannot create a new child");
                        exit(1);
                    case 0:

                        info(BEGIN, 4, 0);

                        pid_t p5 = fork();
                            switch (p5) {
                            case -1:
                                // error case
                                perror("Cannot create a new child");
                                exit(1);
                            case 0:
                                info(BEGIN,5,0);

                                sem51=sem_open("p5_semaphore",O_RDWR);
                                sem55=sem_open("p6_semaphore",O_RDWR);
                                
                               
                                    pid_t p6 = fork();
                                        switch (p6) {
                                        case -1:
                                            // error case
                                            perror("Cannot create a new child");
                                            exit(1);
                                        case 0:

                                            info(BEGIN,6,0);
                                             pthread_t threads[4];
                                                sem51=sem_open("p5_semaphore",O_RDWR);
                                                sem55=sem_open("p6_semaphore",O_RDWR);
                                                
                                                pthread_cond_init(&cond62,NULL);
                                                pthread_cond_init(&cond63,NULL);
                                                pthread_mutex_init(&lock,NULL);

                                                if (pthread_create(&threads[0], NULL,thread_function61,(void*)1) != 0) {
                                                    perror("Cannot create threads");
                                                    exit(1);
                                                }

                                                if (pthread_create(&threads[1], NULL,thread_function62,(void*)2) != 0) {
                                                    perror("Cannot create threads");
                                                    exit(1);
                                                }
                                                
                                                
                                                if (pthread_create(&threads[2], NULL,thread_function63,(void*)3) != 0) {
                                                    perror("Cannot create threads");
                                                    exit(1);
                                                }
                                               

                                                if (pthread_create(&threads[3], NULL,thread_function64,(void*)4) != 0) {
                                                    perror("Cannot create threads");
                                                    exit(1);
                                                }


                                               
                                                // pthread_join(threads[1], NULL);
                                                // th62_end=1;
                                                // pthread_cond_signal(&cond62);

                                                for (int i=0; i<4; i++){
                                                    

                                                   
                                                    
                                                    pthread_join(threads[i], NULL);
                                                    if(i==0){
                                                      
                                                        //th61_end=1;
                                                      
                                                      // printf("\n th61 end is %d ", th61_end);
                                                        //pthread_cond_signal(&cond61);
                                                        sem_post(sem51);

                                                    }
                                                     if(i==1){
                                                        th62_end=1;
                                                        pthread_cond_signal(&cond62);
                                                    }
                                                    
                                                }

                                                pthread_mutex_destroy(&lock);

                                             pid_t p8 = fork();
                                                switch (p8) {
                                                case -1:
                                                    // error case
                                                    perror("Cannot create a new child");
                                                    exit(1);
                                                case 0:
                                                    info(BEGIN,8,0);
                                                    info(END,8,0);
                                                    exit(0);
                                                    break;
                                                // default:
                                                //     waitpid(p8,NULL, 0);
                                                   

                                                }


                                               



                                            waitpid(p8,NULL, 0);
                                            sem_close(sem51);
                                            sem_close(sem55);
                                            info(END,6,0);
                                            exit(0);
                                            
                                            break;
                                        // default:
                                        //     waitpid(p6,NULL, 0);
                                            //wait(NULL);
                                            //exit(0);
                                        }

                                 pthread_t threads[5];
                                int threadArgs[5];

                                
        

                               pthread_mutex_init(&lock,NULL);
                                pthread_cond_init(&cond55,NULL);
                                pthread_cond_init(&cond61,NULL);
                               
                                for(int i=0;i<5;i++){
                                    threadArgs[i]=i+1;
                                    if (pthread_create(&threads[i], NULL,thread_function_p5,&threadArgs[i]) != 0) {
                                        perror("Cannot create threads");
                                        exit(1);
                                    }

                                }
                                // pthread_mutex_lock(&lock);
                                //     if(th61_end!=0)
                                //         pthread_cond_signal(&cond61);
                                // pthread_mutex_unlock(&lock);

                                for (int i=1; i<5; i++){
                                    pthread_join(threads[i], NULL);
                                    if(i==4){
                                       sem_post(sem55);
                                       
                                    }
                                    
                                }
                                pthread_join(threads[0],NULL);
                             pthread_mutex_destroy(&lock);

                                
                                waitpid(p6,NULL, 0);
                                sem_close(sem51);
                                sem_close(sem55);
                                info(END,5,0);
                                exit(0);

                            //     break;
                            // default:
                            //     waitpid(p5,NULL, 0);
       
                                
        }
        
                        waitpid(p5,NULL, 0);
                        info(END, 4, 0);
                        exit(0);
                        break;
                    // default:
                    //     waitpid(p4,NULL, 0);
                        


        }


                pthread_t threads[35];
                int threadArgs[35];

                pthread_mutex_init(&lock,NULL);
                pthread_cond_init(&cond,NULL);
                pthread_cond_init(&cond1,NULL);
                for(int i=0;i<35;i++){
                    threadArgs[i]=i+1;
                    if (pthread_create(&threads[i], NULL,thread_function_p3,&threadArgs[i]) != 0) {
                        perror("Cannot create threads");
                        exit(1);
                    }

                }

                for (int i=0; i<35; i++){
                    
                    pthread_join(threads[i], NULL);

                }

                pthread_mutex_destroy(&lock);
                
                waitpid(p4,NULL, 0);
                info(END, 3, 0);
                exit(0);
                break;
            
               



            }

        pid_t p7 = fork();
            switch (p7) {
            case -1:
                // error case
                perror("Cannot create a new child");
                exit(1);
            case 0:
                info(BEGIN,7,0);
                info(END,7,0);
                exit(0);
                break;
            //default:
               
                

            }

        waitpid(p3,NULL, 0);
        waitpid(p7,NULL, 0);

        info(END, 2, 0);
        exit(0);
    
        

    }



    pid_t p9 = fork();
    switch (p9) {
    case -1:
        // error case
        perror("Cannot create a new child");
        exit(1);
    case 0:
        info(BEGIN,9,0);
        info(END,9,0);

        exit(0);
    
        

    }
    waitpid(p2,NULL, 0);
    waitpid(p9,NULL, 0);
    sem_unlink("p5_semaphore");
    sem_unlink("p6_semaphore");
    info(END, 1, 0);
    return 0;
}
