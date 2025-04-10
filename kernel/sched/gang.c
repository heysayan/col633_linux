#include "sched.h"
#include <linux/sched.h>
#include <linux/gang.h>
#include <linux/list.h>
#include <linux/types.h>
#include <linux/jiffies.h>



extern struct list_head gangs;
extern spinlock_t gang_lock;
//static struct gang * curr_gang = NULL;
static void enqueue_gang_task(struct rq *rq, struct task_struct *p, int flags) { 
    /*…*/ 
}

static void dequeue_gang_task(struct rq *rq, struct task_struct *p, int flags) { 
    /*…*/ 
}
static struct task_struct *pick_next_gang_task(struct rq *rq) {
    return NULL;
}

// static struct task_struct *pick_next_gang_task2(struct rq *rq) {
//     unsigned long now = jiffies;
//     struct gang *g = NULL;
//     struct gang *gcur = curr_gang;
//     struct gang_task *gtcur = NULL;
//     struct task_struct *ret = NULL;

//     if (list_empty(&gangs)) // if no gang present, do Fair scheduling
//         return NULL;

//     if (gcur==NULL){    // first gang should be scheduled, no gang has run yet
//         gcur = list_first_entry(&gangs, struct gang, gang_node);
//         gtcur = list_first_entry(&gcur->task_list, struct gang_task, list);
//         if (cpu_of(rq) == task_cpu(gtcur->task) && task_is_running(gtcur->task)) { // current core is governor core
//             g = gcur; // gang to be run
//             curr_gang = g;  //setting currently running gang
//             g->running = 1;
//             ret = gtcur->task;
//             // sending IPI to other gang threads to trigger need_resched using resched_curr(rq) call
//             list_for_each_entry_continue(gtcur,&g->task_list,list){
//                 if (task_is_running(gtcur->task))   // other gang task is running
//                     resched_curr( cpu_rq(task_cpu(gtcur->task)) );  // sends IPI to core and triggers need_resched on it
//             }
//             g->start_jiffies = now; // setting start time of gang
//         }
//     }
//     else if (gcur->running == 0){   // new gang should be scheduled
//         struct list_head *head;
//         gcur = list_next_entry_circular(gcur, &gangs, gang_node);
//         head = &gcur->gang_node;
//         do{ // loops from gcur and till gcur->gang_node.prev / from next to curr_gang till curr_gang circularly
//             gtcur = list_first_entry(&gcur->task_list, struct gang_task, list);
//             if (cpu_of(rq) == task_cpu(gtcur->task) && task_is_running(gtcur->task)) { // current core is governor core
//                 g = gcur; // gang to be run
//                 curr_gang = g;  //setting currently running gang
//                 g->running = 1;
//                 ret = gtcur->task;
//                 // sending IPI to other gang threads to trigger need_resched using resched_curr(rq) call
//                 list_for_each_entry_continue(gtcur,&g->task_list,list){
//                     if (task_is_running(gtcur->task))   // other gang task is running
//                         resched_curr( cpu_rq(task_cpu(gtcur->task)) );  // sends IPI to core and triggers need_resched on it
//                 }
//                 g->start_jiffies = now; // setting start time of gang
//                 break;
//             }
//         }
//         while(!list_entry_is_head(gcur, head, gang_node));


//         // list_for_each_entry_continue_pivot(gcur, &gcur->gang_node, &gangs , gang_node){ // loops after and till gcur
//         //     gtcur = list_first_entry(&gcur->task_list, struct gang_task, list);
//         //     if (cpu_of(rq) == task_cpu(gtcur->task) && task_is_running(gtcur->task)) { // current is governor core
//         //         g = gcur; // gang to be run
//         //         curr_gang = g;  //setting currently running gang
//         //         g->running = 1;
//         //         ret = gtcur->task;
//         //         // sending IPI to other gang threads to trigger need_resched using resched_curr(rq) call
//         //         list_for_each_entry_continue(gtcur,&g->task_list,list){
//         //             if (task_is_running(gtcur->task))   // other gang task is running
//         //                 resched_curr( cpu_rq(task_cpu(gtcur->task)) );  // sends IPI to core and triggers need_resched on it
//         //         }
//         //         g->start_jiffies = now; // setting start time of gang
//         //         break;
//         //     }
//         // }
//     }
//     else{   // gang already running, so the gang thread must have been preempted
//         list_for_each_entry(gtcur, &gcur->task_list, list) {
//             if (cpu_of(rq) == task_cpu(gtcur->task)) { // task scheduled on this cpu
//                 if(task_is_running(gtcur->task)){   //task is running
//                     ret = gtcur->task;
//                     break;
//                 }
//             }
//         }
//     }

//     return ret;


//     // if (!curr_gang){ // no gang already running, need to schedule fresh gang, only relevant if gang governor on this core
        
//     //     // deciding gang to be run, which is gang just after recently running gang, with gang leader running on this core

//     //     list_for_each_entry(gcur, &gangs, gang_node) { //looping to get recently running gang
//     //         if (gcur->running && time_after_eq(now, gcur->start_jiffies + msecs_to_jiffies(10))){ //this is prev gang
//     //             // reseting prev gang
//     //             gcur->running = 0;
//     //             gcur->start_jiffies = 0;
//     //             break;
//     //         }
//     //     }
//     //     list_for_each_entry_continue_pivot(gcur, &gcur->gang_node, &gangs , gang_node){ // loops after and till gcur
//     //         gtcur = list_first_entry(&gcur->task_list, struct gang_task, list);
//     //         if (cpu_of(rq) == task_cpu(gtcur->task) && task_is_running(gtcur->task)) { // governor core detected
//     //             g = gcur; // gang to be run
//     //             curr_gang = g;  //setting currently running gang
//     //             g->running = 1;
//     //             ret = gtcur->task;
//     //             // sending IPI to trigger need_resched using resched_curr(rq) call
//     //             list_for_each_entry_continue(gtcur,&g->task_list,list){
//     //                 resched_curr( cpu_rq(task_cpu(gtcur->task)) );  // sends IPI to core and triggers need_resched on it
//     //             }
//     //             g->start_jiffies = now; // setting start time of gang
//     //             return g;
//     //         }
//     //     }
//     //     if (!g){    // no gang foung
//     //         // so trying recently runnnig gang again
//     //         gtcur = list_first_entry(&gcur->task_list, struct gang_task, list);
//     //         if (cpu_of(rq) == task_cpu(gtcur->task) && task_is_running(gtcur->task)) { // governor core detected
//     //             g = gcur; // gang to be run
//     //             curr_gang = g;  //setting currently running gang
//     //             g->running = 1;
//     //             ret = gtcur->task;
//     //             // sending IPI to trigger need_resched using resched_curr(rq) call
//     //             list_for_each_entry_continue(gtcur,&g->task_list,list){
//     //                 resched_curr( cpu_rq(task_cpu(gtcur->task)) );  // sends IPI to core and triggers need_resched on it
//     //             }
//     //             g->start_jiffies = now; // setting start time of gang
//     //             return g;
//     //         }
//     //         else return NULL;
//     //     }


//     //     // // selecting gang next to recently running gang
//     //     // g = list_next_entry_circular(gcur, &gangs, gang_node);


//     //     // //traversing all the tasks of the gang to find task to be scheduled, which is task running on this cpu
//     //     // list_for_each_entry(gtcur, &g->task_list, list) {
//     //     //     if (cpu_of(rq) == task_cpu(gtcur->task)) { // task scheduled on this cpu
//     //     //         if(task_is_running(gtcur->task)){   //task is running
//     //     //             if (list_is_first(&gtcur->list,&g->task_list)){  // First task of gang, thus governor core
//     //     //                 ret = gtcur->task;
//     //     //                 gt = gtcur;
//     //     //                 curr_gang = g;  //setting currently running gang
//     //     //                 curr_gang->running = 1;
//     //     //                 // sending IPI to trigger need_resched using resched_curr(rq) call
//     //     //                 list_for_each_entry_continue(gtcur,&g->task_list,list){
//     //     //                     resched_curr( cpu_rq(task_cpu(gtcur->task)) );  // sends IPI to core and triggers need_resched on it
//     //     //                 }
//     //     //             }
//     //     //         }
//     //     //     break;
//     //     //     }
//     //     // }
//     // }
//     // else{   // a gang already running
//     //     struct gang *g;
//     //     struct gang *gcur;
//     //     if (time_after_eq(now, curr_gang->start_jiffies + msecs_to_jiffies(10))){   // time quanta of gang ended
//     //         // reseting gang
//     //         curr_gang->running = 0;
//     //         curr_gang->start_jiffies = 0;
//     //         // changing gang
//     //         curr_gang = (curr_gang->gang_node).next ;
//     //         curr_gang->running = 1;
//     //     }
//     // }
        
// }


static void task_tick_gang(struct rq *rq, struct task_struct *p, int queued){
}

// static void task_tick_gang2(struct rq *rq, struct task_struct *p, int queued) {
//     unsigned long now = jiffies;
//     if (curr_gang && curr_gang->running && time_after_eq(now, curr_gang->start_jiffies + msecs_to_jiffies(10))){   // time quanta of gang ended
//         // reseting gang
//         curr_gang->running = 0;
//         curr_gang->start_jiffies = 0;
//         // changing gang
//         resched_curr(rq); // rescheduling the current core
//     }
// }

static void switched_to_gang(struct rq *rq, struct task_struct *p) { 
    /*…*/ 
}

DEFINE_SCHED_CLASS(gang) = {
	.enqueue_task   = enqueue_gang_task,
	.dequeue_task   = dequeue_gang_task,
	.pick_next_task = pick_next_gang_task,
	.switched_to    = switched_to_gang,
	.task_tick      = task_tick_gang,   // optional but highly recommended
};