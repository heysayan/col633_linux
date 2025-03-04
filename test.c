#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>
#include <string.h>

#define SYS_register 451  // Replace with actual syscall numbers
#define SYS_fetch 452
#define SYS_deregister 453

struct per_proc_resource {
    pid_t pid ;
     /* process id */
    unsigned long heapsize;
     /* Total memory allocated by
    a process through the brk system call , and the mmap
    system call when used with the MAP_ANONYMOUS |
    MAP_PRIVATE flags . */
    unsigned long openfile_count; /* Total number of open
    files of a process using the open , openat , and
    openat2 system calls */
};

void test_syscalls() {
    pid_t pid = getpid();
    printf("Testing syscalls for PID: %d\n", pid);

    // Register process
    int ret = syscall(SYS_register, pid);
    if (ret < 0) {
        printf("sys_register failed: %s\n", strerror(errno));
        return;
    }
    printf("Process registered successfully.\n");

    // // Allocate memory to trigger brk() syscall
    // void *p1 = malloc(1024 * 1024);  // Allocate 1MB
    // if (!p1) {
    //     printf("Malloc failed.\n");
    //     return;
    // }
    // printf("Allocated 1MB of memory.\n");

    // Force heap expansion using sbrk()
    void *old_brk = sbrk(0);
    sbrk(1024 * 1024);  // Expand heap by 1MB
    void *new_brk = sbrk(0);
    printf("Heap expanded by: %ld bytes\n", (char *)new_brk - (char *)old_brk);


    // Fetch updated resource usage
    struct per_proc_resource stats;
    ret = syscall(SYS_fetch, &stats, pid);
    if (ret < 0) {
        printf("sys_fetch failed: %s\n", strerror(errno));
    } else {
        printf("Fetched Resource Usage:\n");
        printf("Heap Size: %lu bytes\n", stats.heapsize);
        printf("Open Files: %lu\n", stats.openfile_count);
    }

    // Deregister process
    ret = syscall(SYS_deregister, pid);
    if (ret < 0) {
        printf("sys_deregister failed: %s\n", strerror(errno));
    } else {
        printf("Process deregistered successfully.\n");
    }

    // // Free allocated memory
    // free(p1);
}

int main() {
    test_syscalls();
    return 0;
}
