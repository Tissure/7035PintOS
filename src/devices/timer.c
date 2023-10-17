#include "devices/timer.h"
#include <debug.h>
#include <inttypes.h>
#include <round.h>
#include <stdio.h>
#include "devices/pit.h"
#include "threads/interrupt.h"
#include "threads/synch.h"
#include "threads/thread.h"

#include "list.h"
  
/* See [8254] for hardware details of the 8254 timer chip. */

#if TIMER_FREQ < 19
#error 8254 timer requires TIMER_FREQ >= 19
#endif
#if TIMER_FREQ > 1000
#error TIMER_FREQ <= 1000 recommended
#endif

// static struct list blocked_list;

/* Number of timer ticks since OS booted. */
static int64_t ticks;

/* Number of loops per timer tick.
   Initialized by timer_calibrate(). */
static unsigned loops_per_tick;

static intr_handler_func timer_interrupt;
static bool too_many_loops (unsigned loops);
static void busy_wait (int64_t loops);
static void real_time_sleep (int64_t num, int32_t denom);
static void real_time_delay (int64_t num, int32_t denom);

/* Sets up the timer to interrupt TIMER_FREQ times per second,
   and registers the corresponding interrupt. */
void
timer_init (void) 
{
  pit_configure_channel (0, 2, TIMER_FREQ);
  intr_register_ext (0x20, timer_interrupt, "8254 Timer");
//   list_init (&blocked_list);
}

/* Calibrates loops_per_tick, used to implement brief delays. */
void
timer_calibrate (void) 
{
  unsigned high_bit, test_bit;

  ASSERT (intr_get_level () == INTR_ON);
  printf ("Calibrating timer...  ");

  /* Approximate loops_per_tick as the largest power-of-two
     still less than one timer tick. */
  loops_per_tick = 1u << 10;
  while (!too_many_loops (loops_per_tick << 1)) 
    {
      loops_per_tick <<= 1;
      ASSERT (loops_per_tick != 0);
    }

  /* Refine the next 8 bits of loops_per_tick. */
  high_bit = loops_per_tick;
  for (test_bit = high_bit >> 1; test_bit != high_bit >> 10; test_bit >>= 1)
    if (!too_many_loops (loops_per_tick | test_bit))
      loops_per_tick |= test_bit;

  printf ("%'"PRIu64" loops/s.\n", (uint64_t) loops_per_tick * TIMER_FREQ);
}

/* Returns the number of timer ticks since the OS booted. */
int64_t
timer_ticks (void) 
{
  enum intr_level old_level = intr_disable ();
  int64_t t = ticks;
  intr_set_level (old_level);
  return t;
}

/* Returns the number of timer ticks elapsed since THEN, which
   should be a value once returned by timer_ticks(). */
int64_t
timer_elapsed (int64_t then) 
{
  return timer_ticks () - then;
}

/* Sleeps for approximately TICKS timer ticks.  Interrupts must
   be turned on. */
// void
// timer_sleep (int64_t ticks) 
// {
//   int64_t start = timer_ticks ();

//   ASSERT (intr_get_level () == INTR_ON);
//   while (timer_elapsed (start) < ticks) 
//     thread_yield ();
// }
void
timer_sleep (int64_t inputTicks)
{
    // 1. Block the thread (put it into Blocked State).
    // 2. Check every tick for when it does go past the given amount of time (i.e ticks)
    // 3. Then Signal to the Blocked thread to schedule it into the Ready State

    /*
    1. Store the SleepTime to Signal in the Current Thread.
    2. Block the thread, thus adding it to Blocked List.
    3. Check Everytick, the first element (Estimated Time), see if timer_ticks() surpasses Estimated Time
    4x. If yes, access second element, the saved (Thread) and Unblock.
    5x. Remove unblocked Thread from List.
    */

    // printf("Checking Running Thread TID: %i\n", thread_current()->tid);
    
    struct thread* current_thread = thread_current();

    // Disable Interrupts
    // printf("1. Disable Interrupt\n");
    enum intr_level old_level = intr_disable ();

    // Number of timer ticks since OS booted.
    int64_t start = timer_ticks();

    // Record Expected time to signal after sleeping.
    // printf("2. Record time to signal in thread_current()->SleepUntil\n");
    current_thread->SleepUntil = inputTicks;
    // printf("2.1: Thread TID(%i)->SleepUntil = %i, and ticks inputed = %lu\n",current_thread->tid, current_thread->SleepUntil, inputTicks);

    // Add to Blocked_Threads_List
    // printf("3. ADD current Thread to Blocked List\n");
    add_blocked_thread(current_thread);

    print_blocked_list();

    // Block Thread, which adds it to the Blocked List
    // printf("4. Block current Thread\n");
    thread_block();

    // Enable Interrupts
    // printf("5. Re-enable Interrupts\n");
    intr_set_level(old_level);
    

    // // Number of timer ticks since OS booted.
    // int64_t start = timer_ticks();
    // struct thread* current_thread = thread_current();

    // // Disable Interrupts
    // intr_disable ();

    // // Create Timer Entry for Global list.
    // struct list* timer_entry;
    // list_init(timer_entry);

    // // Push-back the Estimated Time and Current Thread to Timer Entry
    // list_push_back(timer_entry, start+ticks);
    // list_push_back(timer_entry, current_thread);

    // while(timer_elapsed(timer_ticks()) > ticks){
    //     thread_block();
    // }

    // // Block Thread
    // thread_unblock(thread_current()); 
    
    // Check every Tick for Event to Signal Blocked Thread

    // int64_t start = timer_ticks ();
    // intr_disable ();
    // thread_block ();
    // while (timer_elapsed (start) < ticks){
    //     printf("Ticks: %i", ticks);
    // };
    // thread_unblock(thread_current());

}

/* Sleeps for approximately MS milliseconds.  Interrupts must be
   turned on. */
void
timer_msleep (int64_t ms) 
{
  real_time_sleep (ms, 1000);
}

/* Sleeps for approximately US microseconds.  Interrupts must be
   turned on. */
void
timer_usleep (int64_t us) 
{
  real_time_sleep (us, 1000 * 1000);
}

/* Sleeps for approximately NS nanoseconds.  Interrupts must be
   turned on. */
void
timer_nsleep (int64_t ns) 
{
  real_time_sleep (ns, 1000 * 1000 * 1000);
}

/* Busy-waits for approximately MS milliseconds.  Interrupts need
   not be turned on.

   Busy waiting wastes CPU cycles, and busy waiting with
   interrupts off for the interval between timer ticks or longer
   will cause timer ticks to be lost.  Thus, use timer_msleep()
   instead if interrupts are enabled. */
void
timer_mdelay (int64_t ms) 
{
  real_time_delay (ms, 1000);
}

/* Sleeps for approximately US microseconds.  Interrupts need not
   be turned on.

   Busy waiting wastes CPU cycles, and busy waiting with
   interrupts off for the interval between timer ticks or longer
   will cause timer ticks to be lost.  Thus, use timer_usleep()
   instead if interrupts are enabled. */
void
timer_udelay (int64_t us) 
{
  real_time_delay (us, 1000 * 1000);
}

/* Sleeps execution for approximately NS nanoseconds.  Interrupts
   need not be turned on.

   Busy waiting wastes CPU cycles, and busy waiting with
   interrupts off for the interval between timer ticks or longer
   will cause timer ticks to be lost.  Thus, use timer_nsleep()
   instead if interrupts are enabled.*/
void
timer_ndelay (int64_t ns) 
{
  real_time_delay (ns, 1000 * 1000 * 1000);
}

/* Prints timer statistics. */
void
timer_print_stats (void) 
{
  printf ("Timer: %"PRId64" ticks\n", timer_ticks ());
}

/* Timer interrupt handler. */
static void
timer_interrupt (struct intr_frame *args UNUSED)
{
  ticks++;
  thread_tick ();

  // 3. Every tick, check the (Estimated Time), see if timer_ticks() surpasses Estimated Time
  // Iterate through all blocked threads in respective list.
  // If current_thread()->SleepTime < timer_elapsed(timer_ticks()) 
//   printf("(Every Tick) Check every thread's SleepUntil in BlockedThreadList\n");
  
  check_thread_timer();  
}

/* Returns true if LOOPS iterations waits for more than one timer
   tick, otherwise false. */
static bool
too_many_loops (unsigned loops) 
{
  /* Wait for a timer tick. */
  int64_t start = ticks;
  while (ticks == start)
    barrier ();

  /* Run LOOPS loops. */
  start = ticks;
  busy_wait (loops);

  /* If the tick count changed, we iterated too long. */
  barrier ();
  return start != ticks;
}

/* Iterates through a simple loop LOOPS times, for implementing
   brief delays.

   Marked NO_INLINE because code alignment can significantly
   affect timings, so that if this function was inlined
   differently in different places the results would be difficult
   to predict. */
static void NO_INLINE
busy_wait (int64_t loops) 
{
  while (loops-- > 0)
    barrier ();
}

/* Sleep for approximately NUM/DENOM seconds. */
static void
real_time_sleep (int64_t num, int32_t denom) 
{
  /* Convert NUM/DENOM seconds into timer ticks, rounding down.
          
        (NUM / DENOM) s          
     ---------------------- = NUM * TIMER_FREQ / DENOM ticks. 
     1 s / TIMER_FREQ ticks
  */
  int64_t ticks = num * TIMER_FREQ / denom;

  ASSERT (intr_get_level () == INTR_ON);
  if (ticks > 0)
    {
      /* We're waiting for at least one full timer tick.  Use
         timer_sleep() because it will yield the CPU to other
         processes. */                
      timer_sleep (ticks); 
    }
  else 
    {
      /* Otherwise, use a busy-wait loop for more accurate
         sub-tick timing. */
      real_time_delay (num, denom); 
    }
}

/* Busy-wait for approximately NUM/DENOM seconds. */
static void
real_time_delay (int64_t num, int32_t denom)
{
  /* Scale the numerator and denominator down by 1000 to avoid
     the possibility of overflow. */
  ASSERT (denom % 1000 == 0);
  busy_wait (loops_per_tick * num / 1000 * TIMER_FREQ / (denom / 1000)); 
}

// void
// add_blocked_thread(struct thread* inputThread)
// {

//     enum intr_level old_level = intr_disable ();

//     if (inputThread != get_idle_thread())
//     {
//         list_push_back(&blocked_list, &inputThread->elem);
//     }
//     printf("Size Of Block Threads List: %i\n", list_size(&blocked_list));

//     intr_set_level(old_level);

// }

// const char* getStatus(enum thread_status input)
// {
//         switch(input)
//         {
//             case THREAD_BLOCKED:
//                 return "Thread Blocked";
//             case THREAD_RUNNING:
//                 return "Thread Running";
//             case THREAD_READY:
//                 return "Thread Ready";
//             case THREAD_DYING:
//                 return "Thread Dying";
//         }
// }

// void 
// print_blocked_list(void)
// {
//     printf("\nPRINTING BLOCKED LIST========\n");
//     struct list_elem* listElementPtr;
//     for(listElementPtr = list_begin(&blocked_list); listElementPtr != list_end(&blocked_list); listElementPtr = list_next(listElementPtr))
//     {
//         struct thread* currentThread = list_entry(listElementPtr, struct thread, elem);
//         printf("Blocked Thread Item TID: %i, STATUS: %s\n", currentThread->tid, getStatus(currentThread->status));
//     }
//     printf("END OF BLOCKED LIST========\n\n");
// }

// void 
// check_thread_timer(int64_t inputTicks)
// {   
//     //printf("Size of Blocked_List: %i", list_size(&blocked_list));
//     // if (thread_current()->status == THREAD_BLOCKED)
//     // {

//     // ASSERT(thread_current()->status == THREAD_BLOCKED);
//     // printf("(Every Tick) Check_Thread_Timer Called, now before For Loop\n");

//     enum intr_level old_level = intr_disable ();
//     struct list_elem* listElementPtr;

//     // printf("(Every Tick) Size of Blocked List: %i\n", list_size(&blocked_list));

//     if (list_empty(&blocked_list)){
//         return;
//     } else {
//         // Iterate through list of blocked threads and unblock them if they are done "Sleeping"
//         for(listElementPtr = list_begin(&blocked_list); listElementPtr != list_end(&blocked_list); listElementPtr = list_next(listElementPtr))
//         {
//             struct thread* currentElement = list_entry(listElementPtr, struct thread, elem);

//             // printf("(Every Tick) Check each List Item: Thread:%i, SleepUntil:%i, Ticks: %i\n", currentThread->tid, currentThread->SleepUntil, inputTicks);
//             // Check to see if Time Since Boot is greater than SleepTime.
//             if(currentElement->SleepUntil <= inputTicks){
//                 printf("(Every Tick) Thread(%i)->SleepUntil: %i, timer_ticks:%i\n", currentElement->tid ,currentElement->SleepUntil, inputTicks);
//                 // Unblock Thread
//                 printf("(Every Tick) Unblock Thread\n");
//                 printf("(Every Tick) Thread(%i) Status: %s\n", currentElement->tid, getStatus(currentElement->status));

//                 thread_unblock(currentElement);
//                 // Remove Current List Element from BlockedList
//                 printf("(Every Tick) Remove Thread(%i) from BlockedList\n", currentElement->tid);
//                 list_remove(listElementPtr);
//                 print_blocked_list();
//             }

//         }
//     }

//     intr_set_level(old_level);

//     // }

//     // Do nothing if Thread Status is running
// }
