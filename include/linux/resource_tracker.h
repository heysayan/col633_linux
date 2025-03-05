#ifndef _RESOURCE_TRACKER_H_
#define _RESOURCE_TRACKER_H_
#include <linux/list.h>
#include <linux/types.h>
#include <linux/rwlock_types.h>
struct pid_node {
    struct per_proc_resource * proc_resource;/* Resource utilization of a process */
    struct list_head next_prev_list ; /* contains pointers to previous and next elements */
};
struct per_proc_resource {
    pid_t pid ; /* process id */
    unsigned long heapsize; /* Total memory allocated by
    a process through the brk system call , and the mmap
    system call when used with the MAP_ANONYMOUS |
    MAP_PRIVATE flags . */
    unsigned long openfile_count ;/* Total number of open
    files of a process using the open , openat , and
    openat2 system calls */
};
extern struct list_head tracked_resources_list;
extern rwlock_t resource_tracker_lock;
#endif