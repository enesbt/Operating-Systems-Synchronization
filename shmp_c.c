#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include<fcntl.h>
#include <sys/types.h>
#include<pthread.h>
#include <semaphore.h>

#define N 10
pthread_mutex_t lock;

void *ptr;
int pd[2];//pd[0] alma pd[1] yazma


void *task(void *arg)
{    
    char *msg = (char *)arg;
    pthread_mutex_lock(&lock);

    char message[100];
    int i = 0;

    while (*msg != ',' && *msg != '\0') {
        message[i++] = *msg++;
    }

    message[i] = '\0';

    //printf("Message: %s\n", message);
    write(pd[1],message,strlen(message)); //okunan mesajı ebeveyne ilet
    if (*msg == ',') {
        msg++; // Virgülü atla
    }

    pthread_mutex_unlock(&lock); // Kilidi serbest bırak
    pthread_exit(NULL);
}


int main()
{
    sem_t *sem_producer, *sem_consumer;   
    // Semaforları oluştur veya aç
    sem_producer = sem_open("producer_sem", O_CREAT, 0644, 1);
    sem_consumer = sem_open("consumer_sem", O_CREAT, 0644, 0);
    if (sem_producer == SEM_FAILED || sem_consumer == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }
   

    while (1) 
    {
        sem_wait(sem_consumer); // Semaforu kilitle
        
        // Kritik bölge başlangıcı
        printf("Tüketici: Ürün tüketiyorum\n");
        const char *name = "OS";
        const int SIZE = 4096;
        int shm_fd;
        int i;

        /* open the shared memory segment */
        shm_fd = shm_open(name, O_RDONLY, 0666);
        if (shm_fd == -1) {
            printf("shared memory failed\n");
            exit(-1);
        }

        /* now map the shared memory segment in the address space of the process */
        ptr = mmap(0,SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
        if (ptr == MAP_FAILED) {
            printf("Map failed\n");
            exit(-1);
        }
        
        int pid,ret;

        ret = pipe(pd); //tunel olustur

        if(ret<0)
            printf("pipe error");

        pid = fork();
        if(pid==0) //child proces
        {           
            close(pd[0]);
            pthread_t tid[N]; //thread dizisi
            pthread_attr_t attr;
            pthread_attr_init(&attr);
            
        
            char* msg = (char*)ptr;
            for (int i = 0; i < 10; i++) //thread paylasilmis mesaji okur ve tunel ile ebeveyne yollar.
            {
                pthread_create(&(tid[i]), &attr,task, (void*)msg);
                sleep(1);
                pthread_join(tid[i],NULL);
                while (*msg != ',' && *msg != '\0') {
                    msg++;
                }

                if (*msg == ',') {
                    msg++; // Virgülü atla
                }
            }
        }
        else if(pid>0) // parent
        {
            close(pd[1]);
            char buf[10];
            for (int i = 0; i < 10; i++) //tunel ile thredlerden gelen bilgi yazdirilir.
            {
                read(pd[0], buf, sizeof(buf));    
                printf("Okunan == %s \n",buf);
            }
            
            wait(NULL);
        }
        else
        {
            printf("fork hatası");
        }

        /* remove the shared memory segment */
        if (shm_unlink(name) == -1) {
            printf("Error removing %s\n",name);
            exit(-1);
        }

        // Kritik bölge sonu

        sleep(1);
        sem_post(sem_producer);

    }

	
    sem_close(sem_producer);
    sem_close(sem_consumer);

    sem_unlink("sem_producer");
    sem_unlink("sem_consumer");
    sem_destroy(sem_producer);   
    sem_destroy(sem_consumer);
    
	return 0;
}
