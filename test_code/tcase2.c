#include "test_template.h"
#include <sys/mman.h>
#include <stdio.h>
#include <unistd.h>
#include <malloc.h>
#include <errno.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdbool.h>

#define MB (1024 * 1024)

/* This thread function performs the memory allocations and file openings.
   It allocates 30 MB using malloc and opens a file in each iteration,
   repeating this process 10 times.
*/
volatile int iter =0 ; 
volatile pid_t child_kernel_tid = -1 ; 
void* thread_work(void* arg) {
    child_kernel_tid = syscall(SYS_getpid);
    // int res_ret = sys_resource_cap(child_kernel_tid,280*MB,5); 
    for (int i = 0; i < 10; i++) {
        void* mem = malloc(30 * MB);
        if (!mem) {
            perror("malloc");
            pthread_exit(NULL);
        }
        memset(mem, 0, 30 * MB);

        int fd = open("dummy_file.txt", O_CREAT | O_RDWR, 0644);
        if (fd < 0) {
            perror("open");
            free(mem);
            pthread_exit(NULL);
        }
        printf("Current iteration: %d\n",iter++); 
        
      
        sleep(1);
    }
    return NULL;
}

int main() {
    pid_t parent_pid = getpid();
    sys_register(parent_pid); 
    /* Set parent's resource limits: 100 MB heap and 2 open files */
    if (sys_resource_cap(parent_pid, 100, 2) != 0) {
        perror("sys_resource_cap");
        exit(EXIT_FAILURE);
    }
    printf("Parent resource limits set: 100 MB heap, 2 open files\n");

    pid_t child_pid = fork();
    if (child_pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (child_pid == 0) {
     
        /* Child process */
        pid_t my_pid = getpid();
        int ret = sys_register(my_pid); 
        int res_ret = sys_resource_cap(my_pid,280,5); 
        printf("Child process registered for resource tracking, pid: %d\n", my_pid);
        if (ret != 0) {
            perror("sys_register");
            exit(EXIT_FAILURE);
        }
        // thread_work(NULL); // uncomment this and comment the thread one if not tgid thing implemented 
        printf("THREAD TCASE CHILD\n"); 
        thread_work(NULL);
        // pthread_t tid;
        // if (pthread_create(&tid, NULL, thread_work, NULL) != 0) {
        // perror("pthread_create");
        // exit(EXIT_FAILURE);
        // }
        // pthread_join(tid, NULL);
       // potentially deregister if not automatic 
       sys_deregister(my_pid); 
    } else {
        /* Parent process monitors the child using fetch_stats */
        struct per_proc_resource stats;
        bool check_once = false ; 
        bool check_twice = false ; 
        while (1) {
         // will run till child has not terminated. 
            int ret = sys_fetch(&stats, child_pid);
            
            if (check_once) {
                printf("Child process may have terminated.\n");
                break;
            }
            // printf("%d",ret); 
            if(ret ==0)
            {
                check_twice = true ; 
                print_stats(&stats);
            }
            if(ret!=0)
            {
                if(check_twice)
                {
                    check_once = true ; 
                }
            }
            sleep(1);
        }
        wait(NULL);

        sys_deregister(parent_pid); 
        printf("DONE\n"); 
    }





    return 0;
}
