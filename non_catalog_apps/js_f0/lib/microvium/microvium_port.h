/*

# Instructions

Make a copy of this file and name it exactly `microvium_port.h`. Put the copy somewhere
in your project where it is accessible by a `#include "microvium_port.h"` directive.

Customize your copy of the port file with platform-specific configurations.

The recommended workflow is to keep the vm source files separate from your
custom port file, so that you can update the vm source files regularly with bug
fixes and improvement from the original github or npm repository.

*/
#pragma once

#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>

/**
 * The version of the port interface that this file is implementing.
 */
#define MVM_PORT_VERSION 1

/**
 * Number of bytes to use for the stack.
 *
 * Note: the that stack is fixed-size, even though the heap grows dynamically
 * as-needed.
 */
#define MVM_STACK_SIZE 256

/**
 * When more space is needed for the VM heap, the VM will malloc blocks with a
 * minimum of this size from the host.
 *
 * Note that the VM can also allocate blocks larger than this. It will do so if
 * it needs a larger contiguous space than will fit in a standard block, and
 * also during heap compaction (`runGC`) where it defragments the heap into as
 * few mallocd blocks as possible to make access more efficient.
 */
#define MVM_ALLOCATION_BUCKET_SIZE 256

/**
 * The maximum size of the virtual heap before an MVM_E_OUT_OF_MEMORY error is
 * given.
 *
 * When the VM reaches this level, it will first try to perform a garbage
 * collection cycle. If a GC cycle does not free enough memory, a fatal
 * MVM_E_OUT_OF_MEMORY error is given.
 *
 * Note: this is the space in the virtual heap (the amount consumed by
 * allocations in the VM), not the physical space malloc'd from the host, the
 * latter of which can peak at roughly twice the virtual space during a garbage
 * collection cycle in the worst case.
 */
#define MVM_MAX_HEAP_SIZE 1024

/**
 * Set to 1 if a `void*` pointer is natively 16-bit (e.g. if compiling for
 * 16-bit architectures). This allows some optimizations since then a native
 * pointer can fit in a Microvium value slot.
 */
#define MVM_NATIVE_POINTER_IS_16_BIT 0

/**
 * Set to 1 to compile in support for floating point operations (64-bit). This
 * adds significant cost in smaller devices, but is required if you want the VM
 * to be compliant with the ECMAScript standard.
 *
 * When float support is disabled, operations on floats will throw.
 */
// set to zero for f0 compilation
#define MVM_SUPPORT_FLOAT 0

#if MVM_SUPPORT_FLOAT

/**
 * The type to use for double-precision floating point. Note that anything other
 * than an IEEE 754 double-precision float is not compliant with the ECMAScript
 * spec and results may not always be as expected. Also remember that the
 * bytecode is permitted to have floating point literals embedded in it, and
 * these must match the exact format specification used here if doubles are to
 * persist correctly across a snapshot.
 *
 * Note that on some embedded systems, the `double` type is actually 32-bit, so
 * this may need to be `long double` or whatever the equivalent 64-bit type is
 * on your system.
 */
#define MVM_FLOAT64 double

/**
 * Value to use for NaN
 */
#define MVM_FLOAT64_NAN ((MVM_FLOAT64)(INFINITY * 0.0))

#endif // MVM_SUPPORT_FLOAT

/**
 * Set to `1` to enable additional internal consistency checks, or `0` to
 * disable them. Note that consistency at the API boundary is always checked,
 * regardless of this setting. Consistency checks make the VM *significantly*
 * bigger and slower, and are really only intended for testing.
 */
#define MVM_SAFE_MODE 1

/**
 * Set to `1` to enable extra validation checks of bytecode while executing.
 * This is _beyond_ the basic version and CRC checks that are done upon loading,
 * and should only be enabled if you expect bugs in the bytecode compiler.
 */
#define MVM_DONT_TRUST_BYTECODE 1

/**
 * Not recommended!
 *
 * Set to `1` to enable extra checks for pointer safety within the engine. In
 * particular, this triggers a GC collection cycle at every new allocation in
 * order to find potential dangling pointer issues, and each GC collection
 * shifts the address space a little to invalidate native pointers early.
 * This option is only intended for testing purposes.
 */
#define MVM_VERY_EXPENSIVE_MEMORY_CHECKS 0

/**
 * A long pointer is a type that can refer to either ROM or RAM. It is not size
 * restricted.
 *
 * On architectures where bytecode is directly addressable with a normal
 * pointer, this can just be `void*` (e.g. 32-bit architectures). On
 * architectures where bytecode can be addressed with a special pointer, this
 * might be something like `__data20 void*` (MSP430). On Harvard architectures
 * such as AVR8 where ROM and RAM are in different address spaces,
 * `MVM_LONG_PTR_TYPE` can be some integer type such as `uint32_t`, where you
 * use part of the value to distinguish which address space and part of the
 * value as the actual pointer value.
 *
 * The chosen representation/encoding of `MVM_LONG_PTR_TYPE` must be an integer
 * or pointer type, such that `0`/`NULL` represents the null pointer.
 *
 * Microvium doesn't access data through pointers of this type directly -- it
 * does so through macro operations in this port file.
 */
#define MVM_LONG_PTR_TYPE void*

/**
 * Convert a normal pointer to a long pointer
 */
#define MVM_LONG_PTR_NEW(p) ((MVM_LONG_PTR_TYPE)p)

/**
 * Truncate a long pointer to a normal pointer.
 *
 * This will only be invoked on pointers to VM RAM data.
 */
#define MVM_LONG_PTR_TRUNCATE(p) ((void*)p)

/**
 * Add an offset `s` in bytes onto a long pointer `p`. The result must be a
 * MVM_LONG_PTR_TYPE.
 *
 * The maximum offset that will be passed is 16-bit.
 *
 * Offset may be negative
 */
#define MVM_LONG_PTR_ADD(p, s) ((MVM_LONG_PTR_TYPE)((uint8_t*)p + (intptr_t)s))

/**
 * Subtract two long pointers to get an offset. The result must be a signed
 * 16-bit integer of p2 - p1 (where p2 is the FIRST param).
 */
#define MVM_LONG_PTR_SUB(p2, p1) ((int16_t)((uint8_t*)p2 - (uint8_t*)p1))

/*
 * Read memory of 1 or 2 bytes
 */
#define MVM_READ_LONG_PTR_1(lpSource) (*((uint8_t *)lpSource))
#define MVM_READ_LONG_PTR_2(lpSource) (*((uint16_t *)lpSource))

/**
 * Reference to an implementation of memcmp where p1 and p2 are LONG_PTR
 */
#define MVM_LONG_MEM_CMP(p1, p2, size) memcmp(p1, p2, size)

/**
 * Reference to an implementation of memcpy where `source` is a LONG_PTR
 */
#define MVM_LONG_MEM_CPY(target, source, size) memcpy(target, source, size)

/**
 * This is invoked when the virtual machine encounters a critical internal error
 * and execution of the VM should halt.
 *
 * Note that API-level errors are communicated via returned error codes from
 * each of the API functions and will not trigger a fatal error.
 *
 * Note: if malloc fails, this is considered a fatal error since many embedded
 * systems cannot safely continue when they run out of memory.
 *
 * If you need to halt the VM without halting the host, consider running the VM
 * in a separate RTOS thread, or using setjmp/longjmp to escape the VM without
 * returning to it. Either way, the VM should NOT be allowed to continue
 * executing after MVM_FATAL_ERROR (control should not return).
 */
// fatalError is required due to lack of exit() on f0
extern void fatalError(void* vm, int e);
#define MVM_FATAL_ERROR(vm, e) fatalError(vm, e)

/**
 * Set MVM_ALL_ERRORS_FATAL to 1 to have the MVM_FATAL_ERROR handler called
 * eagerly when a new error is encountered, rather than returning an error code
 * from `mvm_call`. This is mainly for debugging the VM itself, since the
 * MVM_FATAL_ERROR handler is called before unwinding the C stack.
 */
#define MVM_ALL_ERRORS_FATAL 0

#define MVM_SWITCH(tag, upper) switch (tag)
#define MVM_CASE(value) case value

/**
 * Macro that evaluates to true if the CRC of the given data matches the
 * expected value. Note that this is evaluated against the bytecode, so lpData
 * needs to be a long pointer type. If you don't want the overhead of validating
 * the CRC, just return `true`. The Microvium compiler uses CCITT16 as the CRC.
 */
#define MVM_CHECK_CRC16_CCITT(lpData, size, expected) (default_crc16(lpData, size) == expected)

/**
 * Set to 1 to compile in the ability to generate snapshots (mvm_createSnapshot)
 */
#define MVM_INCLUDE_SNAPSHOT_CAPABILITY 1

/**
 * Set to 1 to compile support for the debug API (mvm_dbg_*)
 */
#define MVM_INCLUDE_DEBUG_CAPABILITY 1

#if MVM_INCLUDE_SNAPSHOT_CAPABILITY
/**
 * Calculate the CRC. This is only used when generating snapshots.
 *
 * Unlike MVM_CHECK_CRC16_CCITT, pData here is a pointer to RAM.
 */
#define MVM_CALC_CRC16_CCITT(pData, size) (default_crc16(pData, size))
#endif // MVM_INCLUDE_SNAPSHOT_CAPABILITY

/**
 * On architectures like small ARM MCUs where there is a large address space
 * (e.g. 32-bit) but only a small region of that is used for heap allocations,
 * Microvium is more efficient if you can tell it the high bits of the addresses
 * so it can store the lower 16-bits.
 *
 * If MVM_USE_SINGLE_RAM_PAGE is set to 1, then MVM_RAM_PAGE_ADDR must be
 * the address of the page.
 */
#define MVM_USE_SINGLE_RAM_PAGE 0

#if MVM_USE_SINGLE_RAM_PAGE
/**
 * Address of the RAM page to use, such that all pointers to RAM are between
 * MVM_RAM_PAGE_ADDR and (MVM_RAM_PAGE_ADDR + 0xFFFF)
 */
#define MVM_RAM_PAGE_ADDR 0x12340000
#endif

/**
 * Implementation of malloc and free to use.
 *
 * Note that MVM_CONTEXTUAL_FREE needs to accept null pointers as well.
 *
 * If MVM_USE_SINGLE_RAM_PAGE is set, pointers returned by MVM_CONTEXTUAL_MALLOC
 * must always be within 64kB of MVM_RAM_PAGE_ADDR.
 *
 * The `context` passed to these macros is whatever value that the host passes
 * to `mvm_restore`. It can be any value that fits in a pointer.
 */
#define MVM_CONTEXTUAL_MALLOC(size, context) MVM_MALLOC(size)
#define MVM_CONTEXTUAL_FREE(ptr, context) MVM_FREE(ptr)

/**
 * If defined, this will enable the API methods `mvm_stopAfterNInstructions` and
 * `mvm_getInstructionCountRemaining`.
 */
#define MVM_GAS_COUNTER
