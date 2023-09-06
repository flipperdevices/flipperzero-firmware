// Copyright 2020 Michael Hunter. Part of the Microvium project. Links to full code at https://microvium.com for license details.

/*
 * Microvium Bytecode Interpreter
 *
 * Version: 8.0.0
 *
 * This is the main header for the Microvium bytecode interpreter. Latest source
 * available at https://microvium.com. Raise issues at
 * https://github.com/coder-mike/microvium/issues.
 */
#pragma once

#include "microvium_port.h"
#include <stdbool.h>
#include <stdint.h>

#define MVM_ENGINE_MAJOR_VERSION 8  /* aka MVM_BYTECODE_VERSION */
#define MVM_ENGINE_MINOR_VERSION 0  /* aka MVM_ENGINE_VERSION */

typedef uint16_t mvm_Value;
typedef uint16_t mvm_VMExportID;
typedef uint16_t mvm_HostFunctionID;

typedef enum mvm_TeError {
  /*  0 */ MVM_E_SUCCESS,
  /*  1 */ MVM_E_UNEXPECTED,
  /*  2 */ MVM_E_MALLOC_FAIL,
  /*  3 */ MVM_E_ALLOCATION_TOO_LARGE,
  /*  4 */ MVM_E_INVALID_ADDRESS,
  /*  5 */ MVM_E_COPY_ACROSS_BUCKET_BOUNDARY,
  /*  6 */ MVM_E_FUNCTION_NOT_FOUND,
  /*  7 */ MVM_E_INVALID_HANDLE,
  /*  8 */ MVM_E_STACK_OVERFLOW,
  /*  9 */ MVM_E_UNRESOLVED_IMPORT,
  /* 10 */ MVM_E_ATTEMPT_TO_WRITE_TO_ROM,
  /* 11 */ MVM_E_INVALID_ARGUMENTS,
  /* 12 */ MVM_E_TYPE_ERROR,
  /* 13 */ MVM_E_TARGET_NOT_CALLABLE,
  /* 14 */ MVM_E_HOST_ERROR,
  /* 15 */ MVM_E_NOT_IMPLEMENTED,
  /* 16 */ MVM_E_HOST_RETURNED_INVALID_VALUE,
  /* 17 */ MVM_E_ASSERTION_FAILED,
  /* 18 */ MVM_E_INVALID_BYTECODE,
  /* 19 */ MVM_E_UNRESOLVED_EXPORT,
  /* 20 */ MVM_E_RANGE_ERROR,
  /* 21 */ MVM_E_DETACHED_EPHEMERAL,
  /* 22 */ MVM_E_TARGET_IS_NOT_A_VM_FUNCTION,
  /* 23 */ MVM_E_FLOAT64,
  /* 24 */ MVM_E_NAN,
  /* 25 */ MVM_E_NEG_ZERO,
  /* 26 */ MVM_E_OPERATION_REQUIRES_FLOAT_SUPPORT,
  /* 27 */ MVM_E_BYTECODE_CRC_FAIL,
  /* 28 */ MVM_E_BYTECODE_REQUIRES_FLOAT_SUPPORT,
  /* 29 */ MVM_E_PROTO_IS_READONLY, // The __proto__ property of objects and arrays is not mutable
  /* 30 */ MVM_E_SNAPSHOT_TOO_LARGE, // The resulting snapshot does not fit in the 64kB boundary
  /* 31 */ MVM_E_MALLOC_MUST_RETURN_POINTER_TO_EVEN_BOUNDARY,
  /* 32 */ MVM_E_ARRAY_TOO_LONG,
  /* 33 */ MVM_E_OUT_OF_MEMORY, // Allocating a new block of memory from the host causes it to exceed MVM_MAX_HEAP_SIZE
  /* 34 */ MVM_E_TOO_MANY_ARGUMENTS, // Exceeded the maximum number of arguments for a function (255)
  /* 35 */ MVM_E_REQUIRES_LATER_ENGINE, // Please update your microvium.h and microvium.c files
  /* 36 */ MVM_E_PORT_FILE_VERSION_MISMATCH, // Please migrate your port file to the required version
  /* 37 */ MVM_E_PORT_FILE_MACRO_TEST_FAILURE, // Something in microvium_port.h doesn't behave as expected
  /* 38 */ MVM_E_EXPECTED_POINTER_SIZE_TO_BE_16_BIT, // MVM_NATIVE_POINTER_IS_16_BIT is 1 but pointer size is not 16-bit
  /* 39 */ MVM_E_EXPECTED_POINTER_SIZE_NOT_TO_BE_16_BIT, // MVM_NATIVE_POINTER_IS_16_BIT is 0 but pointer size is 16-bit
  /* 40 */ MVM_E_TYPE_ERROR_TARGET_IS_NOT_CALLABLE, // The script tried to call something that wasn't a function
  /* 41 */ MVM_E_TDZ_ERROR, // The script tried to access a local variable before its declaration
  /* 42 */ MVM_E_MALLOC_NOT_WITHIN_RAM_PAGE, // See instructions in example port file at the defitions MVM_USE_SINGLE_RAM_PAGE and MVM_RAM_PAGE_ADDR
  /* 43 */ MVM_E_INVALID_ARRAY_INDEX, // Array indexes must be integers in the range 0 to 8191
  /* 44 */ MVM_E_UNCAUGHT_EXCEPTION, // The script threw an exception with `throw` that was wasn't caught before returning to the host
  /* 45 */ MVM_E_FATAL_ERROR_MUST_KILL_VM, // Please make sure that MVM_FATAL_ERROR does not return, or bad things can happen. (Kill the process, the thread, or use longjmp)
  /* 46 */ MVM_E_OBJECT_KEYS_ON_NON_OBJECT, // Can only use Reflect.ownKeys on plain objects (not functions, arrays, or other values)
  /* 47 */ MVM_E_INVALID_UINT8_ARRAY_LENGTH, // Either non-numeric or out-of-range argument for creating a Uint8Array
  /* 48 */ MVM_E_CAN_ONLY_ASSIGN_BYTES_TO_UINT8_ARRAY, // Value assigned to index of Uint8Array must be an integer in the range 0 to 255
  /* 49 */ MVM_E_WRONG_BYTECODE_VERSION, // The version of bytecode is different to what the engine supports
  /* 50 */ MVM_E_USING_NEW_ON_NON_CLASS, // The `new` operator can only be used on classes
  /* 51 */ MVM_E_INSTRUCTION_COUNT_REACHED, // The instruction count set by `mvm_stopAfterNInstructions` has been reached
  /* 52 */ MVM_E_REQUIRES_ACTIVE_VM, // The given operation requires that the VM has active calls on the stack
  /* 53 */ MVM_E_ASYNC_START_ERROR, // mvm_asyncStart must be called exactly once at the beginning of a host function that is called from JS
  /* 54 */ MVM_E_ASYNC_WITHOUT_AWAIT, // mvm_asyncStart can only be used with a script that has await points. Add at least one (reachable) await point to the script.
  /* 55 */ MVM_E_TYPE_ERROR_AWAIT_NON_PROMISE, // Can only await a promise in Microvium
  /* 56 */ MVM_E_HEAP_CORRUPT, // Microvium's internal heap is not in a consistent state
  /* 57 */ MVM_E_CLASS_PROTOTYPE_MUST_BE_NULL_OR_OBJECT, // The prototype property of a class must be null or a plain object
} mvm_TeError;

typedef enum mvm_TeType {
  VM_T_UNDEFINED   = 0,
  VM_T_NULL        = 1,
  VM_T_BOOLEAN     = 2,
  VM_T_NUMBER      = 3,
  VM_T_STRING      = 4,
  VM_T_FUNCTION    = 5,
  VM_T_OBJECT      = 6,
  VM_T_ARRAY       = 7,
  VM_T_UINT8_ARRAY = 8,
  VM_T_CLASS       = 9,
  VM_T_SYMBOL      = 10, // Reserved
  VM_T_BIG_INT     = 11, // Reserved

  VM_T_END,
} mvm_TeType;

// Prefix to attach to exported microvium API functions. If a user doesn't
// specify this, we just set it up as the empty macro.
#ifndef MVM_EXPORT
#define MVM_EXPORT
#endif

#ifndef MVM_SUPPORT_FLOAT
#define MVM_SUPPORT_FLOAT 1
#endif

#ifndef MVM_FLOAT64
#define MVM_FLOAT64 double
#endif

#ifndef MVM_INCLUDE_DEBUG_CAPABILITY
#define MVM_INCLUDE_DEBUG_CAPABILITY 1
#endif

typedef struct mvm_VM mvm_VM;

typedef mvm_TeError (*mvm_TfHostFunction)(mvm_VM* vm, mvm_HostFunctionID hostFunctionID, mvm_Value* result, mvm_Value* args, uint8_t argCount);

typedef mvm_TeError (*mvm_TfResolveImport)(mvm_HostFunctionID hostFunctionID, void* context, mvm_TfHostFunction* out_hostFunction);

typedef void (*mvm_TfBreakpointCallback)(mvm_VM* vm, uint16_t bytecodeAddress);

typedef struct mvm_TsMemoryStats {
  // Total RAM currently allocated by the VM from the host
  size_t totalSize;

  // Number of distinct, currently-allocated memory allocations (mallocs) from the host
  size_t fragmentCount;

  // RAM size of VM core state
  size_t coreSize;

  // RAM allocated to the VM import table (table of functions resolved from the host)
  size_t importTableSize;

  // RAM allocated to global variables in RAM
  size_t globalVariablesSize;

  // If the machine registers are allocated (if a call is active), this says how
  // much RAM these consume. Otherwise zero if there is no active stack.
  size_t registersSize;

  // Virtual stack size (bytes) currently allocated (if a call is active), or
  // zero if there is no active stack. Note that virtual stack space is
  // malloc'd, not allocated on the C stack.
  size_t stackHeight;

  // Virtual stack space capacity if a call is active, otherwise zero.
  size_t stackAllocatedCapacity;

  // Maximum stack size over the lifetime of the VM. This value can be used to
  // tune the MVM_STACK_SIZE port definition
  size_t stackHighWaterMark;

  // Amount of virtual heap that the VM is currently using
  size_t virtualHeapUsed;

  // Maximum amount of virtual heap space ever used by this VM
  size_t virtualHeapHighWaterMark;

  // Current total size of virtual heap (will expand as needed up to a max of MVM_MAX_HEAP_SIZE)
  size_t virtualHeapAllocatedCapacity;

} mvm_TsMemoryStats;

/**
 * A handle holds a value that must not be garbage collected.
 *
 * Maintainer note: `_value` is the first field so that a `mvm_Handle*` is also
 * a `mvm_Value*`, which allows some internal functions to be polymorphic in
 * whether they accept handles or just plain value pointers.
 */
typedef struct mvm_Handle { mvm_Value _value; struct mvm_Handle* _next; } mvm_Handle;

#include "microvium_port.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Creates a VM (restores the state of a virtual machine from a snapshot)
 *
 * WARNING: The snapshot bytecode is not copied by Microvium, so it needs to
 * stay in memory (or flash).
 *
 * A VM created with mvm_restore needs to be freed with mvm_free.
 *
 * Note: the bytecode should be aligned to the processor word size.
 *
 * Note: the bytecode needs to live as long as the VM.
 *
 * @param resolveImport A callback function that the VM will call when it needs
 * to import a host function.
 * @param context Any value. The context for a VM can be retrieved later using
 * `mvm_getContext`. It can be used to attach user-defined data to a VM.
 */
MVM_EXPORT mvm_TeError mvm_restore(mvm_VM** result, MVM_LONG_PTR_TYPE snapshotBytecode, size_t bytecodeSize, void* context, mvm_TfResolveImport resolveImport);

/**
 * Free all memory associated with a VM. The VM must not be used again after freeing.
 */
MVM_EXPORT void mvm_free(mvm_VM* vm);

/**
 * Call a function in the VM
 *
 * @param func The function value to call
 * @param out_result Where to put the result, or NULL if the result is not
 * needed
 * @param args Pointer to arguments array, or NULL if no arguments
 * @param argCount Number of arguments
 *
 * If the JS code throws an exception, the return value will be
 * MVM_E_UNCAUGHT_EXCEPTION and the exception value will be put into
 * `out_result`
 */
MVM_EXPORT mvm_TeError mvm_call(mvm_VM* vm, mvm_Value func, mvm_Value* out_result, mvm_Value* args, uint8_t argCount);

MVM_EXPORT void* mvm_getContext(mvm_VM* vm);

/**
 * Handle operations. Handles are used to hold values that must not be garbage
 * collected. See `doc\handles-and-garbage-collection.md` for more information.
 */
MVM_EXPORT void mvm_initializeHandle(mvm_VM* vm, mvm_Handle* handle); // Handle must be released by mvm_releaseHandle
MVM_EXPORT void mvm_cloneHandle(mvm_VM* vm, mvm_Handle* target, const mvm_Handle* source); // Target must be released by mvm_releaseHandle
MVM_EXPORT mvm_TeError mvm_releaseHandle(mvm_VM* vm, mvm_Handle* handle);
static inline mvm_Value mvm_handleGet(const mvm_Handle* handle) { return handle->_value; }
static inline mvm_Value* mvm_handleAt(mvm_Handle* handle) { return &handle->_value; }
static inline void mvm_handleSet(mvm_Handle* handle, mvm_Value value) { handle->_value = value; }

/**
 * Roughly like the `typeof` operator in JS, except with distinct values for
 * null and arrays
 */
MVM_EXPORT mvm_TeType mvm_typeOf(mvm_VM* vm, mvm_Value value);

/**
 * Converts the value to a string encoded as UTF-8.
 *
 * @param out_sizeBytes Returns the length of the string in bytes, or provide NULL if not needed.
 * @return A pointer to the string data which may be in VM memory or bytecode.
 *
 * Note: for convenience, the returned data has an extra null character appended
 * to the end of it, so that the result is directly usable in printf, strcpy,
 * etc. The returned size in bytes is the size of the original string data,
 * excluding the extra null.
 *
 * The string data itself is permitted to contain nulls or any other data. For
 * example, if the string value is "abc\0", the size returned is "4", and the
 * returned pointer points to the data "abc\0\0" (i.e. with the extra safety
 * null beyond the user-provided data).
 *
 * The memory pointed to by the return value may be transient: it is only guaranteed
 * to exist until the next garbage collection cycle. See
 * [memory-management.md](https://github.com/coder-mike/microvium/blob/master/doc/native-vm/memory-management.md)
 * for details.
 */
MVM_EXPORT const char* mvm_toStringUtf8(mvm_VM* vm, mvm_Value value, size_t* out_sizeBytes);

/**
 * Returns the length of a string as it appears in bytes when encoded as UTF-8
 * (which is also the internal representation of Microvium).
 */
MVM_EXPORT size_t mvm_stringSizeUtf8(mvm_VM* vm, mvm_Value value);

/**
 * Convert the value to a bool based on its truthiness.
 *
 * See https://developer.mozilla.org/en-US/docs/Glossary/Truthy
 */
MVM_EXPORT bool mvm_toBool(mvm_VM* vm, mvm_Value value);

/**
 * Converts the value to a 32-bit signed integer.
 *
 * The result of this should be the same as `value|0` in JavaScript code.
 */
MVM_EXPORT int32_t mvm_toInt32(mvm_VM* vm, mvm_Value value);

#if MVM_SUPPORT_FLOAT
/**
 * Converts the value to a number.
 *
 * The result of this should be the same as `+value` in JavaScript code.
 *
 * For efficiency, use mvm_toInt32 instead if your value is an integer.
 */
MVM_EXPORT MVM_FLOAT64 mvm_toFloat64(mvm_VM* vm, mvm_Value value);

/**
 * Create a JavaScript number value in the VM.
 *
 * WARNING: the result is eligible for garbage collection the next time the VM
 * has control. See `doc\handles-and-garbage-collection.md` for more information.
 *
 * For efficiency, use mvm_newInt32 instead if your value is an integer.
 *
 * Design note: mvm_newNumber creates a number *from* a float64, so it's named
 * `newNumber` and not `newFloat64`
 */
MVM_EXPORT mvm_Value mvm_newNumber(mvm_VM* vm, MVM_FLOAT64 value);
#endif

MVM_EXPORT bool mvm_isNaN(mvm_Value value);

extern const mvm_Value mvm_undefined;
extern const mvm_Value mvm_null;

/**
 * Create a JavaScript boolean value in the VM.
 */
MVM_EXPORT mvm_Value mvm_newBoolean(bool value);

/**
 * Create a JavaScript number from a 32-bit integer.
 *
 * WARNING: the result is eligible for garbage collection the next time the VM
 * has control. See `doc\handles-and-garbage-collection.md` for more information.
 */
MVM_EXPORT mvm_Value mvm_newInt32(mvm_VM* vm, int32_t value);

/**
 * Create a new string in Microvium memory.
 *
 * WARNING: the result is eligible for garbage collection the next time the VM
 * has control. See `doc\handles-and-garbage-collection.md` for more information.
 *
 * @param valueUtf8 The a pointer to the string content.
 * @param sizeBytes The size in bytes of the string, excluding any null terminator.
 */
MVM_EXPORT mvm_Value mvm_newString(mvm_VM* vm, const char* valueUtf8, size_t sizeBytes);

/**
 * A Uint8Array in Microvium is an efficient buffer of bytes. It is mutable but
 * cannot be resized. The new Uint8Array created by this method will contain a
 * *copy* of the supplied data.
 *
 * WARNING: the result is eligible for garbage collection the next time the VM
 * has control. See `doc\handles-and-garbage-collection.md` for more information.
 *
 * Within the VM, you can create a new Uint8Array using the global
 * `Microvium.newUint8Array`.
 *
 * See also: mvm_uint8ArrayToBytes
 */
MVM_EXPORT mvm_Value mvm_uint8ArrayFromBytes(mvm_VM* vm, const uint8_t* data, size_t size);

/**
 * Given a Uint8Array, this will give a pointer to its data and its size (in
 * bytes).
 *
 * Warning: The data pointer should be considered invalid on the next call to
 * any of the Microvium API methods, since a garbage can move the data. It is
 * recommended to call this method again each time you need the pointer.
 *
 * The returned pointer can also be used to mutate the buffer, with caution.
 *
 * See also: mvm_uint8ArrayFromBytes
 */
MVM_EXPORT mvm_TeError mvm_uint8ArrayToBytes(mvm_VM* vm, mvm_Value uint8ArrayValue, uint8_t** out_data, size_t* out_size);

/**
 * Resolves (finds) the values exported by the VM, identified by ID.
 *
 * @param ids An array of `count` IDs to look up.
 * @param results An array of `count` output values that result from each
 * lookup
 *
 * Note: Exports are immutable (shallow immutable), so they don't need to be
 * captured by a mvm_Handle. In typical usage, exports will each be function
 * values, but any value type is valid.
 */
MVM_EXPORT mvm_TeError mvm_resolveExports(mvm_VM* vm, const mvm_VMExportID* ids, mvm_Value* results, uint8_t count);

/**
 * Run a garbage collection cycle.
 *
 * If `squeeze` is `true`, the GC runs in 2 passes: the first pass computes the
 * exact required size, and the second pass compacts into exactly that size.
 *
 * If `squeeze` is `false`, the GC runs in a single pass, estimating the amount
 * of needed as the amount of space used after the last compaction, and then
 * adding blocks as-necessary.
 */
MVM_EXPORT void mvm_runGC(mvm_VM* vm, bool squeeze);

/**
 * Compares two values for equality. The same semantics as JavaScript `===`
 */
MVM_EXPORT bool mvm_equal(mvm_VM* vm, mvm_Value a, mvm_Value b);

/**
 * The current bytecode address being executed (relative to the beginning of the
 * bytecode image), or null if the machine is not currently active.
 *
 * This value can be looked up in the map file generated by the CLI flag
 * `--map-file`
 */
MVM_EXPORT uint16_t mvm_getCurrentAddress(mvm_VM* vm);

/**
 * Get stats about the VM memory
 */
MVM_EXPORT void mvm_getMemoryStats(mvm_VM* vm, mvm_TsMemoryStats* out_stats);


/**
 * Call this at the beginning of an asynchronous host function. It accepts a
 * pointer to the synchronous result and returns a callback function that can be
 * used to set the asynchronous result.
 *
 * @param out_result The result pointer that was passed to the host function.
 *                   mvm_asyncStart will set the result. The host must not set
 *                   or use the result field.
 *
 * @returns A JavaScript callback function accepting arguments (isSuccess,
 * value)
 *
 * This function sets the synchronous result `*out_result` to a promise object
 * (or this promise value may be optimized away in certain cases), and returns a
 * JavaScript callback function that represents the caller continuation.
 *
 * If the asynchronous operation ends successfully, call the callback with
 * arguments (true, result). If the asynchronous operation fails, call the
 * callback with arguments (false, error).
 *
 * Note: The callback will not invoke the continuation immediately but will
 * schedule it on Microvium's job queue.
 *
 * Note: mvm_asyncStart does not do anything regarding threading. It's up to the
 * host to promptly return from the host function and to call the callback
 * later.
 *
 * @warning The returned mvm_Value is subject to garbage collection and the host
 * should keep it in a handle until it's ready to call it.
 *
 * @warning The host must call `mvm_asyncStart` right at the beginning of the
 * host function, before doing anything else, since this accesses an internal
 * register that is not preserved across function calls.
 */
mvm_Value mvm_asyncStart(mvm_VM* vm, mvm_Value* out_result);


#if MVM_INCLUDE_SNAPSHOT_CAPABILITY
/**
 * Create a snapshot of the VM
 *
 * @param vm The virtual machine to snapshot.
 * @param out_size Pointer to variable which will receive the size of the
 * generated snapshot
 *
 * The snapshot generated by this function is suitable to be used in a call to
 * mvm_restore to be restored later.
 *
 * It's recommended to run a garbage collection cycle (mvm_runGC) before
 * creating the snapshot, to get as compact a snapshot as possible.
 *
 * No snapshots ever contain the stack or register states -- they only encode
 * the heap and global variable states.
 *
 * Note: The result is malloc'd on the host heap, and so needs to be freed with
 * a call to *free*.
 */
MVM_EXPORT void* mvm_createSnapshot(mvm_VM* vm, size_t* out_size);
#endif // MVM_INCLUDE_SNAPSHOT_CAPABILITY

#if MVM_INCLUDE_DEBUG_CAPABILITY
/**
 * Set a breakpoint on the given bytecode address.
 *
 * Use (-1) to break on every instruction.
 *
 * Whenever the VM executes the instruction at the given bytecode address, the
 * VM will invoke the breakpoint callback (see mvm_dbg_setBreakpointCallback).
 *
 * The given bytecode address is measured from the beginning of the given
 * bytecode image (passed to mvm_restore). The address point exactly to the
 * beginning of a bytecode instruction (addresses corresponding to the middle of
 * a multi-byte instruction are ignored).
 *
 * The breakpoint remains registered/active until mvm_dbg_removeBreakpoint is
 * called with the exact same bytecode address.
 *
 * Setting a breakpoint a second time on the same address of an existing active
 * breakpoint will have no effect.
 */
MVM_EXPORT void mvm_dbg_setBreakpoint(mvm_VM* vm, int bytecodeAddress);

/**
 * Remove a breakpoint added by mvm_dbg_setBreakpoint
 */
MVM_EXPORT void mvm_dbg_removeBreakpoint(mvm_VM* vm, uint16_t bytecodeAddress);

/**
 * Set the function to be called when any breakpoint is hit.
 *
 * The callback only applies to the given virtual machine (the callback can be
 * different for different VMs).
 *
 * The callback is invoked with the bytecode address corresponding to where the
 * VM is stopped. The VM will continue execution when the breakpoint callback
 * returns. To suspend the VM indefinitely, the callback needs to
 * correspondingly block indefinitely.
 *
 * It's possible but not recommended for the callback itself call into the VM
 * again (mvm_call), causing control to re-enter the VM while the breakpoint is
 * still active. This should *NOT* be used to continue execution, but could
 * theoretically be used to evaluate debug watch expressions.
 */
MVM_EXPORT void mvm_dbg_setBreakpointCallback(mvm_VM* vm, mvm_TfBreakpointCallback cb);
#endif // MVM_INCLUDE_DEBUG_CAPABILITY

#ifdef MVM_GAS_COUNTER
/**
 * mvm_stopAfterNInstructions
 *
 * Sets the VM to stop (return error MVM_E_INSTRUCTION_COUNT_REACHED) after n
 * further bytecode instructions have been executed. This may help the host to
 * catch run-away VMs or infinite loops. Use n = -1 to disable the limit.
 *
 * If `n` is zero, the VM will stop before executing any further instructions.
 *
 * When the VM reaches the stopped state, further calls to the VM will fail with
 * the same error until `mvm_stopAfterNInstructions` is called again to reset
 * the countdown.
 *
 * The stopping unwinds the current call stack in a similar way to an exception,
 * except that it will not hit any catch blocks. However, be aware that if the
 * call to the VM is reentrant (e.g. the host calls the VM which calls the host
 * which calls the VM again), the stopping will only unwind the innermost call
 * stack. The outer call stack will then unwind if the inner host function
 * returns an error code (e.g. propagating the MVM_E_INSTRUCTION_COUNT_REACHED)
 * or simply does not reset the countdown, so that the VM will fail again when
 * the host returns control to the VM.
 */
MVM_EXPORT void mvm_stopAfterNInstructions(mvm_VM* vm, int32_t n);

/**
 * mvm_getInstructionCountRemaining
 *
 * If `mvm_stopAfterNInstructions` has been used to set a limit on the number of
 * instructions to execute, this function can be used to see the remaining
 * number of instructions before the VM will stop.
 *
 * The return value will be negative if the countdown is currently disabled.
 */
MVM_EXPORT int32_t mvm_getInstructionCountRemaining(mvm_VM* vm);
#endif // MVM_GAS_COUNTER

#ifdef __cplusplus
} // extern "C"
#endif
