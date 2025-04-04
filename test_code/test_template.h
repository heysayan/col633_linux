#include <sys/syscall.h> 
#include <stdio.h> 
#include <unistd.h> 
#include <string.h> 
#include <stdlib.h> 
#include <sys/types.h> 

struct per_proc_resource {
    pid_t pid; /* process id */
    unsigned long heapsize;  /* Total memory allocated by
    a process through the brk system call , and the mmap
    system call when used with the MAP_ANONYMOUS |
    MAP_PRIVATE flags . */
    unsigned long openfile_count;  /* Total number of open
    files of a process using the open , openat , and
    openat2 system calls */
    };

#define __NR_register 451 
#define __NR_fetch 452 
#define __NR_deregister 453 
#define __NR_resource_cap 454
#define __NR_resource_reset 455
int sys_register(pid_t pid)
{
    return syscall(__NR_register,pid); 
}
int sys_deregister(pid_t pid)
{
    return syscall(__NR_deregister,pid); 
}
int sys_fetch(struct per_proc_resource* stats,pid_t pid)
{
    return syscall(__NR_fetch,stats,pid) ; 
}
int sys_resource_cap(pid_t pid,long heapquota,long filequoata)
{
    return syscall(__NR_resource_cap,pid,heapquota,filequoata); 
}
int sys_resource_reset(pid_t pid)
{
    return syscall(__NR_resource_cap,pid); 
}
void print_stats(struct per_proc_resource* stats)
{
    printf("Stats for pid: %d, Heapsize: %ld, openfilecount: %ld\n",stats->pid,stats->heapsize,stats->openfile_count); 
}

