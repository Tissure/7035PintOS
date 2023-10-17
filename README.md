# COMP 7035
                         +-----------------+
                         |    COMP 7035    |
                         |   ASSIGNMENT 1  |
                         | DESIGN DOCUMENT |
                         +-----------------+

---- GROUP ----

- Jeff Phan [jphan19@my.bcit.ca]
- Kyle Ng [kng168@my.bcit.ca]
- Jay Wang [jwang550@my.bcit.ca]   

---- PRELIMINARIES ----

- Classmates
- https://www.youtube.com/watch?v=myO2bs5LMak&ab_channel=EE415%3AIntro.toOperatingSystem


--- Jay’s References ---
- https://github.com/dillstead/pintos/blob/assignment_1/src/threads/thread.c#L346 Got the idea to decrement SleepUntil from this person. Ultimately it didn’t fix the Issue with List Traversal.
- https://www.khoury.northeastern.edu/home/skotthe/classes/cs5600/fall/2015/notes/pintos-project1.pdf 
- https://stackoverflow.com/questions/3168306/print-text-instead-of-value-from-c-enum Used this for clarity on the status of my stored threads in the blocked_list. 
- https://www.javatpoint.com/traversing-in-doubly-linked-list 
- https://web.stanford.edu/class/cs140/projects/pintos/pintos_6.html#SEC100 
- https://cs162.org/static/proj/pintos-docs/docs/synch/disable_interrupts/ Wanted to get to this one, but ultimately didn’t use
- https://www.researchgate.net/publication/319135949_Pintos-T01_Timer_Alarms_without_Busy_Waits_--_A_Guide_for_Students 
- https://souzanurafrianto.wordpress.com/2011/05/06/avoiding-busy-wait-in-timer_sleep-on-pintos/ 



                              Timer Sleep
                             	====

---- DATA STRUCTURES ----

Added to struct thread to allow each thread to store its own wakeup 
time in ticks:
 int64_t wakeup_tick; /* Tick to wake up at*/

Added to thread.c to store sleeping threads:
static struct list sleep_list;

---- ALGORITHMS ----

thread_sleep() sets the internal timer (wakeup_tick) signifying the 
time for which we then wake up this thread. It then inserts this thread 
into sleep_list using list_insert_ordered, then setting its status 
to THREAD_BLOCKED.

thread_check_sleep()  is called by every tick tick_interrupt(). It checks 
the first element in sleep_list if it is time to wake up this thread.

We provide thread_wakeup_less (Comparator) function to 
list_insert_ordered() for inserting into a list such that it is sorted.

---- SYNCHRONIZATION ----

We want to prevent context switching when we are adding to sleep_list
as we do not want to have a context switch when a new thread is added to 
the sleep list, but has not been blocked yet. To prevent this we made the
adding and blocking atomic by disabling interrupts and ensuring that each
entry to the sleep list is properly blocked.

---- RATIONALE ----

The original implementation of timer_sleep() used busy waiting to pause
the thread. This method worked but is expensive as it uses CPU time. 
Instead we disable the thread directly by blocking it and putting it on a 
blocked list. By doing so we are able to free up CPU time for other tasks. 

However, this design requires disabling interrupts when adding to the 
sleep list as well  as blocking and unblocking threads. By disabling 
interrupts we force the OS to run this thread only ignoring any other 
action. This means actions like I/O are not able to run while manipulating 
the thread.
