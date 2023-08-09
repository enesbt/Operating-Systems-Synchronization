#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include<pthread.h>
#include <semaphore.h>

#define N 10

int s = 0; // Paylaşılan değişken
pthread_mutex_t lock;
void *ptr;


void *task(void *arg)
{
    char *a = (char*)arg;
    pthread_mutex_lock(&lock); // Kilidi al
    printf("paylasilan degisken  = %d\n", s);
    s+=1;
    ptr += sprintf(ptr,"%s",a);
    ptr += sprintf(ptr,"%s",",");
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

        printf("Üretici: Ürün üretiyorum\n");
        sem_wait(sem_producer); // Tüketici veriyi işlediğinde üreticiye izin ver

        // Ürün üretme işlemleri

        // Kritik bölge başlangıcı
        const int SIZE = 4096;
        const char *name = "OS";
        int pid,ret;
        int pd[2];  //pd[0] alma ucu pd[1] yazma ucu

        ret = pipe(pd); //tunel olustur

        if(ret<0)
            printf("pipe error");

        int shm_fd;

        /* create the shared memory segment */
        shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);

        /* configure the size of the shared memory segment */
        ftruncate(shm_fd,SIZE);

        /* now map the shared memory segment in the address space of the process */
        ptr = mmap(0,SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
        if (ptr == MAP_FAILED) {
            printf("Map failed\n");
            return -1;
        }


        pid = fork();

        
        if(pid==0) //child proces
        {
            // thread
            pthread_t tid[N]; //thread dizisi
            pthread_attr_t attr;
            int i;
            pthread_attr_init(&attr);

            
            char buf[10];
            close(pd[1]);

            for(int i=0;i<10;i++)
            {
                read(pd[0],buf,sizeof(buf));
                pthread_create(&(tid[i]), &attr,task, buf); 
                sleep(1);
                pthread_join(tid[i],NULL);           
            }
            printf("Yavru okudu\n");
        }
        else if(pid>0)  //parent
        {
            close(pd[0]);
            char messages[10][100];
            for (int i = 0; i < 10; i++) {
                sprintf(messages[i], "%d", i + 1);
            }
            for(int i=0;i<10;i++) //yavruya yazdiralacak mesajlar iletilir.
            {
                write(pd[1],messages[i],strlen(messages[i])+1);
                sleep(1);
            }
            wait(NULL);
        }
        else
        {
            printf("fork hatası");
        }
        // Kritik bölge sonu
        sleep(1);
        sem_post(sem_consumer); // Veri üretimi tamamlandı, tüketiciye haber ver
    }
    
    sem_close(sem_producer);
    sem_close(sem_consumer);

    sem_unlink("sem_producer");
    sem_unlink("sem_consumer");
    sem_destroy(sem_producer);   
    sem_destroy(sem_consumer);
    
	return 0;
}
