#include <semaphore.h>
#include <stdio.h>
#include <string.h> 
#include <sys/mman.h>
#include <sys/ipc.h>
#include <sys/shm.h>


//
// Shared memory between processes using IPC library.
//



#define SHM_FILE "./shared_data"
#define SHM_FILE_SIZE 255


void main()
{
    int proj_id = 5;
    int counter;
    int shmid;
    char* shm_buff;
    key_t key;

    // Create key.
    if ((key = ftok(SHM_FILE,proj_id)) == -1)
    {
        perror("Craete key failed.\n");
        return;
    }

    if ((shmid = shmget(key,SHM_FILE_SIZE,IPC_CREAT | 0660)) ==  -1)
    {
        perror("shmget failed.\n");
        return;
    }

    if ((shm_buff = shmat(shmid,NULL,0)) == (char*) -1)
    {
        perror("shmget failed.\n");
        return;
    }
    
    memset(shm_buff,'*',SHM_FILE_SIZE);

    while (*shm_buff != 'b')
    {
        printf("Waiting for the client.\n");
        sleep(1);
    }

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
