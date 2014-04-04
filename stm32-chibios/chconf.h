#ifndef _CHCONF_H_
#define _CHCONF_H_

#if !defined(CH_FREQUENCY)
#define CH_FREQUENCY                    1000
#endif

#if !defined(CH_TIME_QUANTUM)
#define CH_TIME_QUANTUM                 20
#endif

#if !defined(CH_MEMCORE_SIZE)
#define CH_MEMCORE_SIZE                 0
#endif

#if !defined(CH_NO_IDLE_THREAD)
#define CH_NO_IDLE_THREAD               FALSE
#endif

#if !defined(CH_OPTIMIZE_SPEED)
#define CH_OPTIMIZE_SPEED               TRUE
#endif

#if !defined(CH_USE_REGISTRY)
#define CH_USE_REGISTRY                 TRUE
#endif

#if !defined(CH_USE_WAITEXIT)
#define CH_USE_WAITEXIT                 TRUE
#endif

#if !defined(CH_USE_SEMAPHORES)
#define CH_USE_SEMAPHORES               TRUE
#endif

#if !defined(CH_USE_SEMAPHORES_PRIORITY)
#define CH_USE_SEMAPHORES_PRIORITY      FALSE
#endif

#if !defined(CH_USE_SEMSW)
#define CH_USE_SEMSW                    TRUE
#endif

#if !defined(CH_USE_MUTEXES)
#define CH_USE_MUTEXES                  TRUE
#endif

#if !defined(CH_USE_CONDVARS)
#define CH_USE_CONDVARS                 TRUE
#endif


#if !defined(CH_USE_CONDVARS_TIMEOUT)
#define CH_USE_CONDVARS_TIMEOUT         TRUE
#endif

#if !defined(CH_USE_EVENTS)
#define CH_USE_EVENTS                   TRUE
#endif

#if !defined(CH_USE_EVENTS_TIMEOUT)
#define CH_USE_EVENTS_TIMEOUT           TRUE
#endif

#if !defined(CH_USE_MESSAGES)
#define CH_USE_MESSAGES                 TRUE
#endif

#if !defined(CH_USE_MESSAGES_PRIORITY)
#define CH_USE_MESSAGES_PRIORITY        FALSE
#endif

#if !defined(CH_USE_MAILBOXES)
#define CH_USE_MAILBOXES                TRUE
#endif

#if !defined(CH_USE_QUEUES)
#define CH_USE_QUEUES                   TRUE
#endif

#if !defined(CH_USE_MEMCORE)
#define CH_USE_MEMCORE                  TRUE
#endif

#if !defined(CH_USE_HEAP)
#define CH_USE_HEAP                     TRUE
#endif

#if !defined(CH_USE_MALLOC_HEAP)
#define CH_USE_MALLOC_HEAP              FALSE
#endif

#if !defined(CH_USE_MEMPOOLS)
#define CH_USE_MEMPOOLS                 TRUE
#endif

#if !defined(CH_USE_DYNAMIC)
#define CH_USE_DYNAMIC                  TRUE
#endif

#if !defined(CH_DBG_SYSTEM_STATE_CHECK)
#define CH_DBG_SYSTEM_STATE_CHECK       FALSE
#endif

#if !defined(CH_DBG_ENABLE_CHECKS)
#define CH_DBG_ENABLE_CHECKS            FALSE
#endif

#if !defined(CH_DBG_ENABLE_ASSERTS)
#define CH_DBG_ENABLE_ASSERTS           FALSE
#endif

#if !defined(CH_DBG_ENABLE_TRACE)
#define CH_DBG_ENABLE_TRACE             FALSE
#endif

#if !defined(CH_DBG_ENABLE_STACK_CHECK)
#define CH_DBG_ENABLE_STACK_CHECK       FALSE
#endif

#if !defined(CH_DBG_FILL_THREADS)
#define CH_DBG_FILL_THREADS             FALSE
#endif

#if !defined(CH_DBG_THREADS_PROFILING)
#define CH_DBG_THREADS_PROFILING        TRUE
#endif

#if !defined(THREAD_EXT_FIELDS)
#define THREAD_EXT_FIELDS                                                   \
  /* Add threads custom fields here.*/
#endif

#if !defined(THREAD_EXT_INIT_HOOK)
#define THREAD_EXT_INIT_HOOK(tp) {                                          \
  /* Add threads initialization code here.*/                                \
}
#endif

#if !defined(THREAD_EXT_EXIT_HOOK)
#define THREAD_EXT_EXIT_HOOK(tp) {                                          \
  /* Add threads finalization code here.*/                                  \
}
#endif

#if !defined(THREAD_CONTEXT_SWITCH_HOOK)
#define THREAD_CONTEXT_SWITCH_HOOK(ntp, otp) {                              \
  /* System halt code here.*/                                               \
}
#endif

#if !defined(IDLE_LOOP_HOOK)
#define IDLE_LOOP_HOOK() {                                                  \
  /* Idle loop code here.*/                                                 \
}
#endif

#if !defined(SYSTEM_TICK_EVENT_HOOK)
#define SYSTEM_TICK_EVENT_HOOK() {                                          \
  /* System tick event code here.*/                                         \
}
#endif

#if !defined(SYSTEM_HALT_HOOK)
#define SYSTEM_HALT_HOOK() {                                                \
  /* System halt code here.*/                                               \
}
#endif

#define CORTEX_USE_FPU                  FALSE

#endif
