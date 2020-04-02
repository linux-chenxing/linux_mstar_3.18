/* this file collect debug library about thread */

void MDBShowAllThread()
{
    printk("Thread back trace dump!!!\n");
    {
        struct task_struct *g, *p;
        
        printk("Threads unsleeping...\n");
        do_each_thread(g, p) {
            if(p->state == TASK_INTERRUPTIBLE)
            	continue;
            printk("< pid = %d , tgid = %d \n",p->pid,p->tgid);
            sched_show_task(p);
            printk("> \n");
        } while_each_thread(g, p);
        
        printk("\n\nThreads ALL...\n");
        do_each_thread(g, p) {
            printk("< pid = %d , tgid = %d \n",p->pid,p->tgid);
            sched_show_task(p);
            printk("> \n");
        } while_each_thread(g, p);
    }
}