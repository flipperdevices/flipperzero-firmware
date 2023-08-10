/* linuxkm_memory.c
 *
 * Copyright (C) 2006-2023 wolfSSL Inc.
 *
 * This file is part of wolfSSL.
 *
 * wolfSSL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * wolfSSL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
 */

/* included by wolfcrypt/src/memory.c */

#ifdef HAVE_KVMALLOC
/* adapted from kvrealloc() draft by Changli Gao, 2010-05-13 */
void *lkm_realloc(void *ptr, size_t newsize) {
    void *nptr;
    size_t oldsize;

    if (unlikely(newsize == 0)) {
        kvfree(ptr);
        return ZERO_SIZE_PTR;
    }

    if (unlikely(ptr == NULL))
        return kvmalloc_node(newsize, GFP_KERNEL, NUMA_NO_NODE);

    if (is_vmalloc_addr(ptr)) {
        /* no way to discern the size of the old allocation,
         * because the kernel doesn't export find_vm_area().  if
         * it did, we could then call get_vm_area_size() on the
         * returned struct vm_struct.
         */
        return NULL;
    } else {
#ifndef __PIE__
        struct page *page;

        page = virt_to_head_page(ptr);
        if (PageSlab(page) || PageCompound(page)) {
            if (newsize < PAGE_SIZE)
#endif /* ! __PIE__ */
                return krealloc(ptr, newsize, GFP_KERNEL);
#ifndef __PIE__
            oldsize = ksize(ptr);
        } else {
            oldsize = page->private;
            if (newsize <= oldsize)
                return ptr;
        }
#endif /* ! __PIE__ */
    }

    nptr = kvmalloc_node(newsize, GFP_KERNEL, NUMA_NO_NODE);
    if (nptr != NULL) {
        memcpy(nptr, ptr, oldsize);
        kvfree(ptr);
    }

    return nptr;
}
#endif /* HAVE_KVMALLOC */

#if defined(WOLFSSL_LINUXKM_USE_SAVE_VECTOR_REGISTERS) && defined(CONFIG_X86)

static unsigned int wc_linuxkm_fpu_states_n_tracked = 0;

struct wc_thread_fpu_count_ent {
    volatile pid_t pid;
    unsigned int fpu_state;
};
struct wc_thread_fpu_count_ent *wc_linuxkm_fpu_states = NULL;
#define WC_FPU_COUNT_MASK 0x7fffffffU
#define WC_FPU_SAVED_MASK 0x80000000U

WARN_UNUSED_RESULT int allocate_wolfcrypt_linuxkm_fpu_states(void)
{
    if (wc_linuxkm_fpu_states != NULL) {
        static int warned_for_repeat_alloc = 0;
        if (! warned_for_repeat_alloc) {
            pr_err("attempt at repeat allocation"
                   " in allocate_wolfcrypt_linuxkm_fpu_states\n");
            warned_for_repeat_alloc = 1;
        }
        return BAD_STATE_E;
    }

    if (nr_cpu_ids >= 16)
        wc_linuxkm_fpu_states_n_tracked = nr_cpu_ids * 2;
    else
        wc_linuxkm_fpu_states_n_tracked = 32;

    wc_linuxkm_fpu_states =
        (struct wc_thread_fpu_count_ent *)malloc(
            wc_linuxkm_fpu_states_n_tracked * sizeof(wc_linuxkm_fpu_states[0]));

    if (! wc_linuxkm_fpu_states) {
        pr_err("allocation of %lu bytes for "
               "wc_linuxkm_fpu_states failed.\n",
               nr_cpu_ids * sizeof(struct fpu_state *));
        return MEMORY_E;
    }

    memset(wc_linuxkm_fpu_states, 0, wc_linuxkm_fpu_states_n_tracked * sizeof(wc_linuxkm_fpu_states[0]));

    return 0;
}

void free_wolfcrypt_linuxkm_fpu_states(void) {
    struct wc_thread_fpu_count_ent *i, *i_endptr;
    pid_t i_pid;

    if (wc_linuxkm_fpu_states == NULL) {
        pr_err("free_wolfcrypt_linuxkm_fpu_states called"
               " before allocate_wolfcrypt_linuxkm_fpu_states.\n");
        return;
    }

    for (i = wc_linuxkm_fpu_states,
             i_endptr = &wc_linuxkm_fpu_states[wc_linuxkm_fpu_states_n_tracked];
         i < i_endptr;
         ++i)
    {
        i_pid = __atomic_load_n(&i->pid, __ATOMIC_CONSUME);
        if (i_pid == 0)
            continue;
        if (i->fpu_state != 0) {
            pr_err("free_wolfcrypt_linuxkm_fpu_states called"
                   " with nonzero state 0x%x for pid %d.\n", i->fpu_state, i_pid);
            i->fpu_state = 0;
        }
    }

    free(wc_linuxkm_fpu_states);
    wc_linuxkm_fpu_states = NULL;
}

/* lock-(mostly)-free thread-local storage facility for tracking recursive fpu pushing/popping */
static struct wc_thread_fpu_count_ent *wc_linuxkm_fpu_state_assoc(int create_p) {
    struct wc_thread_fpu_count_ent *i, *i_endptr, *i_empty;
    pid_t my_pid = task_pid_nr(current), i_pid;

    {
        static int _warned_on_null = 0;
        if (wc_linuxkm_fpu_states == NULL)
        {
            if (_warned_on_null == 0) {
                pr_err("wc_linuxkm_fpu_state_assoc called by pid %d"
                       " before allocate_wolfcrypt_linuxkm_fpu_states.\n", my_pid);
                _warned_on_null = 1;
            }
            return NULL;
        }
    }

    i_endptr = &wc_linuxkm_fpu_states[wc_linuxkm_fpu_states_n_tracked];

    for (;;) {
        for (i = wc_linuxkm_fpu_states,
                 i_empty = NULL;
             i < i_endptr;
             ++i)
        {
            i_pid = __atomic_load_n(&i->pid, __ATOMIC_CONSUME);
            if (i_pid == my_pid)
                return i;
            if ((i_empty == NULL) && (i_pid == 0))
                i_empty = i;
        }
        if ((i_empty == NULL) || (! create_p))
            return NULL;

        i_pid = 0;
        if (__atomic_compare_exchange_n(
                &(i_empty->pid),
                &i_pid,
                my_pid,
                0 /* weak */,
                __ATOMIC_SEQ_CST /* success_memmodel */,
                __ATOMIC_SEQ_CST /* failure_memmodel */))
        {
            return i_empty;
        }
    }
}

static void wc_linuxkm_fpu_state_free(struct wc_thread_fpu_count_ent *ent) {
    if (ent->fpu_state != 0) {
        static int warned_nonzero_fpu_state = 0;
        if (! warned_nonzero_fpu_state) {
            pr_err("wc_linuxkm_fpu_state_free for pid %d"
                   " with nonzero fpu_state 0x%x.\n", ent->pid, ent->fpu_state);
            warned_nonzero_fpu_state = 1;
        }
        ent->fpu_state = 0;
    }
    __atomic_store_n(&ent->pid, 0, __ATOMIC_RELEASE);
}

WARN_UNUSED_RESULT int save_vector_registers_x86(void)
{
    struct wc_thread_fpu_count_ent *pstate = wc_linuxkm_fpu_state_assoc(1);
    if (pstate == NULL)
        return ENOMEM;

    /* allow for nested calls */
    if (pstate->fpu_state != 0U) {
        if ((pstate->fpu_state & WC_FPU_COUNT_MASK)
            == WC_FPU_COUNT_MASK)
        {
            pr_err("save_vector_registers_x86 recursion register overflow for "
                   "pid %d.\n", pstate->pid);
            return BAD_STATE_E;
        } else {
            ++pstate->fpu_state;
            return 0;
        }
    }

    if (irq_fpu_usable()) {
#if defined(CONFIG_SMP) && !defined(CONFIG_PREEMPT_COUNT) && (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 7, 0))
        /* inhibit migration, which gums up the algorithm in kernel_fpu_{begin,end}(). */
        migrate_disable();
#endif
        kernel_fpu_begin();
        pstate->fpu_state = 1U; /* set msb 0 to trigger kernel_fpu_end() at cleanup. */
    } else if (in_nmi() || (hardirq_count() > 0) || (softirq_count() > 0)) {
        static int warned_fpu_forbidden = 0;
        if (! warned_fpu_forbidden)
            pr_err("save_vector_registers_x86 called from IRQ handler.\n");
        wc_linuxkm_fpu_state_free(pstate);
        return EPERM;
    } else {
#if defined(CONFIG_SMP) && !defined(CONFIG_PREEMPT_COUNT) && (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 7, 0))
        migrate_disable();
#endif
        /* assume already safely in_kernel_fpu. */
        pstate->fpu_state =
            WC_FPU_SAVED_MASK + 1U; /* set msb 1 to inhibit kernel_fpu_end() at cleanup. */
    }

    return 0;
}

void restore_vector_registers_x86(void)
{
    struct wc_thread_fpu_count_ent *pstate = wc_linuxkm_fpu_state_assoc(0);
    if (pstate == NULL) {
        pr_err("restore_vector_registers_x86 called by pid %d "
               "with no saved state.\n", task_pid_nr(current));
        return;
    }

    if ((--pstate->fpu_state & WC_FPU_COUNT_MASK) > 0U) {
        return;
    }

    if (pstate->fpu_state == 0U)
        kernel_fpu_end();
    else
        pstate->fpu_state = 0U;
#if defined(CONFIG_SMP) && !defined(CONFIG_PREEMPT_COUNT) && (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 7, 0))
    migrate_enable();
#endif

    wc_linuxkm_fpu_state_free(pstate);

    return;
}
#endif /* WOLFSSL_LINUXKM_USE_SAVE_VECTOR_REGISTERS && CONFIG_X86 */

#if defined(__PIE__) && (LINUX_VERSION_CODE >= KERNEL_VERSION(6, 1, 0))
/* needed in 6.1+ because show_free_areas() static definition in mm.h calls
 * __show_free_areas(), which isn't exported (neither was show_free_areas()).
 */
void my__show_free_areas(
    unsigned int flags,
    nodemask_t *nodemask,
    int max_zone_idx)
{
    (void)flags;
    (void)nodemask;
    (void)max_zone_idx;
    return;
}
#endif
