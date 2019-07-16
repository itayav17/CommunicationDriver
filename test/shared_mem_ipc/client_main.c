#include <semaphore.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/ipc.h>
#include <sys/shm.h>


//
// Shared memory between processes using IPC library.
//


<<<<<<< HEAD
// structure for message queue 
struct mesg_buffer
{ 
    long mesg_type; 
    char mesg_text[100]; 
} message;

#define SHM_FILE "./shared_mem"
=======

#define SHM_FILE "./shared_data"
>>>>>>> cc2165481a5306708d2681f0fd67cf437b7c91d5
#define SHM_FILE_SIZE 255


void main()
{
    int proj_id = 5;
    int counter;
    int shmid;
<<<<<<< HEAD
    int msgid;
=======
>>>>>>> cc2165481a5306708d2681f0fd67cf437b7c91d5
    char* shm_buff;
    key_t key;

    // Create key.
    if ((key = ftok(SHM_FILE,proj_id)) == -1)
    {
        perror("Craete key failed.\n");
        return;
    }

    if ((shmid = shmget(key,SHM_FILE_SIZE,0666)) ==  -1)
    {
        perror("shmget failed.\n");
        return;
    }

    if ((shm_buff = shmat(shmid,NULL,0)) == (char*) -1)
    {
        perror("shmget failed.\n");
        return;
    }

    for(counter = 0;
            counter < SHM_FILE_SIZE;
            counter++)
<<<<<<< HEAD
        printf("%c",shm_buff[counter]);


    *shm_buff = 'b';

    if ((msgid = msgget(key,0666 | IPC_CREAT)) == -1)
    {
        perror("msgget failed.\n");
        return;
    }
    
    if (msgrcv(msgid, &message, sizeof(message), 1, 0) < 0)
    {
		perror("msgrcv");
		return;
	}

    // display the message 
    printf("Data Received is : %s \n",  
           message.mesg_text);

    // Destroy the message queue 
    msgctl(msgid, IPC_RMID, NULL);

    if (!shmdt(shm_buff) == -1)
        perror("Failed to detach shared memory.\n");
}
=======
        putchar(shm_buff[counter]);

    *shm_buff = 'b';

    if (!shmdt(shm_buff) == -1)
        perror("Failed to detach shared memory.\n");
}





































/*



#define PROCESS_NUMBER 5
sem_t* semaphore= NULL;





void main(void)
{
    int shared_process = 1;
    int offset = 0;
    int fd = -1;
    int counter;
    pid_t pid_master;
    pid_t pid;
    key_t key;
    void* data;

    
    // Create system V IPC key.
    if ((key = ftok(SHM_FILE,'b')) == -1)
    {
        perror("Failed to create key.\n");
        return;
    }
    
    // Create the segment.
    data =
        mmap(NULL,4,
                PROT_READ | PROT_WRITE,
                MAP_SHARED | MAP_ANONYMOUS,
                -1, 0);

    memset(data,4,4);

    if (semaphore == MAP_FAILED)
    {
        perror("Failed to mapping the semaphore.\n");
        return;
    }

    if (sem_init(semaphore,shared_process,
                    PROCESS_NUMBER - 1))
    {
        perror("Semaphore initialization failed");
        return;
    }

    // Get the process master id.
    pid_master = getpid();

    for (counter = 0;
            counter < PROCESS_NUMBER;
            counter++)
        // Master process could create new processes.
        if (pid_master == getpid())
            pid = fork();

    if(pid_master == getpid())
        producer();
    else if (!pid)
        consumer();
    else if (pid < 0)
    {
        perror("Fork failed.");
        return; 
    }

    if (munmap(semaphore,sizeof(sem_t)) < 0)
    {
        perror("Unmap semaphore failed"\n);
        return;
    }

    return 0;

*/
>>>>>>> cc2165481a5306708d2681f0fd67cf437b7c91d5
