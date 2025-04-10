#ifndef _LINUX_GANG_H
#define _LINUX_GANG_H
#include <linux/list.h>

struct gang_task {
    struct task_struct *task;
    int exec_time;
    struct list_head list;
};

struct gang {
    int gangid;
    int size;
    int running;    // whether gang is running
    unsigned long start_jiffies;    // gang execution start time
    struct list_head task_list;  // Doubly linked list of gang_task
    struct list_head gang_node;  //  global gangs list
};
#endif