#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h> 

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
    // void *old_brk = sbrk(0);
    // sbrk(1024 * 1024);  // Expand heap by 1MB
    // void *new_brk = sbrk(0);
    // printf("Heap expanded by: %ld bytes\n", (char *)new_brk - (char *)old_brk);

    /* 2. Force heap expansion using sbrk() */
    void *old_brk = sbrk(0);
    if (sbrk(128 * 1024) == (void *)-1) {  // Expand heap by 128KB
        perror("sbrk failed");
        syscall(SYS_deregister, pid);
        exit(EXIT_FAILURE);
    }
    void *new_brk = sbrk(0);
    printf("Heap expanded by: %ld bytes\n", (char *)new_brk - (char *)old_brk);


    /* 3. Test open file count */
    /* Open two files using open() */
    int fd1 = open("/test.txt", O_RDONLY);
    if (fd1 < 0) {
        perror("open /test.txt failed");
    } else {
        printf("Opened /test.txt: fd=%d\n", fd1);
    }
    int fd2 = open("/dev/null", O_RDONLY);
    if (fd2 < 0) {
        perror("open /dev/null failed");
    } else {
        printf("Opened /dev/null: fd=%d\n", fd2);
    }

    // Fetch updated resource usage
    struct per_proc_resource stats;
    ret = syscall(SYS_fetch, &stats, pid);
    if (ret < 0) {
        printf("sys_fetch failed: %s\n", strerror(errno));
        syscall(SYS_deregister, pid);
        exit(EXIT_FAILURE);
    } else {
        printf("After opening files and expanding heap:\n");
        printf("Fetched Resource Usage:\n");
        printf("Heap Size: %lu bytes\n", stats.heapsize);
        printf("Open Files: %lu\n", stats.openfile_count);
    }

    /* 5. Close the opened files */
    if (fd1 >= 0) {
        ret = close(fd1);
        if (ret < 0)
            perror("close fd1 failed");
        else
            printf("Closed fd1=%d\n", fd1);
    }
    if (fd2 >= 0) {
        ret = close(fd2);
        if (ret < 0)
            perror("close fd2 failed");
        else
            printf("Closed fd2=%d\n", fd2);
    }

    old_brk = sbrk(0);
    if (sbrk(-128*1024) == (void *)-1) {
        perror("sbrk failed");
    }
    new_brk = sbrk(0);
    printf("Heap decreased by: %ld bytes\n", (char *)new_brk - (char *)old_brk);
    
    /* Give a moment for the close syscalls to update the tracked open file count */
    sleep(1);




    /* 6. Fetch resource usage after closing files */
    ret = syscall(SYS_fetch, &stats, pid);
    if (ret < 0) {
        perror("sys_fetch failed after closing files");
        syscall(SYS_deregister, pid);
        exit(EXIT_FAILURE);
    }
    printf("After closing files and reducing heap:\n");
    printf("  Heap Size: %lu bytes\n", stats.heapsize);
    printf("  Open File Count: %lu\n", stats.openfile_count);

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
