// Copyright 2020 Michael Hunter. Part of the Microvium project. Links to full code at https://microvium.com for license details.

/*
 * Microvium Bytecode Interpreter
 *
 * Version: 8.0.0
 *
 * This file contains the Microvium virtual machine C implementation.
 *
 * The key functions are mvm_restore() and mvm_call(), which perform the
 * initialization and run loop respectively.
 *
 * I've written Microvium in C because lots of embedded projects for small
 * processors are written in pure C, and so integration for them will be easier.
 * Also, there are a surprising number of C++ compilers in the embedded world
 * that deviate from the standard, and I don't want to be testing on all of them
 * individually.
 *
 * For the moment, I'm keeping Microvium all in one file for usability. Users
 * can treat this file as a black box that contains the VM, and there's only one
 * file they need to have built into their project in order to have Microvium
 * running. The build process also pulls in the dependent header files, so
 * there's only one header file and it's the one that users of Microvium need to
 * see. Certain compilers and optimization settings also do a better job when
 * related functions are co-located the same compilation unit.
 *
 * User-facing functions and definitions are all prefixed with `mvm_` to
 * namespace them separately from other functions in their project, some of
 * which use the prefix `vm_` and some without a prefix. (TODO: this should be
 * consolidated)
 */

#include "microvium.h"

#include <ctype.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h> // Note: only uses snprintf from stdio.h

// See microvium.c for design notes.


#include "stdbool.h"
#include "stdint.h"
#include "string.h"
#include "stdlib.h"

#include "microvium.h"
#include "microvium_port.h"


#include "stdint.h"

// These sections appear in the bytecode in the order they appear in this
// enumeration.
typedef enum mvm_TeBytecodeSection {
  /**
   * Import Table
   *
   * List of host function IDs (vm_TsImportTableEntry) which are called by the
   * VM. References from the VM to host functions are represented as indexes
   * into this table. These IDs are resolved to their corresponding host
   * function pointers when a VM is restored.
   */
  BCS_IMPORT_TABLE,

  /**
   * A list of immutable `vm_TsExportTableEntry` that the VM exports, mapping
   * export IDs to their corresponding VM Value. Mostly these values will just
   * be function pointers.
   */
  // TODO: We need to test what happens if we export numbers and objects
  BCS_EXPORT_TABLE,

  /**
   * Short Call Table. Table of vm_TsShortCallTableEntry.
   *
   * To make the representation of function calls in IL more compact, up to 256
   * of the most frequent function calls are listed in this table, including the
   * function target and the argument count.
   *
   * See `LBL_CALL_SHORT`
   */
  BCS_SHORT_CALL_TABLE,

  /**
   * Builtins
   *
   * See `mvm_TeBuiltins`
   *
   * Table of `Value`s that need to be directly identifiable by the engine, such
   * as the Array prototype.
   *
   * These are not copied into RAM, they are just constant values like the
   * exports, but like other values in ROM they are permitted to hold mutable
   * values by pointing (as BytecodeMappedPtr) to the corresponding global
   * variable slot.
   *
   * Note: at one point, I had these as single-byte offsets into the global
   * variable space, but this made the assumption that all accessible builtins
   * are also mutable, which is probably not true. The new design makes the
   * opposite assumption: most builtins will be immutable at runtime (e.g.
   * nobody changes the array prototype), so they can be stored in ROM and
   * referenced by immutable Value pointers, making them usable but not
   * consuming RAM at all. It's the exception rather than the rule that some of
   * these may be mutable and require indirection through the global slot table.
   */
  BCS_BUILTINS,

  /**
   * Interned Strings Table
   *
   * To keep property lookup efficient, Microvium requires that strings used as
   * property keys can be compared using pointer equality. This requires that
   * there is only one instance of each string (see
   * https://en.wikipedia.org/wiki/String_interning). This table is the
   * alphabetical listing of all the strings in ROM (or at least, all those
   * which are valid property keys). See also TC_REF_INTERNED_STRING.
   *
   * There may be two string tables: one in ROM and one in RAM. The latter is
   * required in general if the program might use arbitrarily-computed strings
   * as property keys. For efficiency, the ROM string table is contiguous and
   * sorted, to allow for binary searching, while the RAM string table is a
   * linked list for efficiency in appending (expected to be used only
   * occasionally).
   */
  BCS_STRING_TABLE,

  /**
   * Functions and other immutable data structures.
   *
   * While the whole bytecode is essentially "ROM", only this ROM section
   * contains addressable allocations.
   */
  BCS_ROM,

  /**
   * Globals
   *
   * One `Value` entry for the initial value of each global variable. The number
   * of global variables is determined by the size of this section.
   *
   * This section will be copied into RAM at startup (restore).
   *
   * Note: the global slots are used both for global variables and for "handles"
   * (these are different to the user-defined handles for referencing VM objects
   * from user space). Handles allow ROM allocations to reference RAM
   * allocations, even though the ROM can't be updated when the RAM allocation
   * moves during a GC collection. A handle is a slot in the "globals" space,
   * where the slot itself is pointed to by a ROM value and it points to the
   * corresponding RAM value. During a GC cycle, the RAM value may move and the
   * handle slot is updated, but the handle slot itself doesn't move. See
   * `offsetToDynamicPtr` in `encode-snapshot.ts`.
   *
   * The handles appear as the *last* global slots, and will generally not be
   * referenced by `LOAD_GLOBAL` instructions.
   *
   * WARNING: the globals section is the last section before the heap, so no ROM
   * pointer should point after the globals section. Some functions (e.g.
   * vm_getHandleTargetOrNull) assume this to be true.
   */
  BCS_GLOBALS,

  /**
   * Heap Section: heap allocations.
   *
   * This section is copied into RAM when the VM is restored. It becomes the
   * initial value of the GC heap. It contains allocations that are mutable
   * (like the DATA section) but also subject to garbage collection.
   *
   * Note: the heap must be at the end, because it is the only part that changes
   * size from one snapshot to the next. There is code that depends on this
   * being the last section because the size of this section is computed as
   * running to the end of the bytecode image.
   */
  BCS_HEAP,

  BCS_SECTION_COUNT,
} mvm_TeBytecodeSection;

typedef enum mvm_TeBuiltins {
  BIN_INTERNED_STRINGS,
  BIN_ARRAY_PROTO,
  BIN_STR_PROTOTYPE, // If the string "prototype" is interned, this builtin points to it.
  BIN_ASYNC_CONTINUE, // A function used to construct a closure for the job queue to complete async operations
  BIN_ASYNC_CATCH_BLOCK, // A block, bundled as a function, for the root try-catch in async functions
  BIN_ASYNC_HOST_CALLBACK, // Bytecode to use as the callback for host async operations
  BIN_PROMISE_PROTOTYPE,

  BIN_BUILTIN_COUNT
} mvm_TeBuiltins;

// Minimal bytecode is 32 bytes (sizeof(mvm_TsBytecodeHeader) + BCS_SECTION_COUNT*2 + BIN_BUILTIN_COUNT*2)
typedef struct mvm_TsBytecodeHeader { // Size = 12B + sectionOffsets
  uint8_t bytecodeVersion; // MVM_ENGINE_MAJOR_VERSION
  uint8_t headerSize;
  uint8_t requiredEngineVersion; // MVM_ENGINE_MINOR_VERSION
  uint8_t reserved; // =0

  uint16_t bytecodeSize; // Including header
  uint16_t crc; // CCITT16 (header and data, of everything after the CRC)

  uint32_t requiredFeatureFlags;

  /*
  Note: the sections are assumed to be in order as per mvm_TeBytecodeSection, so
  that the size of a section can be computed as the difference between the
  adjacent offsets. The last section runs up until the end of the bytecode.
  */
  uint16_t sectionOffsets[BCS_SECTION_COUNT]; // 8 sections, 16B
} mvm_TsBytecodeHeader;

typedef enum mvm_TeFeatureFlags {
  FF_FLOAT_SUPPORT = 0,
} mvm_TeFeatureFlags;

typedef struct vm_TsExportTableEntry {
  mvm_VMExportID exportID;
  mvm_Value exportValue;
} vm_TsExportTableEntry;

typedef struct vm_TsShortCallTableEntry {
  /* Note: the `function` field has been broken up into separate low and high
   * bytes, `functionL` and `functionH` respectively, for alignment purposes,
   * since this is a 3-byte structure occurring in a packed table.
   *
   * `functionL` and `functionH` together make an `mvm_Value` which should be a
   * callable value (a pointer to a `TsBytecodeFunc`, `TsHostFunc`, or
   * `TsClosure`). TODO: I don't think this currently works, and I'm not even
   * sure how we would test it. */
  uint8_t functionL;
  uint8_t functionH;
  uint8_t argCount;
} vm_TsShortCallTableEntry;




/*
Note: the instruction set documentation is in
`microvium/doc/internals/instruction-set`

Microvium categorizes operations into groups based on common features. The first
nibble of an instruction is its vm_TeOpcode. This is followed by 4 bits which
can either be interpreted as a data parameter or as another opcode (e.g.
vm_TeOpcodeEx1). I call the first nibble the "primary opcode" and the second
nibble is the "secondary opcode".

There are a number of possible secondary opcodes, and each group has common
preparation logic across the group. Preparation logic means the code that runs
before the operation. For example, many operations require popping a value off
the stack before operating on the value. The VM implementation is more compact
if the pop code is common to all instructions that do the pop.

Operations can have different "follow through" logic grouped arbitrarily, since
the implementation of all instructions requires a "jump", those that have common
follow through logic simply jump to the same follow through without additional
cost, which eventually lands up back at the loop start. So the instruction
grouping does not need to cater for follow through logic, only preparation
logic.

To keep operation commonality as seamlessly as possible, the VM implementation
use 16-bit "registers", which have overloaded meaning depending on the context:

  - `reg1`
    - Initially holds the zero-extended 4-bit secondary nibble
    - Operations that load an 8- or 16-bit literal will overwrite `reg1` with
      the literal.
    - "Pure" operations use reg1 as the first popped operand (none of the pure
      operations have an embedded literal). "Pure" are what I'm calling
      operations whose entire effect is to pop some operands off the stack,
      operate on them, and push a result back onto the stack. For example,
      `ADD`.
    - `reg1` is also used as the "result" value for the common push-result tail
      logic
  - `reg2`
    - used as the second popped value of binary operations
    - used as the value to store, store-like operations
  - `reg3`
    - can be used arbitrarily by operations and does not have a common meaning

Additionally, the number operations have variations that work on 32 or 64 bit
values. These have their own local/ephemeral registers:

  - `reg1I`: the value of the reg1 register unpacked to a `uint32_t`
  - `reg2I`: the value of the reg2 register unpacked to a `uint32_t`
  - `reg1F`: the value of the reg1 register unpacked to a `double`
  - `reg2F`: the value of the reg2 register unpacked to a `double`

Operation groups and their corresponding preparation logic

  - vm_TeOpcodeEx1:
    - The prep does not read a literal (all these instructions are single-byte).
    - The prep pops 0, 1, or 2 values from the stack depending on the
      instruction range

  - vm_TeOpcodeEx2:
    - Prep reads 8-bit literal into reg1
      - Two separate instruction ranges specify whether to sign extend or not.
    - Two instruction ranges specify whether the prep will also pop an arg into
      reg2.

  - vm_TeOpcodeEx3:
    - Prep reads a 16-bit value from byte stream into reg1. This can be
      interpreted as either signed or unsigned by the particular instruction.
    - A sub-range within the instruction specifies whether an argument is popped
      from the stack.
    - (Edit: there are violations of this pattern because I ran out space in
      vm_TeOpcodeEx1)

  - vm_TeOpcodeEx4:
    - Not really any common logic. Just a bucket of miscellaneous instructions.

  - vm_TeNumberOp:
    - These are all dual-implementation instructions which have both 32 and 64
      bit implementations.
    - Prep pops one or two values off the stack and reads them into reg1 and
      reg2 respectively. The choice of 1 or 2 depends on the sub-range. If
      popping one value, the second is left as zero.
    - Prep unpacks to either int32 or float64 depending on the corresponding
      data types.
    - The operations can dispatch to a different tail/follow through routine
      depending on whether they overflow or not.

  - vm_TeBitwiseOp:
    - These operations all operate on 32-bit integers and produce 32-bit integer
      results.
    - Prep pops one or two values off the stack and reads them into reg1 and
      reg2 respectively. The choice of 1 or 2 depends on the sub-range. If
      popping one value, the second is left as zero.
    - Prep unpacks reg1 and reg2 to int32

Follow-through/tail routines:

  - Push float (reg1F)
  - Push int32 (reg1I)
  - Push 16-bit result (reg1)

*/

// TODO: I think this instruction set needs an overhaul. The categorization has
// become chaotic and not that efficient.

// TODO: If we wanted to make space in the primary opcode range, we could remove
// `VM_OP_LOAD_ARG_1` and just leave `VM_OP2_LOAD_ARG_2`, since static analysis
// should be able to convert many instances of `LoadArg` into `LoadVar`

// 4-bit enum
typedef enum vm_TeOpcode {
  VM_OP_LOAD_SMALL_LITERAL  = 0x0, // (+ 4-bit vm_TeSmallLiteralValue)
  VM_OP_LOAD_VAR_1          = 0x1, // (+ 4-bit variable index relative to stack pointer)
  VM_OP_LOAD_SCOPED_1       = 0x2, // (+ 4-bit scoped variable index)
  VM_OP_LOAD_ARG_1          = 0x3, // (+ 4-bit arg index)
  VM_OP_CALL_1              = 0x4, // (+ 4-bit index into short-call table)
  VM_OP_FIXED_ARRAY_NEW_1   = 0x5, // (+ 4-bit length)
  VM_OP_EXTENDED_1          = 0x6, // (+ 4-bit vm_TeOpcodeEx1)
  VM_OP_EXTENDED_2          = 0x7, // (+ 4-bit vm_TeOpcodeEx2)
  VM_OP_EXTENDED_3          = 0x8, // (+ 4-bit vm_TeOpcodeEx3)
  VM_OP_CALL_5              = 0x9, // (+ 4-bit arg count + 16-bit target)

  VM_OP_DIVIDER_1, // <-- ops after this point pop at least one argument (reg2)

  VM_OP_STORE_VAR_1         = 0xA, // (+ 4-bit variable index relative to stack pointer)
  VM_OP_STORE_SCOPED_1      = 0xB, // (+ 4-bit scoped variable index)
  VM_OP_ARRAY_GET_1         = 0xC, // (+ 4-bit item index)
  VM_OP_ARRAY_SET_1         = 0xD, // (+ 4-bit item index)
  VM_OP_NUM_OP              = 0xE, // (+ 4-bit vm_TeNumberOp)
  VM_OP_BIT_OP              = 0xF, // (+ 4-bit vm_TeBitwiseOp)

  VM_OP_END
} vm_TeOpcode;

typedef enum vm_TeOpcodeEx1 {
  VM_OP1_RETURN                  = 0x0,
  VM_OP1_THROW                   = 0x1,

  // (target) -> TsClosure
  VM_OP1_CLOSURE_NEW             = 0x2,

  // (TsClass, ...args) -> object
  VM_OP1_NEW                     = 0x3, // (+ 8-bit unsigned arg count. Target is dynamic)

  VM_OP1_RESERVED_VIRTUAL_NEW    = 0x4,

  VM_OP1_SCOPE_NEW               = 0x5, // (+ 8-bit variable count)

  // (value) -> mvm_TeType
  VM_OP1_TYPE_CODE_OF            = 0x6, // More efficient than VM_OP1_TYPEOF

  VM_OP1_POP                     = 0x7, // Pop one item

  VM_OP1_TYPEOF                  = 0x8,

  VM_OP1_OBJECT_NEW              = 0x9,

  // boolean -> boolean
  VM_OP1_LOGICAL_NOT             = 0xA,

  VM_OP1_DIVIDER_1, // <-- ops after this point are treated as having at least 2 stack arguments

  // (object, prop) -> any
  VM_OP1_OBJECT_GET_1            = 0xB, // (field ID is dynamic)

  // (string, string) -> string
  // (number, number) -> number
  VM_OP1_ADD                     = 0xC,

  // (any, any) -> boolean
  VM_OP1_EQUAL                   = 0xD,
  VM_OP1_NOT_EQUAL               = 0xE,

  // (object, prop, any) -> void
  VM_OP1_OBJECT_SET_1            = 0xF, // (field ID is dynamic)

  VM_OP1_END
} vm_TeOpcodeEx1;

// All of these operations are implemented with an 8-bit literal embedded into
// the instruction. The literal is stored in reg1.
typedef enum vm_TeOpcodeEx2 {
  VM_OP2_BRANCH_1            = 0x0, // (+ 8-bit signed offset)

  VM_OP2_STORE_ARG           = 0x1, // (+ 8-bit unsigned arg index)
  VM_OP2_STORE_SCOPED_2      = 0x2, // (+ 8-bit unsigned scoped variable index)
  VM_OP2_STORE_VAR_2         = 0x3, // (+ 8-bit unsigned variable index relative to stack pointer)
  VM_OP2_ARRAY_GET_2_RESERVED = 0x4, // (+ 8-bit unsigned field index)
  VM_OP2_ARRAY_SET_2_RESERVED = 0x5, // (+ 8-bit unsigned field index)

  VM_OP2_DIVIDER_1, // <-- ops before this point pop from the stack into reg2

  VM_OP2_JUMP_1              = 0x6, // (+ 8-bit signed offset)
  VM_OP2_CALL_HOST           = 0x7, // (+ 8-bit arg count + 8-bit unsigned index into resolvedImports)
  VM_OP2_CALL_3              = 0x8, // (+ 8-bit unsigned arg count. Target is dynamic)
  VM_OP2_CALL_6              = 0x9, // (+ 8-bit index into short-call table)

  VM_OP2_LOAD_SCOPED_2       = 0xA, // (+ 8-bit unsigned scoped variable index)
  VM_OP2_LOAD_VAR_2          = 0xB, // (+ 8-bit unsigned variable index relative to stack pointer)
  VM_OP2_LOAD_ARG_2          = 0xC, // (+ 8-bit unsigned arg index)

  VM_OP2_EXTENDED_4          = 0xD, // (+ 8-bit unsigned vm_TeOpcodeEx4)

  VM_OP2_ARRAY_NEW           = 0xE, // (+ 8-bit capacity count)
  VM_OP2_FIXED_ARRAY_NEW_2   = 0xF, // (+ 8-bit length count)

  VM_OP2_END
} vm_TeOpcodeEx2;

// Most of these instructions all have an embedded 16-bit literal value
typedef enum vm_TeOpcodeEx3 {
  // Note: Pop[0] can be used as a single-byte NOP instruction
  VM_OP3_POP_N               = 0x0, // (+ 8-bit pop count) Pops N items off the stack
  VM_OP3_SCOPE_DISCARD       = 0x1, // Set the closure reg to undefined
  VM_OP3_SCOPE_CLONE         = 0x2,
  VM_OP3_AWAIT               = 0x3, // (no literal operands)
  VM_OP3_AWAIT_CALL          = 0x4, // (+ 8-bit arg count)
  VM_OP3_ASYNC_RESUME        = 0x5, // (+ 8-bit stack restoration slot count)

  VM_OP3_RESERVED_3          = 0x6,

  VM_OP3_DIVIDER_1, // <-- ops before this point are miscellaneous and don't automatically get any literal values or stack values

  VM_OP3_JUMP_2              = 0x7, // (+ 16-bit signed offset)
  VM_OP3_LOAD_LITERAL        = 0x8, // (+ 16-bit value)
  VM_OP3_LOAD_GLOBAL_3       = 0x9, // (+ 16-bit global variable index)
  VM_OP3_LOAD_SCOPED_3       = 0xA, // (+ 16-bit scoped variable index)

  VM_OP3_DIVIDER_2, // <-- ops after this point pop an argument into reg2

  VM_OP3_BRANCH_2            = 0xB, // (+ 16-bit signed offset)
  VM_OP3_STORE_GLOBAL_3      = 0xC, // (+ 16-bit global variable index)
  VM_OP3_STORE_SCOPED_3      = 0xD, // (+ 16-bit scoped variable index)

  VM_OP3_OBJECT_GET_2        = 0xE, // (+ 16-bit property key)
  VM_OP3_OBJECT_SET_2        = 0xF, // (+ 16-bit property key)

  VM_OP3_END
} vm_TeOpcodeEx3;

// This is a bucket of less frequently used instructions that didn't fit into
// the other opcode ranges. We can put up to 256 opcodes here.
typedef enum vm_TeOpcodeEx4 {
  VM_OP4_START_TRY           = 0x00, // (+ 16-bit label to the catch block)
  VM_OP4_END_TRY             = 0x01, // (No literal operands)
  VM_OP4_OBJECT_KEYS         = 0x02, // (No literal operands)
  VM_OP4_UINT8_ARRAY_NEW     = 0x03, // (No literal operands)

  // (constructor, props) -> TsClass
  VM_OP4_CLASS_CREATE        = 0x04, // Creates TsClass (does not in instantiate a class)

  VM_OP4_TYPE_CODE_OF        = 0x05, // Opcode for mvm_typeOf

  VM_OP4_LOAD_REG_CLOSURE    = 0x06, // (No literal operands)

  VM_OP4_SCOPE_PUSH          = 0x07, // (+ 8-bit unsigned slot count) also sets last slot to parent scope
  VM_OP4_SCOPE_POP           = 0x08, // Sets the closure reg to the parent of the current closure
  VM_OP4_SCOPE_SAVE          = 0x09, // Pops the current closure off the closure stack and pushes it to the variable stack

  VM_OP4_ASYNC_START         = 0x0A, // + 7-bit closure slot count and 1-bit flag for parent-capturing.
  VM_OP4_ASYNC_RETURN        = 0x0B, // (No literal operands)
  VM_OP4_ENQUEUE_JOB         = 0x0C, // (No literal operands)
  VM_OP4_ASYNC_COMPLETE      = 0x0D, // (No literal operands)


  VM_OP4_END
} vm_TeOpcodeEx4;


// Number operations. These are operations which take one or two arguments from
// the stack and coerce them to numbers. Each of these will have two
// implementations: one for 32-bit int, and one for 64-bit float.
typedef enum vm_TeNumberOp {

  // (number, number) -> boolean
  VM_NUM_OP_LESS_THAN        = 0x0,
  VM_NUM_OP_GREATER_THAN     = 0x1,
  VM_NUM_OP_LESS_EQUAL       = 0x2,
  VM_NUM_OP_GREATER_EQUAL    = 0x3,

  // (number, number) -> number
  VM_NUM_OP_ADD_NUM          = 0x4,
  VM_NUM_OP_SUBTRACT         = 0x5,
  VM_NUM_OP_MULTIPLY         = 0x6,
  VM_NUM_OP_DIVIDE           = 0x7,
  VM_NUM_OP_DIVIDE_AND_TRUNC = 0x8, // Represented in JS as `x / y | 0`
  VM_NUM_OP_REMAINDER        = 0x9,
  VM_NUM_OP_POWER            = 0xA,

  VM_NUM_OP_DIVIDER, // <-- ops after this point are unary

  // number -> number
  VM_NUM_OP_NEGATE           = 0xB,
  VM_NUM_OP_UNARY_PLUS       = 0xC,

  VM_NUM_OP_END
} vm_TeNumberOp;

// Bitwise operations:
typedef enum vm_TeBitwiseOp {
  // (bits, bits) -> bits
  VM_BIT_OP_SHR_ARITHMETIC = 0x0, // Aka signed shift right. Aka sign-propagating right shift.
  VM_BIT_OP_SHR_LOGICAL    = 0x1, // Aka unsigned shift right. Aka zero-fill right shift.
  VM_BIT_OP_SHL            = 0x2, // Shift left

  VM_BIT_OP_END_OF_SHIFT_OPERATORS, // <-- ops before this point need their operand in the 0-32 range

  VM_BIT_OP_OR             = 0x3,
  VM_BIT_OP_AND            = 0x4,
  VM_BIT_OP_XOR            = 0x5,

  VM_BIT_OP_DIVIDER_2, // <-- ops after this point are unary

  // bits -> bits
  VM_BIT_OP_NOT            = 0x6,

  VM_BIT_OP_END
} vm_TeBitwiseOp;

// vm_TeSmallLiteralValue : 4-bit enum
//
// Note: Only up to 16 values are allowed here.
typedef enum vm_TeSmallLiteralValue {
  VM_SLV_DELETED         = 0x0,
  VM_SLV_UNDEFINED       = 0x1,
  VM_SLV_NULL            = 0x2,
  VM_SLV_FALSE           = 0x3,
  VM_SLV_TRUE            = 0x4,
  VM_SLV_INT_MINUS_1     = 0x5,
  VM_SLV_INT_0           = 0x6,
  VM_SLV_INT_1           = 0x7,
  VM_SLV_INT_2           = 0x8,
  VM_SLV_INT_3           = 0x9,
  VM_SLV_INT_4           = 0xA,
  VM_SLV_INT_5           = 0xB,
} vm_TeSmallLiteralValue;



#define MVM_EXPECTED_PORT_FILE_VERSION 1

// -------------------------- Port-file defaults -----------------------------


#ifndef MVM_PORT_VERSION
#define MVM_STACK_SIZE 256
#endif

#ifndef MVM_ALLOCATION_BUCKET_SIZE
#define MVM_ALLOCATION_BUCKET_SIZE 256
#endif

#ifndef MVM_MAX_HEAP_SIZE
#define MVM_MAX_HEAP_SIZE 1024
#endif

#ifndef MVM_NATIVE_POINTER_IS_16_BIT
#define MVM_NATIVE_POINTER_IS_16_BIT 0
#endif

#ifndef MVM_FLOAT64_NAN
#define MVM_FLOAT64_NAN ((MVM_FLOAT64)(INFINITY * 0.0))
#endif

#ifndef MVM_SAFE_MODE
#define MVM_SAFE_MODE 1
#endif

#ifndef MVM_DONT_TRUST_BYTECODE
#define MVM_DONT_TRUST_BYTECODE 1
#endif

#ifndef MVM_VERY_EXPENSIVE_MEMORY_CHECKS
#define MVM_VERY_EXPENSIVE_MEMORY_CHECKS 0
#endif

#ifndef MVM_LONG_PTR_NEW
#define MVM_LONG_PTR_NEW(p) ((MVM_LONG_PTR_TYPE)p)
#endif

#ifndef MVM_LONG_PTR_TRUNCATE
#define MVM_LONG_PTR_TRUNCATE(p) ((void*)p)
#endif

#ifndef MVM_LONG_PTR_ADD
#define MVM_LONG_PTR_ADD(p, s) ((MVM_LONG_PTR_TYPE)((uint8_t*)p + (intptr_t)s))
#endif

#ifndef MVM_LONG_PTR_SUB
#define MVM_LONG_PTR_SUB(p2, p1) ((int16_t)((uint8_t*)p2 - (uint8_t*)p1))
#endif

#ifndef MVM_READ_LONG_PTR_1
#define MVM_READ_LONG_PTR_1(lpSource) (*((uint8_t *)lpSource))
#endif
#ifndef MVM_READ_LONG_PTR_2
#define MVM_READ_LONG_PTR_2(lpSource) (*((uint16_t *)lpSource))
#endif

#ifndef MVM_LONG_MEM_CMP
#define MVM_LONG_MEM_CMP(p1, p2, size) memcmp(p1, p2, size)
#endif

#ifndef MVM_LONG_MEM_CPY
#define MVM_LONG_MEM_CPY(target, source, size) memcpy(target, source, size)
#endif

#ifndef MVM_FATAL_ERROR
#include <assert.h>
#define MVM_FATAL_ERROR(vm, e) (assert(false), exit(e))
#endif

#ifndef MVM_ALL_ERRORS_FATAL
#define MVM_ALL_ERRORS_FATAL 0
#endif

#ifndef MVM_SWITCH
#define MVM_SWITCH(tag, upper) switch (tag)
#endif

#ifndef MVM_CASE
#define MVM_CASE(value) case value
#endif

#ifndef MVM_INCLUDE_SNAPSHOT_CAPABILITY
#define MVM_INCLUDE_SNAPSHOT_CAPABILITY 1
#endif



#define MVM_NEED_DEFAULT_CRC_FUNC 0

#if MVM_INCLUDE_SNAPSHOT_CAPABILITY

  #ifndef MVM_CALC_CRC16_CCITT
    #define MVM_CALC_CRC16_CCITT(pData, size) (mvm_default_crc16(pData, size))
    #undef MVM_NEED_DEFAULT_CRC_FUNC
    #define MVM_NEED_DEFAULT_CRC_FUNC 1
  #endif

#endif // MVM_INCLUDE_SNAPSHOT_CAPABILITY

#ifndef MVM_CHECK_CRC16_CCITT
  #define MVM_CHECK_CRC16_CCITT(lpData, size, expected) (mvm_default_crc16(lpData, size) == expected)
  #undef MVM_NEED_DEFAULT_CRC_FUNC
  #define MVM_NEED_DEFAULT_CRC_FUNC 1
#endif

#if MVM_NEED_DEFAULT_CRC_FUNC
  static uint16_t mvm_default_crc16(MVM_LONG_PTR_TYPE lp, uint16_t size) {
    uint16_t r = 0xFFFF;
    while (size--)
    {
      r  = (uint8_t)(r >> 8) | (r << 8);
      r ^= MVM_READ_LONG_PTR_1(lp);
      lp = MVM_LONG_PTR_ADD(lp, 1);
      r ^= (uint8_t)(r & 0xff) >> 4;
      r ^= (r << 8) << 4;
      r ^= ((r & 0xff) << 4) << 1;
    }
    return r;
  }
#endif // MVM_NEED_DEFAULT_CRC_FUNC

#ifndef MVM_USE_SINGLE_RAM_PAGE
#define MVM_USE_SINGLE_RAM_PAGE 0
#endif

#if MVM_USE_SINGLE_RAM_PAGE
  #ifndef MVM_RAM_PAGE_ADDR
  #define MVM_RAM_PAGE_ADDR 0x12340000
  #endif
#endif

#ifndef MVM_MALLOC
#define MVM_MALLOC(size) malloc(size)
#endif

#ifndef MVM_FREE
#define MVM_FREE(p) free(p)
#endif

#ifndef MVM_CONTEXTUAL_MALLOC
#define MVM_CONTEXTUAL_MALLOC(size, context) MVM_MALLOC(size)
#endif

#ifndef MVM_CONTEXTUAL_FREE
#define MVM_CONTEXTUAL_FREE(p, context) MVM_FREE(p)
#endif

// "Hidden" functions cover some internal functions that can be optionally
// exposed by overriding MVM_HIDDEN. This is used by the WASM library to gain
// lower-level access to some internals for the purpose of efficiency. These are
// still undocumented implementation details and may change at any time, but
// dependents like the WASM library are updated in sync with the engine.
#if MVM_EXPOSE_INTERNALS
#define MVM_HIDDEN
#else
#define MVM_HIDDEN static
#endif

// This function might be unused if the user has overridden the
// MVM_CHECK_CRC16_CCITT macro.
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#endif
static uint16_t default_crc16(MVM_LONG_PTR_TYPE lp, uint16_t size) {
  uint16_t r = 0xFFFF;
  while (size--)
  {
    r  = (uint8_t)(r >> 8) | (r << 8);
    r ^= MVM_READ_LONG_PTR_1(lp);
    lp = MVM_LONG_PTR_ADD(lp, 1);
    r ^= (uint8_t)(r & 0xff) >> 4;
    r ^= (r << 8) << 4;
    r ^= ((r & 0xff) << 4) << 1;
  }
  return r;
}
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

// ---------------------------------------------------------------------------

typedef mvm_VM VM;
typedef mvm_TeError TeError;

#ifndef MVM_IMPORT_MATH
// By default, import math.h if floating point is used. But user can override
// this if they want to provide non-math.h implementations
#define MVM_IMPORT_MATH MVM_SUPPORT_FLOAT
#endif

#ifndef MVM_FLOAT_IS_NAN
#define MVM_FLOAT_IS_NAN(x) isnan(x)
#endif

#ifndef MVM_FLOAT_IS_NEG_ZERO
// Note: VisualC++ (and maybe other compilers) seem to have `0.0==-0.0` evaluate
// to true, which is why there's the second check here
#define MVM_FLOAT_IS_NEG_ZERO(x) ((x == -0.0) && (signbit(x) != 0))
#endif

#ifndef MVM_FLOAT_IS_FINITE
#define MVM_FLOAT_IS_FINITE(x) isfinite(x)
#endif

#ifndef MVM_FLOAT_NEG_ZERO
#define MVM_FLOAT_NEG_ZERO (-0.0)
#endif

#ifndef MVM_SNPRINTF
#define MVM_SNPRINTF snprintf
#endif

#ifndef MVM_MALLOC
#define MVM_MALLOC malloc
#endif

#ifndef MVM_FREE
#define MVM_FREE free
#endif

/**
 * mvm_Value
 *
 * Hungarian prefix: v
 *
 * Internally, the name `Value` refers to `mvm_Value`
 *
 * The Microvium Value type is 16 bits with a 1 or 2 bit discriminator in the
 * lowest bits:
 *
 *  - If the lowest bit is `0`, interpret the value as a `ShortPtr`. Note that
 *    in a snapshot bytecode file, a ShortPtr is measured relative to the
 *    beginning of the RAM section of the file.
 *  - If the lowest bits are `11`, interpret the high 14-bits as a signed 14 bit
 *    integer. The Value is an `VirtualInt14`
 *  - If the lowest bits are `01`, interpret the high 15-bits as a
 *    `BytecodeMappedPtr` or a well-known value.
 */
typedef mvm_Value Value;

static inline bool Value_isShortPtr(Value value) { return (value & 1) == 0; }
static inline bool Value_isBytecodeMappedPtrOrWellKnown(Value value) { return (value & 3) == 1; }
static inline bool Value_isVirtualInt14(Value value) { return (value & 3) == 3; }
static inline bool Value_isVirtualUInt12(Value value) { return (value & 0xC003) == 3; }
static inline bool Value_isVirtualUInt8(Value value) { return (value & 0xFC03) == 3; }

/**
 * ShortPtr
 *
 * Hungarian prefix: sp
 *
 * A ShortPtr is a 16-bit **non-nullable** reference which references into GC
 * memory, but not to data memory or bytecode.
 *
 * Note: To avoid confusion of when to use different kinds of null values,
 * ShortPtr should be considered non-nullable. When null is required, use
 * VM_VALUE_NULL for consistency, which is not defined as a short pointer.
 *
 * The GC assumes that anything with a low bit 0 is a non-null pointer into GC
 * memory (it does not do null checking on these, since this is a hot loop).
 *
 * Note: At runtime, pointers _to_ GC memory must always be encoded as
 * `ShortPtr` or indirectly through a BytecodeMappedPtr to a global variable.
 * This is because the GC assumes (for efficiency reasons) only values with the
 * lower bit `0` need to be traced/moved.
 *
 * A ShortPtr is interpreted one of 3 ways depending on the context:
 *
 *   1. On 16-bit architectures (when MVM_NATIVE_POINTER_IS_16_BIT is set),
 *      while the script is running, ShortPtr can be a native pointer, allowing
 *      for fast access. On other architectures, ShortPtr is encoded as an
 *      offset from the beginning of the virtual heap.
 *
 *   2. On non-16-bit architectures (when MVM_NATIVE_POINTER_IS_16_BIT is not
 *      set), ShortPtr is an offset into the allocation buckets. Access is
 *      linear time to the number of buckets, but the buckets are compacted
 *      together during a GC cycle so the number should typically be 1 or low.
 *
 *   3. In the hibernating GC heap, in the snapshot, ShortPtr is treated as an
 *      offset into the bytecode image, but always an offset back into the
 *      GC-RAM section. See `loadPointers`
 *
 * TODO: Rather than just MVM_NATIVE_POINTER_IS_16_BIT, we could better serve
 * small 32-bit devices by having a "page" #define that is added to ShortPtr to
 * get the real address. This is because on ARM architectures, the RAM pointers
 * are mapped to a higher address space.
 *
 * A ShortPtr must never exist in a ROM slot, since they need to have a
 * consistent representation in all cases, and ROM slots are not visited by
 * `loadPointers`. Also short pointers are used iff they point to GC memory,
 * which is subject to relocation and therefore cannot be referenced from an
 * immutable medium.
 *
 * If the lowest bit of the `ShortPtr` is 0 (i.e. points to an even boundary),
 * then the `ShortPtr` is also a valid `Value`.
 *
 * NULL short pointers are only allowed in some special circumstances, but are
 * mostly not valid.
 */
typedef uint16_t ShortPtr;

/**
 * Bytecode-mapped Pointer
 *
 * If `b` is a BytecodeMappedPtr then `b & 0xFFFE` is treated as an offset into
 * the bytecode address space, and its meaning depends on where in the bytecode
 * image it points:
 *
 *
 * 1. If the offset points to the BCS_ROM section of bytecode, it is interpreted
 *    as pointing to that ROM allocation or function.
 *
 * 2. If the offset points to the BCS_GLOBALS region of the bytecode image, the
 *    `BytecodeMappedPtr` is treated being a reference to the allocation
 *    referenced by the corresponding global variable.
 *
 * This allows ROM Values, such as literal, exports, and builtins, to reference
 * RAM allocations. *Note*: for the moment, behavior is not defined if the
 * corresponding global has non-pointer contents, such as an Int14 or well-known
 * value. In future this may be explicitly allowed.
 *
 * A `BytecodeMappedPtr` is only a pointer type and is not defined to encode the
 * well-known values or null.
 *
 * Note that in practice, BytecodeMappedPtr is not used anywhere except in
 * decoding DynamicPtr.
 *
 * See `BytecodeMappedPtr_decode_long`
 */
typedef uint16_t BytecodeMappedPtr;

/**
 * Dynamic Pointer
 *
 * Hungarian prefix: `dp`
 *
 * A `Value` that is a pointer. I.e. its lowest bits are not `11` and it does
 * not encode a well-known value. Can be one of:
 *
 *  - `ShortPtr`
 *  - `BytecodeMappedPtr`
 *  - `VM_VALUE_NULL`
 *
 * Note that the only valid representation of null for this point is
 * `VM_VALUE_NULL`, not 0.
 */
typedef Value DynamicPtr;

/**
 * ROM Pointer
 *
 * Hungarian prefix: none
 *
 * A `DynamicPtr` which is known to only point to ROM
 */
typedef Value RomPtr;

/**
 * Int14 encoded as a Value
 *
 * Hungarian prefix: `vi`
 *
 * A 14-bit signed integer represented in the high 14 bits of a 16-bit Value,
 * with the low 2 bits set to the bits `11`, as per the `Value` type.
 */
typedef Value VirtualInt14;

/**
 * Hungarian prefix: `lp`
 *
 * A nullable-pointer that can reference bytecode and RAM in the same address
 * space. Not necessarily 16-bit.
 *
 * The null representation for LongPtr is assumed to be 0.
 *
 * Values of this type are only managed through macros in the port file, never
 * directly, since the exact type depends on the architecture.
 *
 * See description of MVM_LONG_PTR_TYPE
 */
typedef MVM_LONG_PTR_TYPE LongPtr;

#define READ_FIELD_2(longPtr, structType, fieldName) \
  LongPtr_read2_aligned(LongPtr_add(longPtr, OFFSETOF(structType, fieldName)))

#define READ_FIELD_1(longPtr, structType, fieldName) \
  LongPtr_read1(LongPtr_add(longPtr, OFFSETOF(structType, fieldName)))

// NOTE: In no way are assertions meant to be present in production. They're
// littered everywhere on the assumption that they consume no overhead.
#if MVM_SAFE_MODE
  #define VM_ASSERT(vm, predicate) do { if (!(predicate)) MVM_FATAL_ERROR(vm, MVM_E_ASSERTION_FAILED); } while (false)
#else
  #define VM_ASSERT(vm, predicate)
#endif

#ifndef __has_builtin
  #define __has_builtin(x) 0
#endif

// Offset of field in a struct
#define OFFSETOF(TYPE, ELEMENT) ((uint16_t)(uintptr_t)&(((TYPE *)0)->ELEMENT))

// Maximum size of an allocation (4kB)
#define MAX_ALLOCATION_SIZE 0xFFF

// This is the only valid way of representing NaN
#define VM_IS_NAN(v) ((v) == VM_VALUE_NAN)
// This is the only valid way of representing infinity
#define VM_IS_INF(v) ((v) == VM_VALUE_INF)
// This is the only valid way of representing -infinity
#define VM_IS_NEG_INF(v) ((v) == VM_VALUE_NEG_INF)
// This is the only valid way of representing negative zero
#define VM_IS_NEG_ZERO(v) ((v) == VM_VALUE_NEG_ZERO)

#define VM_NOT_IMPLEMENTED(vm) MVM_FATAL_ERROR(vm, MVM_E_NOT_IMPLEMENTED)
#define VM_RESERVED(vm) MVM_FATAL_ERROR(vm, MVM_E_UNEXPECTED)

// An error corresponding to an internal inconsistency in the VM. Such an error
// cannot be caused by incorrect usage of the VM. In safe mode, this function
// should terminate the application. If not in safe mode, it is assumed that
// this function will never be invoked.
#define VM_UNEXPECTED_INTERNAL_ERROR(vm) (MVM_FATAL_ERROR(vm, MVM_E_UNEXPECTED), -1)

#define VM_VALUE_OF_DYNAMIC(v) ((void*)((TsAllocationHeader*)v + 1))
#define VM_DYNAMIC_TYPE(v) (((TsAllocationHeader*)v)->type)

#define VM_MAX_INT14 0x1FFF
#define VM_MIN_INT14 (-0x2000)

#if MVM_SAFE_MODE
#define VM_EXEC_SAFE_MODE(code) code
#define VM_SAFE_CHECK_NOT_NULL(v) do { if ((v) == NULL) return MVM_E_UNEXPECTED; } while (false)
#define VM_SAFE_CHECK_NOT_NULL_2(v) do { if ((v) == NULL) { MVM_FATAL_ERROR(vm, MVM_E_UNEXPECTED); return NULL; } } while (false)
#define VM_ASSERT_UNREACHABLE(vm) MVM_FATAL_ERROR(vm, MVM_E_UNEXPECTED)
#else
#define VM_EXEC_SAFE_MODE(code)
#define VM_SAFE_CHECK_NOT_NULL(v)
#define VM_SAFE_CHECK_NOT_NULL_2(v)
#define VM_ASSERT_UNREACHABLE(vm)
#endif

#if MVM_DONT_TRUST_BYTECODE || MVM_SAFE_MODE
// TODO: I think I need to do an audit of all the assertions and errors in the code, and make sure they're categorized correctly as bytecode errors or not
#define VM_INVALID_BYTECODE(vm) MVM_FATAL_ERROR(vm, MVM_E_INVALID_BYTECODE)
#define VM_BYTECODE_ASSERT(vm, condition) do { if (!(condition)) VM_INVALID_BYTECODE(vm); } while (false)
#else
#define VM_INVALID_BYTECODE(vm)
#define VM_BYTECODE_ASSERT(vm, condition)
#endif

#ifndef CODE_COVERAGE
/*
 * A set of macros for manual code coverage analysis (because the off-the-shelf
 * tools appear to be quite expensive). This should be overridden in the port
 * file for the unit tests. Each instance of this macro should occur on its own
 * line. The unit tests can dumbly scan the source text for instances of this
 * macro to establish what code paths _should_ be hit. Each instance should have
 * its own unique numeric ID.
 *
 * If the ID is omitted or a non-integer placeholder (e.g. "x"), the script `npm
 * run update-coverage-markers` will fill in a valid ID.
 *
 * Explicit IDs are used instead of line numbers because a previous analysis
 * remains roughly correct even after the code has changed.
 */
#define CODE_COVERAGE(id)
#define CODE_COVERAGE_UNTESTED(id)
#define CODE_COVERAGE_UNIMPLEMENTED(id)
#define CODE_COVERAGE_ERROR_PATH(id)

/**
 * In addition to recording code coverage, it's useful to have information about
 * the coverage information for table entries. Code and tables can be
 * alternative representations of the same thing. For example, a lookup table
 * can be represented as a switch statement. However, only the switch statement
 * form typically shows up in code coverage analysis. With Microvium coverage
 * analysis, tables are covered as well.
 *
 * If the ID is omitted or a non-integer placeholder (e.g. "x"), the script `npm
 * run update-coverage-markers` will fill in a valid ID.
 *
 * @param indexInTable The runtime expression for the case that is actually hit.
 * @param tableSize The size of the table (can be a runtime expression)
 * @param id A unique numeric ID to uniquely identify the marker
 */
#define TABLE_COVERAGE(indexInTable, tableSize, id)
#endif

#ifndef MVM_SUPPORT_FLOAT
#define MVM_SUPPORT_FLOAT 1
#endif

#ifndef MVM_PORT_INT32_OVERFLOW_CHECKS
#define MVM_PORT_INT32_OVERFLOW_CHECKS 1
#endif

#ifndef MVM_SAFE_MODE
#define MVM_SAFE_MODE 0
#endif

// TODO: The example port file sets to 1 because we want it enabled in the
// tests. But really we should have a separate test port file.
#ifndef MVM_VERY_EXPENSIVE_MEMORY_CHECKS
#define MVM_VERY_EXPENSIVE_MEMORY_CHECKS 0
#endif

#ifndef MVM_DONT_TRUST_BYTECODE
#define MVM_DONT_TRUST_BYTECODE 0
#endif

#ifndef MVM_SWITCH
#define MVM_SWITCH(tag, upper) switch (tag)
#endif

#ifndef MVM_CASE
#define MVM_CASE(value) case value
#endif

#ifndef MVM_DEBUG_UTILS
#define MVM_DEBUG_UTILS 0
#endif

// Allocation headers on functions are different. Nothing needs the allocation
// size specifically, so the 12 size bits are repurposed.
/** Flag bit to indicate continuation vs normal func. (1 = continuation) */
#define VM_FUNCTION_HEADER_CONTINUATION_FLAG 0x0800
/** (Continuations only) Mask of number of quad-words that the continuation is
 * behind its containing function */
#define VM_FUNCTION_HEADER_BACK_POINTER_MASK 0x07FF
/** (Normal funcs only) Mask of required stack height in words */
#define VM_FUNCTION_HEADER_STACK_HEIGHT_MASK 0x00FF

// Minimum number of items to have in an array when expanding it
#define VM_ARRAY_INITIAL_CAPACITY 4

/**
 * Type code indicating the type of data.
 *
 * This enumeration is divided into reference types (TC_REF_) and value types
 * (TC_VAL_). Reference type codes are used on allocations, whereas value type
 * codes are never used on allocations. The space for the type code in the
 * allocation header is 4 bits, so there are up to 16 reference types and these
 * must be the first 16 types in the enumeration.
 *
 * The reference type range is subdivided into containers or non-containers. The
 * GC uses this distinction to decide whether the body of the allocation should
 * be interpreted as `Value`s (i.e. may contain pointers). To minimize the code,
 * either ALL words in a container are `Value`s, or none.
 *
 * Value types are for the values that can be represented within the 16-bit
 * mvm_Value without interpreting it as a pointer.
 */
typedef enum TeTypeCode {
  // Note: only type code values in the range 0-15 can be used as the types for
  // allocations, since the allocation header allows 4 bits for the type. Types
  // 0-8 are non-container types, 0xC-F are container types (9-B reserved).
  // Every word in a container must be a `Value`. No words in a non-container
  // can be a `Value` (the GC uses this to distinguish whether an allocation may
  // contain pointers, and the signature of each word). Note that buffer-like
  // types would not count as containers by this definition.

  /* --------------------------- Reference types --------------------------- */

  // A type used during garbage collection. Allocations of this type have a
  // single 16-bit forwarding pointer in the allocation.
  TC_REF_TOMBSTONE          = 0x0,

  TC_REF_INT32              = 0x1, // 32-bit signed integer
  TC_REF_FLOAT64            = 0x2, // 64-bit float

  /**
   * UTF8-encoded string that may or may not be unique.
   *
   * Note: If a TC_REF_STRING is in bytecode, it is because it encodes a value
   * that is illegal as a property index in Microvium (i.e. it encodes an
   * integer).
   */
  TC_REF_STRING             = 0x3,

  /**
   * TC_REF_INTERNED_STRING
   *
   * A string whose address uniquely identifies its contents, and does not
   * encode an integer in the range 0 to 0x1FFF.
   *
   * To keep property lookup efficient, Microvium requires that strings used as
   * property keys can be compared using pointer equality. This requires that
   * there is only one instance of each of those strings (see
   * https://en.wikipedia.org/wiki/String_interning).
   *
   * A string with the type code TC_REF_INTERNED_STRING means that it exists in
   * one of the interning tables (either the one in ROM or the one in RAM). Not
   * all strings are interned, because it would be expensive if every string
   * concatenation resulted in a search of the intern table and possibly a new
   * entry (imagine if every JSON string landed up in the table!).
   *
   * In practice we do this:
   *
   *  - All valid non-index property keys in ROM are interned. If a string is in
   *    ROM but it is not interned, the engine can conclude that it is not a
   *    valid property key or it is an index.
   *  - Strings constructed in RAM are only interned when they're used to access
   *    properties.
   */
  TC_REF_INTERNED_STRING    = 0x4,

  TC_REF_FUNCTION           = 0x5, // TsBytecodeFunc
  TC_REF_HOST_FUNC          = 0x6, // TsHostFunc

  TC_REF_UINT8_ARRAY        = 0x7, // Byte buffer
  TC_REF_SYMBOL             = 0x8, // Reserved: Symbol

  /* --------------------------- Container types --------------------------- */
  TC_REF_DIVIDER_CONTAINER_TYPES,  // <--- Marker. Types after or including this point but less than 0x10 are container types

  TC_REF_CLASS              = 0x9, // TsClass
  TC_REF_VIRTUAL            = 0xA, // Reserved: TsVirtual
  TC_REF_RESERVED_1         = 0xB, // Reserved
  TC_REF_PROPERTY_LIST      = 0xC, // TsPropertyList - Object represented as linked list of properties
  TC_REF_ARRAY              = 0xD, // TsArray
  TC_REF_FIXED_LENGTH_ARRAY = 0xE, // TsFixedLengthArray
  TC_REF_CLOSURE            = 0xF, // TsClosure (see description on struct)

  /* ----------------------------- Value types ----------------------------- */
  TC_VAL_INT14              = 0x10,

  TC_VAL_UNDEFINED          = 0x11,
  TC_VAL_NULL               = 0x12,
  TC_VAL_TRUE               = 0x13,
  TC_VAL_FALSE              = 0x14,
  TC_VAL_NAN                = 0x15,
  TC_VAL_NEG_ZERO           = 0x16,
  TC_VAL_DELETED            = 0x17, // Placeholder for properties and list items that have been deleted or holes in arrays
  TC_VAL_STR_LENGTH         = 0x18, // The string "length"
  TC_VAL_STR_PROTO          = 0x19, // The string "__proto__"

  /**
   * TC_VAL_NO_OP_FUNC
   *
   * Represents a function that does nothing and returns undefined.
   *
   * This is required by async-await for the case where you void-call an async
   * function and it needs to synthesize a dummy callback that does nothing,
   * particularly for a host async function to call back.
   */
  TC_VAL_NO_OP_FUNC         = 0x1A,

  TC_END,
} TeTypeCode;

// Note: VM_VALUE_NAN must be used instead of a pointer to a double that has a
// NaN value (i.e. the values must be normalized to use the following table).
// Operations will assume this canonical form.

// Note: the `(... << 2) | 1` is so that these values don't overlap with the
// ShortPtr or BytecodeMappedPtr address spaces.

// Indexes of internal slots for objects. Note that even-valued indexed slots
// can only hold negative int14 values since those slots are overloading
// property key slots. The slot indexes can only start at 2 because the first 2
// "slots" are the dpNext and dpProto of TsPropertyList.
typedef enum vm_TeObjectInternalSlots {
  VM_OIS_NEXT = 0,
  VM_OIS_PROTO = 1,

  VM_OIS_PROTO_SLOT_MAGIC_KEY = 2,
  VM_OIS_PROTO_SLOT_COUNT = 3,

  VM_OIS_PROMISE_STATUS = 2, // vm_TePromiseStatus
  VM_OIS_PROMISE_OUT = 3, // Result if resolved, error if rejected, undefined or subscriber or subscriber-list if pending
} vm_TeObjectInternalSlots;

#define VIRTUAL_INT14_ENCODE(i) ((uint16_t)(((unsigned int)(i) << 2) | 3))

// Note: these values must be negative int14 values
typedef enum vm_TePromiseStatus {
  VM_PROMISE_STATUS_PENDING = VIRTUAL_INT14_ENCODE(-1),
  VM_PROMISE_STATUS_RESOLVED = VIRTUAL_INT14_ENCODE(-2),
  VM_PROMISE_STATUS_REJECTED = VIRTUAL_INT14_ENCODE(-3),
} vm_TePromiseStatus;

#define VM_PROTO_SLOT_MAGIC_KEY_VALUE VIRTUAL_INT14_ENCODE(-0x2000)

// Some well-known values
typedef enum vm_TeWellKnownValues {
  // Note: well-known values share the bytecode address space, so we can't have
  // too many here before user-defined allocations start to become unreachable.
  // The first addressable user allocation in a bytecode image is around address
  // 0x2C (measured empirically -- see test `1.empty-export`) if the image has
  // one export and one string in the string table, which means the largest
  // well-known-value can be the prior address `0x2C-4=0x28` (encoded as a
  // bytecode pointer will be 0x29), corresponding to type-code 0x1B.

  VM_VALUE_UNDEFINED     = (((int)TC_VAL_UNDEFINED - 0x11) << 2) | 1, // = 1
  VM_VALUE_NULL          = (((int)TC_VAL_NULL - 0x11) << 2) | 1,
  VM_VALUE_TRUE          = (((int)TC_VAL_TRUE - 0x11) << 2) | 1,
  VM_VALUE_FALSE         = (((int)TC_VAL_FALSE - 0x11) << 2) | 1,
  VM_VALUE_NAN           = (((int)TC_VAL_NAN - 0x11) << 2) | 1,
  VM_VALUE_NEG_ZERO      = (((int)TC_VAL_NEG_ZERO - 0x11) << 2) | 1,
  VM_VALUE_DELETED       = (((int)TC_VAL_DELETED - 0x11) << 2) | 1,
  VM_VALUE_STR_LENGTH    = (((int)TC_VAL_STR_LENGTH - 0x11) << 2) | 1,
  VM_VALUE_STR_PROTO     = (((int)TC_VAL_STR_PROTO - 0x11) << 2) | 1,
  VM_VALUE_NO_OP_FUNC    = (((int)TC_VAL_NO_OP_FUNC - 0x11) << 2) | 1,

  VM_VALUE_WELLKNOWN_END,
} vm_TeWellKnownValues;

typedef struct TsArray {
 /*
  * Note: the capacity of the array is the length of the TsFixedLengthArray
  * pointed to by dpData, or 0 if dpData is VM_VALUE_NULL. The logical length
  * of the array is determined by viLength.
  *
  * Note: If dpData is not null, it must be a unique pointer (it must be the
  * only pointer that points to that allocation)
  *
  * Note: for arrays in GC memory, their dpData must point to GC memory as well
  *
  * Note: Values in dpData that are beyond the logical length MUST be filled
  * with VM_VALUE_DELETED.
  */

  DynamicPtr dpData; // Points to TsFixedLengthArray or VM_VALUE_NULL
  VirtualInt14 viLength;
} TsArray;

typedef struct TsFixedLengthArray {
  // Note: the length of the fixed-length-array is determined by the allocation header
  Value items[1];
} TsFixedLengthArray;

typedef struct vm_TsStack vm_TsStack;

/**
 * Used to represent JavaScript objects.
 *
 * The `proto` pointer points to the prototype of the object.
 *
 * Properties on object are stored in a linked list of groups. Each group has a
 * `next` pointer to the next group (list). When assigning to a new property,
 * rather than resizing a group, the VM will just append a new group to the list
 * (a group with just the one new property).
 *
 * Only the `proto` field of the first group of properties in an object is used.
 *
 * The garbage collector compacts multiple groups into one large one, so it
 * doesn't matter that appending a single property requires a whole new group on
 * its own or that they have unused proto properties.
 *
 * Note: at one stage, I thought that objects could be treated like arrays and
 * just expand geometrically rather than as linked lists. This would work, but
 * then like dynamic arrays they would need to be 2 allocations instead of 1
 * because we can't find all the references to the object each time it grows.
 *
 * Something I've thought of, but not considered too deeply yet, is the
 * possibility of implementing objects in terms of dynamic arrays, to reuse the
 * machinery of dynamic arrays in terms of growing and compacting. This could
 * potentially make the engine smaller.
 */
typedef struct TsPropertyList {
  // Note: if the property list is in GC memory, then dpNext must also point to
  // GC memory, but dpProto can point to any memory (e.g. a prototype stored in
  // ROM).

  // Note: in the serialized form, the next pointer must be null
  DynamicPtr dpNext; // TsPropertyList* or VM_VALUE_NULL, containing further appended properties
  DynamicPtr dpProto; // Note: the prototype is only meaningful on the first in the list
  /*
  Followed by N of these pairs to the end of the allocated size:
    Value key; // TC_VAL_INT14 or TC_REF_INTERNED_STRING
    Value value;
   */
} TsPropertyList;

/**
 * A property list with a single property. See TsPropertyList for description.
 */
typedef struct TsPropertyCell /* extends TsPropertyList */ {
  TsPropertyList base;
  Value key; // TC_VAL_INT14 or TC_REF_INTERNED_STRING
  Value value;
} TsPropertyCell;

/**
 * A TsClosure (TC_REF_CLOSURE) is a function-like (callable) container that is
 * overloaded to represent both closures and/or their variable environments.
 *
 * See also [closures](../doc/internals/closures.md)
 *
 * The first and last slots in a closure are special:
 *
 *   1. The first slot is the function `target`. If a CALL operation is executed
 *      on a closure then the call is delegated to the function in the first
 *      slot. It's permissable to use this slot for other purposes if the
 *      closure will never be called.
 *
 *   2. The last slot is the `parentScope`. If the index provided to
 *      `LoadScoped` or `StoreScoped` overflow the current closure then they
 *      automatically index into the parent scope, recursively up the chain.
 *      It's permissible to use this slot for custom purposes if the bytecode
 *      will not try to access variables from a parent scope.
 *
 * The minimum closure size is 1 slot. This could happen if neither the function
 * slot nor parent slot are used, and the scope contains a single variable.
 *
 * The bytecode instructions LoadScoped and StoreScoped write to the slots of
 * the _current closure_ (TsRegisters.closure).
 *
 * The instruction VM_OP1_CLOSURE_NEW creates a closure with exactly 2 slots,
 * where the first second is populated from the current closure.
 *
 * The instruction VM_OP1_SCOPE_PUSH creates a closure with any number of slots
 * and no function pointer, and sets it as the current closure. From there, the
 * IL can set it's own function pointer using `StoreScoped`.
 */
typedef struct TsClosure {
  Value target; // function
  /* followed optionally by other variables, and finally by a pointer to the
  parent scope if needed */
} TsClosure;

/**
 * This type is to provide support for a subset of the ECMAScript classes
 * feature. Classes can be instantiated using `new`, but it is illegal to call
 * them directly. Similarly, `new` doesn't work on arbitrary function.
 */
typedef struct TsClass {
  Value constructorFunc; // Function type
  Value staticProps;
} TsClass;

/**
 * TsVirtual (at the time of this writing, this is just a placeholder type)
 *
 * This is a placeholder for an idea to have something like a "low-level proxy"
 * type. See my private notes for details (if you have access to them). The
 * `type` and `state` fields correspond roughly to the "handler" and "target"
 * fields respectively in a normal ES `Proxy`.
 */
typedef struct TsVirtual {
  Value state;
  Value type;
} TsVirtual;

// External function by index in import table
typedef struct TsHostFunc {
  // Note: TC_REF_HOST_FUNC is not a container type, so it's fields are not
  // traced by the GC.
  //
  // Note: most host function reference can be optimized to not require this
  // allocation -- they can use VM_OP2_CALL_HOST directly. This allocation is
  // only required then the reference to host function is ambiguous or there are
  // calls to more than 256 host functions.
  uint16_t indexInImportTable;
} TsHostFunc;

typedef struct TsBucket {
  uint16_t offsetStart; // The number of bytes in the heap before this bucket
  struct TsBucket* prev;
  struct TsBucket* next;
  /* Note: pEndOfUsedSpace used to be on the VM struct, rather than per-bucket.
   * The main reason it's useful to have it on each bucket is in the hot GC-loop
   * which needs to check if it's caught up with the write cursor in to-space or
   * check if it's hit the end of the bucket. Without this value being in each
   * bucket, the calculation to find the end of the bucket is expensive.
   *
   * Note that for the last bucket, `pEndOfUsedSpace` doubles up as the write
   * cursor, since it's only recording the *used* space. The *capacity* of each
   * bucket is not recorded, but the capacity of the *last* bucket is recorded
   * in `pLastBucketEndCapacity` (on the VM and GC structures).  */
  uint16_t* pEndOfUsedSpace;

  /* ...data */
} TsBucket;

typedef struct TsBreakpoint {
  struct TsBreakpoint* next;
  int bytecodeAddress;
} TsBreakpoint;

/*
  Minimum size:
    - 6 pointers + 1 long pointer + 4 words
    - = 24B on 16bit
    - = 36B on 32bit.

  Maximum size (on 64-bit machine):
    - 9 pointers + 4 words
    - = 80 bytes on 64-bit machine

  See also the unit tests called "minimal-size"

*/
struct mvm_VM {
  uint16_t* globals;
  LongPtr lpBytecode;
  vm_TsStack* stack;

  // Last bucket of GC memory
  TsBucket* pLastBucket;
  // End of the capacity of the last bucket of GC memory
  uint16_t* pLastBucketEndCapacity;
  // Handles - values to treat as GC roots
  mvm_Handle* gc_handles;

  void* context;

  #if MVM_INCLUDE_DEBUG_CAPABILITY
  TsBreakpoint* pBreakpoints;
  mvm_TfBreakpointCallback breakpointCallback;
  #endif // MVM_INCLUDE_DEBUG_CAPABILITY

  #ifdef MVM_GAS_COUNTER
  int32_t stopAfterNInstructions; // Set to -1 to disable
  #endif // MVM_GAS_COUNTER

  uint16_t heapSizeUsedAfterLastGC;
  uint16_t stackHighWaterMark;
  uint16_t heapHighWaterMark;

  #if MVM_VERY_EXPENSIVE_MEMORY_CHECKS
  // Amount to shift the heap over during each collection cycle
  uint8_t gc_heap_shift;
  #endif

  #if MVM_SAFE_MODE
  // A number that increments at every possible opportunity for a GC cycle
  uint8_t gc_potentialCycleNumber;
  #endif // MVM_SAFE_MODE
};

typedef struct TsInternedStringCell {
  ShortPtr spNext;
  Value str;
} TsInternedStringCell;

// Possible values for the `flags` machine register
typedef enum vm_TeActivationFlags {
  // This is not an activation flag, but I'm putting it in this enum because it
  // shares the same bit space as the flags.
  AF_ARG_COUNT_MASK = 0x7F,

  // Note: these flags start at bit 8 because they use the same word as the
  // argument count and the high byte is used for flags, with the exception of
  // AF_VOID_CALLED which is in the first byte because the flag is bundled with
  // the argument count during a call operation.

  // Set to 1 in the current activation frame if the caller call site is a void
  // call (does not use the response). Note: this flag is in the high bit of the
  // first byte, unlike the other bits which are in the second byte. See above
  // for description.
  AF_VOID_CALLED = 1 << 7,

  // Flag to indicate if the most-recent CALL operation involved a stack-based
  // function target (as opposed to a literal function target). If this is set,
  // then the next RETURN instruction will also pop the function reference off
  // the stack.
  AF_PUSHED_FUNCTION = 1 << 8,

  // Flag to indicate that returning from the current frame should return to the host
  AF_CALLED_FROM_HOST = 1 << 9,

  // Only used by mvm_callEx to indicate that the `this` value is already on the stack
  AF_OVERRIDE_THIS = 1 << 10,
} vm_TeActivationFlags;

/**
 * This struct is malloc'd from the host when the host calls into the VM
 */
typedef struct vm_TsRegisters { // 28 B on 32-bit machine
  uint16_t* pFrameBase;
  uint16_t* pStackPointer;
  LongPtr lpProgramCounter;
  // Note: I previously used to infer the location of the arguments based on the
  // number of values PUSHed by a CALL instruction to preserve the activation
  // state (i.e. 3 words). But now that distance is dynamic, so we need and
  // explicit register.
  Value* pArgs;
  uint16_t* pCatchTarget; // NULL if no catch block, otherwise points to catch block
  uint16_t argCountAndFlags; // Lower 8 bits are argument count, upper 8 bits are vm_TeActivationFlags
  Value closure; // Closure scope or VM_VALUE_UNDEFINED

  /**
   * Contains the asynchronous callback for the call of the current activation
   * record.
   *
   * - VM_VALUE_UNDEFINED - Normal call (no callback)
   * - VM_VALUE_DELETED - (poison value) value no longer holds the callback for
   *   the current activation (value has been trashed or consumed)
   * - Pointer to function - Directly after AsyncCall operation
   */
  Value cpsCallback;

  /**
   * The (promise) job queue, for scheduling async callbacks. One of 4 states:
   *
   *   - Unallocated (no registers) - no jobs
   *   - `undefined` means there are no promise jobs enqueued. The reason not to
   *     use `NULL` (0) is because this value is reachable by the garbage
   *     collector and so making it a consistent JavaScript value makes sense.
   *   - A function value: indicates there is only one job in the queue, and the
   *     `jobQueue` register points directly to it.
   *   - A fixed-length array of 3 values: a tuple of `[prev, job, next]` as a
   *     doubly-linked list node. Except that instead of a list, it forms a
   *     cycle, so that the back of the "list" can be reached in `O(1)` time as
   *     as the `prev` of the first item, without needing a second register to
   *     point to the back of the list.
   */
  Value jobQueue;

  #if MVM_SAFE_MODE
  // This will be true if the VM is operating on the local variables rather
  // than the shared vm_TsRegisters structure.
  uint8_t usingCachedRegisters;
  uint8_t _reserved; // My compiler seems to pad this out anyway
  #endif

} vm_TsRegisters;

/**
 * This struct is malloc'd from the host when the host calls into the VM and
 * freed when the VM finally returns to the host. This struct embeds both the
 * working registers and the call stack in the same allocation since they are
 * needed at the same time and it's more efficient to do a single malloc where
 * possible.
 */
struct vm_TsStack {
  // Allocate registers along with the stack, because these are needed at the same time (i.e. while the VM is active)
  vm_TsRegisters reg;
  // Note: the stack grows upwards (towards higher addresses)
  // ... (stack memory) ...
};

typedef struct TsAllocationHeader {
  /* 4 least-significant-bits are the type code (TeTypeCode). Remaining 12 bits
  are the allocation size, excluding the size of the header itself, in bytes
  (measured in bytes so that we can represent the length of strings exactly).
  See also `vm_getAllocationSizeExcludingHeaderFromHeaderWord` */
  uint16_t headerData;
} TsAllocationHeader;

typedef struct TsBytecodeFunc {
  uint8_t maxStackDepth;
  /* Followed by the bytecode bytes */
} TsBytecodeFunc;

typedef struct vm_TsImportTableEntry {
  mvm_HostFunctionID hostFunctionID;
  /*
  Note: I considered having a `paramCount` field in the header since a common
  scenario would be copying the arguments into the parameter slots. However,
  most parameters are not actually mutated in a function, so the LOAD_ARG
  instruction could just be used directly to get the parameter value (if the
  optimizer can detect such cases).
  */
} vm_TsImportTableEntry;

#define GC_TRACE_STACK_COUNT 20

typedef struct gc_TsGCCollectionState {
  VM* vm;
  TsBucket* firstBucket;
  TsBucket* lastBucket;
  uint16_t* lastBucketEndCapacity;
} gc_TsGCCollectionState;

typedef struct mvm_TsCallStackFrame {
  uint16_t programCounter;
  Value* frameBase;
  int frameDepth; // Number of variables
  Value* args;
  int argCount;
  Value* closure;
  int closureSlotCount;
} mvm_TsCallStackFrame;

typedef struct mvm_TsHeapAllocationInfo {
  // Note: the short names are because they display better in the debugger
  uint16_t a; // Lower 16-bits of address
  TeTypeCode t; // type code
  uint16_t s; // size
  uint16_t offset; // What the address would be if serialized in a snapshot
  Value* address;
} mvm_TsHeapAllocationInfo;

#define TOMBSTONE_HEADER ((TC_REF_TOMBSTONE << 12) | 2)

// A CALL instruction saves the current registers to the stack. I'm calling this
// the "frame boundary" since it is a fixed-size sequence of words that marks
// the boundary between stack frames. The shape of this saved state is coupled
// to a few different places in the engine, so I'm versioning it here in case I
// need to make changes
#define VM_FRAME_BOUNDARY_VERSION 2

// The number of words between one call stack frame and the next (i.e. the
// number of saved registers during a CALL)
#define VM_FRAME_BOUNDARY_SAVE_SIZE_WORDS 4

static inline mvm_HostFunctionID vm_getHostFunctionId(VM*vm, uint16_t hostFunctionIndex);
static TeError vm_createStackAndRegisters(VM* vm);
static TeError vm_requireStackSpace(VM* vm, uint16_t* pStackPointer, uint16_t sizeRequiredInWords);
static Value vm_convertToString(VM* vm, Value value);
static Value vm_concat(VM* vm, Value* left, Value* right);
static TeTypeCode deepTypeOf(VM* vm, Value value);
static bool vm_isString(VM* vm, Value value);
static int32_t vm_readInt32(VM* vm, TeTypeCode type, Value value);
static TeError vm_resolveExport(VM* vm, mvm_VMExportID id, Value* result);
static inline mvm_TfHostFunction* vm_getResolvedImports(VM* vm);
static void gc_createNextBucket(VM* vm, uint16_t bucketSize, uint16_t minBucketSize);
static void gc_freeGCMemory(VM* vm);
static Value vm_allocString(VM* vm, size_t sizeBytes, void** data);
static TeError toPropertyName(VM* vm, Value* value);
static void toInternedString(VM* vm, Value* pValue);
static uint16_t vm_stringSizeUtf8(VM* vm, Value str);
static bool vm_ramStringIsNonNegativeInteger(VM* vm, Value str);
static TeError toInt32Internal(mvm_VM* vm, Value value, int32_t* out_result);
static inline uint16_t vm_getAllocationSizeExcludingHeaderFromHeaderWord(uint16_t headerWord);
static inline LongPtr LongPtr_add(LongPtr lp, int16_t offset);
static inline uint16_t LongPtr_read2_aligned(LongPtr lp);
static inline uint16_t LongPtr_read2_unaligned(LongPtr lp);
static void memcpy_long(void* target, LongPtr source, size_t size);
static void loadPointers(VM* vm, uint8_t* heapStart);
static inline ShortPtr ShortPtr_encode(VM* vm, void* ptr);
static inline uint8_t LongPtr_read1(LongPtr lp);
static LongPtr DynamicPtr_decode_long(VM* vm, DynamicPtr ptr);
static inline int16_t LongPtr_sub(LongPtr lp1, LongPtr lp2);
static inline uint16_t readAllocationHeaderWord(void* pAllocation);
static inline uint16_t readAllocationHeaderWord_long(LongPtr pAllocation);
static inline void* mvm_allocateWithConstantHeader(VM* vm, uint16_t header, uint16_t sizeIncludingHeader);
static inline uint16_t vm_makeHeaderWord(VM* vm, TeTypeCode tc, uint16_t size);
static int memcmp_long(LongPtr p1, LongPtr p2, size_t size);
static LongPtr getBytecodeSection(VM* vm, mvm_TeBytecodeSection id, LongPtr* out_end);
static inline void* LongPtr_truncate(VM* vm, LongPtr lp);
static inline LongPtr LongPtr_new(void* p);
static inline uint16_t* getBottomOfStack(vm_TsStack* stack);
static inline uint16_t* getTopOfStackSpace(vm_TsStack* stack);
static inline void* getBucketDataBegin(TsBucket* bucket);
static uint16_t getBucketOffsetEnd(TsBucket* bucket);
static uint16_t getSectionSize(VM* vm, mvm_TeBytecodeSection section);
static Value vm_intToStr(VM* vm, int32_t i);
static Value vm_newStringFromCStrNT(VM* vm, const char* s);
static TeError vm_validatePortFileMacros(MVM_LONG_PTR_TYPE lpBytecode, mvm_TsBytecodeHeader* pHeader, void* context);
static LongPtr vm_toStringUtf8_long(VM* vm, Value value, size_t* out_sizeBytes);
static LongPtr vm_findScopedVariable(VM* vm, uint16_t index);
static inline Value vm_readScopedFromThisClosure(VM* vm, uint16_t varIndex);
static inline void vm_writeScopedToThisClosure(VM* vm, uint16_t varIndex, Value value);
static Value vm_cloneContainer(VM* vm, Value* pArr);
static Value vm_safePop(VM* vm, Value* pStackPointerAfterDecr);
static LongPtr vm_getStringData(VM* vm, Value value);
static inline VirtualInt14 VirtualInt14_encode(VM* vm, int16_t i);
static inline int16_t VirtualInt14_decode(VM* vm, VirtualInt14 viInt);
static inline TeTypeCode vm_getTypeCodeFromHeaderWord(uint16_t headerWord);
static bool DynamicPtr_isRomPtr(VM* vm, DynamicPtr dp);
static inline void mvm_checkValueAccess(VM* vm, uint8_t potentialCycleNumber);
static inline uint16_t vm_getAllocationSize(void* pAllocation);
static inline uint16_t vm_getAllocationSize_long(LongPtr lpAllocation);
static inline TeTypeCode vm_getAllocationType(void* pAllocation);
static inline mvm_TeBytecodeSection vm_sectionAfter(VM* vm, mvm_TeBytecodeSection section);
static void* ShortPtr_decode(VM* vm, ShortPtr shortPtr);
static TeError vm_newError(VM* vm, TeError err);
static void* vm_malloc(VM* vm, size_t size);
static void vm_free(VM* vm, void* ptr);
static inline uint16_t* getTopOfStackSpace(vm_TsStack* stack);
static Value* vm_getHandleTargetOrNull(VM* vm, Value value);
static Value vm_resolveIndirections(VM* vm, Value value);
static mvm_TeError vm_uint8ArrayNew(VM* vm, Value* slot);
static Value getBuiltin(VM* vm, mvm_TeBuiltins builtinID);
static uint16_t* vm_scopePushOrNew(VM* vm, int slotCount, bool captureParent);
static inline Value vm_encodeBytecodeOffsetAsPointer(VM* vm, uint16_t offset);
static void vm_enqueueJob(VM* vm, Value jobClosure);
static Value vm_dequeueJob(VM* vm);
static void* DynamicPtr_decode_native(VM* vm, DynamicPtr ptr);
static mvm_Value* vm_push(mvm_VM* vm, mvm_Value value);
static Value vm_pop(mvm_VM* vm);
static Value vm_asyncStartUnsafe(mvm_VM* vm, Value* out_result);
static Value vm_objectCreate(VM* vm, Value prototype, int internalSlotCount);
static void vm_scheduleContinuation(VM* vm, Value continuation, Value isSuccess, Value resultOrError);
static Value vm_newArray(VM* vm, uint16_t capacity);
static void vm_arrayPush(VM* vm, Value* pvArr, Value* pvItem);
static void growArray(VM* vm, Value* pvArr, uint16_t newLength, uint16_t newCapacity);

#if MVM_SUPPORT_FLOAT
MVM_FLOAT64 mvm_toFloat64(mvm_VM* vm, Value value);
#endif // MVM_SUPPORT_FLOAT

// The MVM_HIDDEN functions are not exposed by default but can be linked to if
// needed. This is currently used by the WASM wrapper to get low-level access to
// some features.
MVM_HIDDEN TeError vm_objectKeys(VM* vm, Value* pObject);
MVM_HIDDEN TeError getProperty(VM* vm, Value* pObjectValue, Value* pPropertyName, Value* out_propertyValue);
MVM_HIDDEN TeError setProperty(VM* vm, Value* pObject, Value* pPropertyName, Value* pPropertyValue);
MVM_HIDDEN void* mvm_allocate(VM* vm, uint16_t sizeBytes, uint8_t /*TeTypeCode*/ typeCode);
MVM_HIDDEN void mvm_subscribeToPromise(VM* vm, Value vPromise, Value vCallback);

#if MVM_SAFE_MODE
static inline uint16_t vm_getResolvedImportCount(VM* vm);
#endif // MVM_SAFE_MODE

#if MVM_DEBUG_UTILS
void mvm_checkHeap(mvm_VM* vm);
int mvm_readHeapCount(VM* vm);
void mvm_checkValue(mvm_VM* vm, Value value);
mvm_TsCallStackFrame* mvm_readCallStack(VM* vm, int* out_frameCount);
mvm_TsHeapAllocationInfo* mvm_readHeap(VM* vm, int* out_count);
#endif

static const Value smallLiterals[] = {
  /* VM_SLV_UNDEFINED */    VM_VALUE_DELETED,
  /* VM_SLV_UNDEFINED */    VM_VALUE_UNDEFINED,
  /* VM_SLV_NULL */         VM_VALUE_NULL,
  /* VM_SLV_FALSE */        VM_VALUE_FALSE,
  /* VM_SLV_TRUE */         VM_VALUE_TRUE,
  /* VM_SLV_INT_MINUS_1 */  VIRTUAL_INT14_ENCODE(-1),
  /* VM_SLV_INT_0 */        VIRTUAL_INT14_ENCODE(0),
  /* VM_SLV_INT_1 */        VIRTUAL_INT14_ENCODE(1),
  /* VM_SLV_INT_2 */        VIRTUAL_INT14_ENCODE(2),
  /* VM_SLV_INT_3 */        VIRTUAL_INT14_ENCODE(3),
  /* VM_SLV_INT_4 */        VIRTUAL_INT14_ENCODE(4),
  /* VM_SLV_INT_5 */        VIRTUAL_INT14_ENCODE(5),
};
#define smallLiteralsSize (sizeof smallLiterals / sizeof smallLiterals[0])

static const char PROTO_STR[] = "__proto__";
static const char LENGTH_STR[] = "length";

static const char TYPE_STRINGS[] =
  "undefined\0boolean\0number\0string\0function\0object\0symbol\0bigint";
// 0          10       18      25      32        41      48      55

// Character offsets into TYPE_STRINGS
static const uint8_t typeStringOffsetByType[VM_T_END] = {
  0 , /* VM_T_UNDEFINED   */
  41, /* VM_T_NULL        */
  10, /* VM_T_BOOLEAN     */
  18, /* VM_T_NUMBER      */
  25, /* VM_T_STRING      */
  32, /* VM_T_FUNCTION    */
  41, /* VM_T_OBJECT      */
  41, /* VM_T_ARRAY       */
  41, /* VM_T_UINT8_ARRAY */
  32, /* VM_T_CLASS       */
  48, /* VM_T_SYMBOL      */
  55, /* VM_T_BIG_INT     */
};

// TeTypeCode -> mvm_TeType
static const uint8_t typeByTC[TC_END] = {
  VM_T_END,         /* TC_REF_TOMBSTONE          */
  VM_T_NUMBER,      /* TC_REF_INT32              */
  VM_T_NUMBER,      /* TC_REF_FLOAT64            */
  VM_T_STRING,      /* TC_REF_STRING             */
  VM_T_STRING,      /* TC_REF_INTERNED_STRING    */
  VM_T_FUNCTION,    /* TC_REF_FUNCTION           */
  VM_T_FUNCTION,    /* TC_REF_HOST_FUNC          */
  VM_T_UINT8_ARRAY, /* TC_REF_UINT8_ARRAY        */
  VM_T_SYMBOL,      /* TC_REF_SYMBOL             */
  VM_T_CLASS,       /* TC_REF_CLASS              */
  VM_T_END,         /* TC_REF_VIRTUAL            */
  VM_T_END,         /* TC_REF_RESERVED_1         */
  VM_T_OBJECT,      /* TC_REF_PROPERTY_LIST      */
  VM_T_ARRAY,       /* TC_REF_ARRAY              */
  VM_T_ARRAY,       /* TC_REF_FIXED_LENGTH_ARRAY */
  VM_T_FUNCTION,    /* TC_REF_CLOSURE            */
  VM_T_NUMBER,      /* TC_VAL_INT14              */
  VM_T_UNDEFINED,   /* TC_VAL_UNDEFINED          */
  VM_T_NULL,        /* TC_VAL_NULL               */
  VM_T_BOOLEAN,     /* TC_VAL_TRUE               */
  VM_T_BOOLEAN,     /* TC_VAL_FALSE              */
  VM_T_NUMBER,      /* TC_VAL_NAN                */
  VM_T_NUMBER,      /* TC_VAL_NEG_ZERO           */
  VM_T_UNDEFINED,   /* TC_VAL_DELETED            */
  VM_T_STRING,      /* TC_VAL_STR_LENGTH         */
  VM_T_STRING,      /* TC_VAL_STR_PROTO          */
  VM_T_FUNCTION,    /* TC_VAL_NO_OP_FUNC         */
};

#define GC_ALLOCATE_TYPE(vm, type, typeCode) \
  (type*)mvm_allocateWithConstantHeader(vm, vm_makeHeaderWord(vm, typeCode, sizeof (type)), 2 + sizeof (type))

#if MVM_SUPPORT_FLOAT
static int32_t mvm_float64ToInt32(MVM_FLOAT64 value);
#endif

#if MVM_VERY_EXPENSIVE_MEMORY_CHECKS
  #define VM_POTENTIAL_GC_POINT(vm) do { \
    mvm_runGC(vm, false); \
    VM_EXEC_SAFE_MODE(vm->gc_potentialCycleNumber++;) \
  } while (0)
#else
  #define VM_POTENTIAL_GC_POINT(vm) do { \
    VM_EXEC_SAFE_MODE(vm->gc_potentialCycleNumber++;) \
  } while (0)
#endif

// MVM_LOCAL declares a local variable whose value would become invalidated if
// the GC performs a cycle. All access to the local should use MVM_GET_LOCAL AND
// MVM_SET_LOCAL. This only needs to be used for pointer values or values that
// might hold a pointer.
#if MVM_SAFE_MODE
#define MVM_LOCAL(type, varName, initial) type varName ## Value = initial; uint8_t _ ## varName ## PotentialCycleNumber = vm->gc_potentialCycleNumber
#define MVM_GET_LOCAL(varName) (mvm_checkValueAccess(vm, _ ## varName ## PotentialCycleNumber), varName ## Value)
#define MVM_SET_LOCAL(varName, value) varName ## Value = value; _ ## varName ## PotentialCycleNumber = vm->gc_potentialCycleNumber
#else
#define MVM_LOCAL(type, varName, initial) type varName = initial
#define MVM_GET_LOCAL(varName) (varName)
#define MVM_SET_LOCAL(varName, value) varName = value
#endif // MVM_SAFE_MODE

// Various things require the registers (vm->stack->reg) to be up to date
#define VM_ASSERT_NOT_USING_CACHED_REGISTERS(vm) \
  VM_ASSERT(vm, !vm->stack || !vm->stack->reg.usingCachedRegisters)

/**
 * (this used to be in the port file but I've moved it out because the semantics
 * may be confusing and are difficult to communicate clearly. See
 * https://github.com/coder-mike/microvium/issues/47)
 *
 * Set to 1 to enable overflow checking for 32 bit integers in compliance with
 * the ECMAScript standard (ES262).
 *
 * If set to 0, then operations on 32-bit signed integers have wrap-around
 * (overflow) behavior, like the typical runtime behavior when adding 32-bit
 * signed integers in C.
 *
 * Explanation: Microvium tries to use 32-bit integer arithmetic where possible,
 * because it's more efficient than the standard 64-bit floating point
 * operations, especially on small microcontrollers. To give the appearance of
 * 64-bit floating point, Microvium needs to check when the result of such
 * operations overflows the 32-bit range and needs to be re-calculated using
 * proper 64-bit floating point operations. These overflow checks can be
 * disabled to improve performance and reduce engine size.
 *
 * Example: `2_000_000_000 + 2_000_000_000` will add to:
 *
 *   - `4_000_000_000` if `MVM_PORT_INT32_OVERFLOW_CHECKS` is `1`
 *   - `-294_967_296` if `MVM_PORT_INT32_OVERFLOW_CHECKS` is `0`
 *
 */
#ifndef MVM_PORT_INT32_OVERFLOW_CHECKS
#define MVM_PORT_INT32_OVERFLOW_CHECKS 1
#endif



#if MVM_IMPORT_MATH
#include "math.h"
#endif

/**
 * Public API to call into the VM to run the given function with the given
 * arguments (also contains the run loop).
 *
 * Control returns from `mvm_call` either when it hits an error or when it
 * executes a RETURN instruction within the called function.
 *
 * If the return code is MVM_E_UNCAUGHT_EXCEPTION then `out_result` points to the exception.
 */
TeError mvm_call(VM* vm, Value targetFunc, Value* out_result, Value* args, uint8_t argCount) {
  /*
  Note: when microvium calls the host, only `mvm_call` is on the call stack.
  This is for the objective of being lightweight. Each stack frame in an
  embedded environment can be quite expensive in terms of memory because of all
  the general-purpose registers that need to be preserved.
  */

  // -------------------------------- Definitions -----------------------------

  #define CACHE_REGISTERS() do { \
    VM_ASSERT(vm, reg->usingCachedRegisters == false); \
    VM_EXEC_SAFE_MODE(reg->usingCachedRegisters = true;) \
    lpProgramCounter = reg->lpProgramCounter; \
    pFrameBase = reg->pFrameBase; \
    pStackPointer = reg->pStackPointer; \
    VM_EXEC_SAFE_MODE(reg->pStackPointer = NULL;) \
    VM_EXEC_SAFE_MODE(reg->lpProgramCounter = LongPtr_new(NULL);) \
  } while (false)

  #define FLUSH_REGISTER_CACHE() do { \
    VM_ASSERT(vm, reg->usingCachedRegisters == true); \
    VM_EXEC_SAFE_MODE(reg->usingCachedRegisters = false;) \
    reg->lpProgramCounter = lpProgramCounter; \
    reg->pFrameBase = pFrameBase; \
    reg->pStackPointer = pStackPointer; \
    VM_EXEC_SAFE_MODE(pFrameBase = NULL;) \
    VM_EXEC_SAFE_MODE(pStackPointer = NULL;) \
    VM_EXEC_SAFE_MODE(lpProgramCounter = LongPtr_new(NULL);) \
  } while (false)

  #define READ_PGM_1(target) do { \
    VM_ASSERT(vm, reg->usingCachedRegisters == true); \
    target = LongPtr_read1(lpProgramCounter);\
    lpProgramCounter = LongPtr_add(lpProgramCounter, 1); \
  } while (false)

  #define READ_PGM_2(target) do { \
    VM_ASSERT(vm, reg->usingCachedRegisters == true); \
    target = LongPtr_read2_unaligned(lpProgramCounter); \
    lpProgramCounter = LongPtr_add(lpProgramCounter, 2); \
  } while (false)

  #define PUSH(v) do { \
    VM_ASSERT(vm, reg->usingCachedRegisters == true); \
    VM_ASSERT(vm, pStackPointer < getTopOfStackSpace(vm->stack)); \
    *pStackPointer = (v); \
    pStackPointer++; \
  } while (false)

  #if MVM_SAFE_MODE
    #define POP() vm_safePop(vm, --pStackPointer)
  #else
    #define POP() (*(--pStackPointer))
  #endif

  // Push the current registers onto the call stack
  #define PUSH_REGISTERS(lpReturnAddress) do { \
    VM_ASSERT(vm, VM_FRAME_BOUNDARY_VERSION == 2); \
    PUSH((uint16_t)(uintptr_t)pStackPointer - (uint16_t)(uintptr_t)pFrameBase); \
    PUSH(reg->closure); \
    PUSH(reg->argCountAndFlags); \
    PUSH((uint16_t)LongPtr_sub(lpReturnAddress, vm->lpBytecode)); \
  } while (false)

  // Inverse of PUSH_REGISTERS
  #define POP_REGISTERS() do { \
    VM_ASSERT(vm, VM_FRAME_BOUNDARY_VERSION == 2); \
    lpProgramCounter = LongPtr_add(vm->lpBytecode, POP()); \
    reg->argCountAndFlags = POP(); \
    reg->closure = POP(); \
    pStackPointer--; \
    pFrameBase = (uint16_t*)((uint8_t*)pStackPointer - *pStackPointer); \
    reg->pArgs = pFrameBase - VM_FRAME_BOUNDARY_SAVE_SIZE_WORDS - (reg->argCountAndFlags & AF_ARG_COUNT_MASK); \
  } while (false)

  // Push a catch target, where `handler` is the bytecode landing pad
  #define PUSH_CATCH_TARGET(handler) do { \
    /* Note: the value stored on the stack is essentially an auto-relative
    pointer stored as an Int14. It will always be negative because the catch
    target is always behind the stack pointer */ \
    int16_t temp = reg->pCatchTarget ? (int16_t)(reg->pCatchTarget - pStackPointer) : 0; \
    pStackPointer[0] = VirtualInt14_encode(vm, temp); \
    /* Note: pCatchTarget points to the base of the catch target, which the
    address before incrementing */  \
    reg->pCatchTarget = pStackPointer++; \
    PUSH(handler); \
  } while (false)

  // Unwinds the catch target at pStackPointer
  #define UNWIND_CATCH_TARGET() do { \
    int16_t temp = VirtualInt14_decode(vm, pStackPointer[0]); \
    reg->pCatchTarget = temp ? pStackPointer + temp : NULL; \
  } while (false)

  // Reinterpret reg1 as 8-bit signed
  #define SIGN_EXTEND_REG_1() reg1 = (uint16_t)((int16_t)((int8_t)reg1))

  #define INSTRUCTION_RESERVED() VM_ASSERT(vm, false)

  // ------------------------------ Common Variables --------------------------

  VM_SAFE_CHECK_NOT_NULL(vm);
  if (argCount) VM_SAFE_CHECK_NOT_NULL(args);

  TeError err = MVM_E_SUCCESS;

  // These are cached values of `vm->stack->reg`, for quick access. Note: I've
  // chosen only the most important registers to be cached here, in the hope
  // that the C compiler will promote these eagerly to the CPU registers,
  // although it may choose not to.
  register uint16_t* pFrameBase;
  register uint16_t* pStackPointer; // Name is confusing. This is the stack pointer, not a pointer to the stack pointer.
  register LongPtr lpProgramCounter;

  // These are general-purpose scratch "registers". Note: probably the compiler
  // would be fine at performing register allocation if we didn't have specific
  // register variables, but having them explicit forces us to think about what
  // state is being used and designing the code to minimize it.
  register uint16_t reg1;
  register uint16_t reg2;
  register uint16_t reg3;
  uint16_t* regP1;
  uint16_t* regP2;
  LongPtr regLP1;

  uint16_t* globals;
  vm_TsRegisters* reg;
  vm_TsRegisters registerValuesAtEntry;

  #if MVM_DONT_TRUST_BYTECODE
    LongPtr maxProgramCounter;
    LongPtr minProgramCounter = getBytecodeSection(vm, BCS_ROM, &maxProgramCounter);
  #endif

  // Note: these initial values are not actually used, but some compilers give a
  // warning if you omit them.
  pFrameBase = 0;
  pStackPointer = 0;
  lpProgramCounter = 0;
  reg1 = 0;
  reg2 = 0;
  reg3 = 0;
  regP1 = NULL;
  regLP1 = NULL;

  // ------------------------------ Initialization ---------------------------

  CODE_COVERAGE(4); // Hit

  // Create the call stack if it doesn't exist
  if (!vm->stack) {
    CODE_COVERAGE(230); // Hit
    err = vm_createStackAndRegisters(vm);
    if (err != MVM_E_SUCCESS) {
      return err;
    }
  } else {
    CODE_COVERAGE(232); // Hit
  }

  globals = vm->globals;
  reg = &vm->stack->reg;

  registerValuesAtEntry = *reg;

  // Because we're coming from C-land, any exceptions that happen during
  // mvm_call should register as host errors
  reg->pCatchTarget = NULL;

  // Copy the state of the VM registers into the logical variables for quick access
  CACHE_REGISTERS();

  // ---------------------- Push host arguments to the stack ------------------

  // 126 is the maximum because we also push the `this` value implicitly
  if (argCount > (AF_ARG_COUNT_MASK - 1)) {
    CODE_COVERAGE_ERROR_PATH(220); // Not hit
    return MVM_E_TOO_MANY_ARGUMENTS;
  } else {
    CODE_COVERAGE(15); // Hit
  }

  err = vm_requireStackSpace(vm, pStackPointer, argCount + 2); // +1 for `this`, +1 for class if needed
  if (err != MVM_E_SUCCESS) goto SUB_EXIT;

  PUSH(targetFunc); // class or function
  if (reg->argCountAndFlags & AF_OVERRIDE_THIS) {
    CODE_COVERAGE_UNTESTED(662); // Hit
    // This is a bit of a hack. If mvm_call is called from mvm_callEx, then
    // mvm_callEx will have already set the `this` value on the stack in this
    // position.
    pStackPointer++;
    reg->argCountAndFlags &= ~AF_OVERRIDE_THIS;
  } else {
    CODE_COVERAGE(663); // Hit
    PUSH(VM_VALUE_UNDEFINED); // Push `this` pointer of undefined
  }

  TABLE_COVERAGE(argCount ? 1 : 0, 2, 513); // Hit 2/2
  reg1 = argCount;
  while (reg1--) {
    PUSH(*args++);
  }

  // ---------------------------- Call target function ------------------------

  reg1 /* argCountAndFlags */ = (argCount + 1) | AF_PUSHED_FUNCTION | AF_CALLED_FROM_HOST; // +1 for the `this` value
  reg2 /* target */ = vm_resolveIndirections(vm, targetFunc);
  reg3 /* cpsCallback */ = VM_VALUE_UNDEFINED;

  // When calling mvm_call from C, if the target is a class then we implicitly
  // `new` the class. This doesn't violate anything from the spec because it
  // doesn't affect JS calls, but it makes interacting with classes from C much
  // easier.
  if (deepTypeOf(vm, targetFunc) == TC_REF_CLASS) {
    goto SUB_NEW;
  } else {
    goto SUB_CALL;
  }

  goto SUB_CALL;

  // --------------------------------- Run Loop ------------------------------

  // This forms the start of the run loop
  //
  // Some useful debug watches:
  //
  //   - Program counter: /* pc */ (uint16_t)((uint8_t*)lpProgramCounter - (uint8_t*)vm->lpBytecode)
  //                      /* pc */ (uint16_t)((uint8_t*)vm->stack->reg.lpProgramCounter - (uint8_t*)vm->lpBytecode)
  //
  //   - Frame height (in words):  /* fh */ (uint16_t*)pStackPointer - (uint16_t*)pFrameBase
  //                               /* fh */ (uint16_t*)vm->stack->reg.pStackPointer - (uint16_t*)vm->stack->reg.pFrameBase
  //
  //   - Frame:                    /* frame */ (uint16_t*)pFrameBase,10
  //                               /* frame */ (uint16_t*)vm->stack->reg.pFrameBase,10
  //
  //   - Stack height (in words): /* sp */ (uint16_t*)pStackPointer - (uint16_t*)(vm->stack + 1)
  //                              /* sp */ (uint16_t*)vm->stack->reg.pStackPointer - (uint16_t*)(vm->stack + 1)
  //
  //   - Frame base (in words): /* bp */ (uint16_t*)pFrameBase - (uint16_t*)(vm->stack + 1)
  //                            /* bp */ (uint16_t*)vm->stack->reg.pFrameBase - (uint16_t*)(vm->stack + 1)
  //
  //   - Arg count:             /* argc */ vm->stack->reg.argCountAndFlags & 0x7F
  //   - First 4 arg values:    /* args */ vm->stack->reg.pArgs,4
  //
  //   - Caller PC:             /* caller-pc */ pFrameBase[-1]
  //
  // Notes:
  //
  //   - The value of VM_VALUE_UNDEFINED is 0x001
  //   - If a value is _odd_, interpret it as a bytecode address by dividing by 2
  //

SUB_DO_NEXT_INSTRUCTION:
  CODE_COVERAGE(59); // Hit

  VM_ASSERT(vm, reg->usingCachedRegisters);

  #ifdef MVM_GAS_COUNTER
  if (vm->stopAfterNInstructions >= 0) {
    CODE_COVERAGE(650); // Hit
    if (vm->stopAfterNInstructions == 0) {
      CODE_COVERAGE(651); // Hit
      err = MVM_E_INSTRUCTION_COUNT_REACHED;
      goto SUB_EXIT;
    } else {
      CODE_COVERAGE(652); // Hit
      vm->stopAfterNInstructions--;
    }
  }
  #endif

  // Check we're within range
  #if MVM_DONT_TRUST_BYTECODE
  if ((lpProgramCounter < minProgramCounter) || (lpProgramCounter >= maxProgramCounter)) {
    VM_INVALID_BYTECODE(vm);
  }
  #endif

  // Check breakpoints
  #if MVM_INCLUDE_DEBUG_CAPABILITY
    if (vm->pBreakpoints) {
      TsBreakpoint* pBreakpoint = vm->pBreakpoints;
      uint16_t currentBytecodeAddress = LongPtr_sub(lpProgramCounter, vm->lpBytecode);
      do {
        if ((pBreakpoint->bytecodeAddress == -1) || (pBreakpoint->bytecodeAddress == (int)currentBytecodeAddress)) {
          FLUSH_REGISTER_CACHE();
          mvm_TfBreakpointCallback breakpointCallback = vm->breakpointCallback;
          if (breakpointCallback)
            breakpointCallback(vm, currentBytecodeAddress);
          CACHE_REGISTERS();
          break;
        }
        pBreakpoint = pBreakpoint->next;
      } while (pBreakpoint);
    }
  #endif // MVM_INCLUDE_DEBUG_CAPABILITY

  // Instruction bytes are divided into two nibbles
  READ_PGM_1(reg3);
  reg1 = reg3 & 0xF; // Primary opcode
  reg3 = reg3 >> 4;  // Secondary opcode or data

  if (reg3 >= VM_OP_DIVIDER_1) {
    CODE_COVERAGE(428); // Hit
    reg2 = POP();
  } else {
    CODE_COVERAGE(429); // Hit
  }

  VM_ASSERT(vm, reg3 < VM_OP_END);
  MVM_SWITCH(reg3, (VM_OP_END - 1)) {

/* ------------------------------------------------------------------------- */
/*                         VM_OP_LOAD_SMALL_LITERAL                          */
/*   Expects:                                                                */
/*     reg1: small literal ID                                                */
/* ------------------------------------------------------------------------- */

    MVM_CASE(VM_OP_LOAD_SMALL_LITERAL): {
      CODE_COVERAGE(60); // Hit
      TABLE_COVERAGE(reg1, smallLiteralsSize, 448); // Hit 11/12

      #if MVM_DONT_TRUST_BYTECODE
      if (reg1 >= smallLiteralsSize) {
        err = vm_newError(vm, MVM_E_INVALID_BYTECODE);
        goto SUB_EXIT;
      }
      #endif
      reg1 = smallLiterals[reg1];
      goto SUB_TAIL_POP_0_PUSH_REG1;
    }

/* ------------------------------------------------------------------------- */
/*                             VM_OP_LOAD_VAR_1                              */
/*   Expects:                                                                */
/*     reg1: variable index                                                  */
/* ------------------------------------------------------------------------- */

    MVM_CASE (VM_OP_LOAD_VAR_1):
      CODE_COVERAGE(61); // Hit
    SUB_OP_LOAD_VAR:
      reg1 = pStackPointer[-reg1 - 1];
      if (reg1 == VM_VALUE_DELETED) {
        err = vm_newError(vm, MVM_E_TDZ_ERROR);
        goto SUB_EXIT;
      }
      goto SUB_TAIL_POP_0_PUSH_REG1;

/* ------------------------------------------------------------------------- */
/*                            VM_OP_LOAD_SCOPED_1                            */
/*   Expects:                                                                */
/*     reg1: variable index                                                  */
/* ------------------------------------------------------------------------- */

    MVM_CASE (VM_OP_LOAD_SCOPED_1):
      CODE_COVERAGE(62); // Hit
      LongPtr lpVar;
    SUB_OP_LOAD_SCOPED:
      lpVar = vm_findScopedVariable(vm, reg1);
      reg1 = LongPtr_read2_aligned(lpVar);
      goto SUB_TAIL_POP_0_PUSH_REG1;

/* ------------------------------------------------------------------------- */
/*                             VM_OP_LOAD_ARG_1                              */
/*   Expects:                                                                */
/*     reg1: argument index                                                  */
/* ------------------------------------------------------------------------- */

    MVM_CASE (VM_OP_LOAD_ARG_1):
      CODE_COVERAGE(63); // Hit
      goto SUB_OP_LOAD_ARG;

/* ------------------------------------------------------------------------- */
/*                               VM_OP_CALL_1                                */
/*   Expects:                                                                */
/*     reg1: index into short-call table                                     */
/* ------------------------------------------------------------------------- */

    MVM_CASE (VM_OP_CALL_1): {
      CODE_COVERAGE_UNTESTED(66); // Not hit
      goto SUB_CALL_SHORT;
    }

/* ------------------------------------------------------------------------- */
/*                               VM_OP_FIXED_ARRAY_NEW_1                     */
/*   Expects:                                                                */
/*     reg1: length of new fixed-length-array                                */
/* ------------------------------------------------------------------------- */

    MVM_CASE (VM_OP_FIXED_ARRAY_NEW_1): {
      CODE_COVERAGE_UNTESTED(134); // Not hit
      goto SUB_FIXED_ARRAY_NEW;
    }

/* ------------------------------------------------------------------------- */
/*                             VM_OP_EXTENDED_1                              */
/*   Expects:                                                                */
/*     reg1: vm_TeOpcodeEx1                                                  */
/* ------------------------------------------------------------------------- */

    MVM_CASE (VM_OP_EXTENDED_1):
      CODE_COVERAGE(69); // Hit
      goto SUB_OP_EXTENDED_1;

/* ------------------------------------------------------------------------- */
/*                             VM_OP_EXTENDED_2                              */
/*   Expects:                                                                */
/*     reg1: vm_TeOpcodeEx2                                                  */
/* ------------------------------------------------------------------------- */

    MVM_CASE (VM_OP_EXTENDED_2):
      CODE_COVERAGE(70); // Hit
      goto SUB_OP_EXTENDED_2;

/* ------------------------------------------------------------------------- */
/*                             VM_OP_EXTENDED_3                              */
/*   Expects:                                                                */
/*     reg1: vm_TeOpcodeEx3                                                  */
/* ------------------------------------------------------------------------- */

    MVM_CASE (VM_OP_EXTENDED_3):
      CODE_COVERAGE(71); // Hit
      goto SUB_OP_EXTENDED_3;

/* ------------------------------------------------------------------------- */
/*                                VM_OP_CALL_5                               */
/*   Expects:                                                                */
/*     reg1: argCount                                                        */
/* ------------------------------------------------------------------------- */

    MVM_CASE (VM_OP_CALL_5): {
      /* Note: this isn't actually used at the moment, because we don't have the
      static analysis to statically determine the target. But my expectation is
      that when we have this static analysis, most function calls are going to
      take this form, where the arg count is small and the target is statically
      determined, but where it's not worth it to put the call into the
      short-call table. */
      CODE_COVERAGE_UNTESTED(72); // Not hit
      // Uses 16 bit literal for function offset
      READ_PGM_2(reg2);
      reg3 /* scope */ = VM_VALUE_UNDEFINED;
      goto SUB_CALL_BYTECODE_FUNC;
    }

/* ------------------------------------------------------------------------- */
/*                             VM_OP_STORE_VAR_1                             */
/*   Expects:                                                                */
/*     reg1: variable index relative to stack pointer                        */
/*     reg2: value to store                                                  */
/* ------------------------------------------------------------------------- */

    MVM_CASE (VM_OP_STORE_VAR_1): {
      CODE_COVERAGE(73); // Hit
    SUB_OP_STORE_VAR:
      // Note: the value to store has already been popped off the stack at this
      // point. The index 0 refers to the slot currently at the top of the
      // stack.
      pStackPointer[-reg1 - 1] = reg2;
      goto SUB_TAIL_POP_0_PUSH_0;
    }

/* ------------------------------------------------------------------------- */
/*                           VM_OP_STORE_SCOPED_1                            */
/*   Expects:                                                                */
/*     reg1: variable index                                                  */
/*     reg2: value to store                                                  */
/* ------------------------------------------------------------------------- */

    MVM_CASE (VM_OP_STORE_SCOPED_1): {
      CODE_COVERAGE(74); // Hit
      LongPtr lpVar;
    SUB_OP_STORE_SCOPED:
      lpVar = vm_findScopedVariable(vm, reg1);
      Value* pVar = (Value*)LongPtr_truncate(vm, lpVar);
      // It would be an illegal operation to write to a closure variable stored in ROM
      VM_BYTECODE_ASSERT(vm, lpVar == LongPtr_new(pVar));
      *pVar = reg2;
      goto SUB_TAIL_POP_0_PUSH_0;
    }

/* ------------------------------------------------------------------------- */
/*                            VM_OP_ARRAY_GET_1                              */
/*   Expects:                                                                */
/*     reg1: item index (4-bit)                                             */
/*     reg2: reference to array                                              */
/* ------------------------------------------------------------------------- */

    MVM_CASE (VM_OP_ARRAY_GET_1): {
      CODE_COVERAGE_UNTESTED(75); // Not hit

      // I think it makes sense for this instruction only to be an optimization for fixed-length arrays
      VM_ASSERT(vm, deepTypeOf(vm, reg2) == TC_REF_FIXED_LENGTH_ARRAY);
      regLP1 = DynamicPtr_decode_long(vm, reg2);
      // These indexes should be compiler-generated, so they should never be out of range
      VM_ASSERT(vm, reg1 < (vm_getAllocationSize_long(regLP1) >> 1));
      regLP1 = LongPtr_add(regLP1, reg2 << 1);
      reg1 = LongPtr_read2_aligned(regLP1);
      goto SUB_TAIL_POP_0_PUSH_REG1;
    }

/* ------------------------------------------------------------------------- */
/*                            VM_OP_ARRAY_SET_1                              */
/*   Expects:                                                                */
/*     reg1: item index (4-bit)                                              */
/* ------------------------------------------------------------------------- */

    MVM_CASE (VM_OP_ARRAY_SET_1): {
      CODE_COVERAGE_UNTESTED(76); // Not hit
      reg2 = POP(); // array reference
      // I think it makes sense for this instruction only to be an optimization for fixed-length arrays
      VM_ASSERT(vm, deepTypeOf(vm, reg3) == TC_REF_FIXED_LENGTH_ARRAY);
      // We can only write to it if it's in RAM, so it must be a short-pointer
      regP1 = (Value*)ShortPtr_decode(vm, reg3);
      // These indexes should be compiler-generated, so they should never be out of range
      VM_ASSERT(vm, reg1 < (vm_getAllocationSize(regP1) >> 1));
      regP1[reg1] = reg2;
      goto SUB_TAIL_POP_0_PUSH_0;
    }

/* ------------------------------------------------------------------------- */
/*                              VM_OP_NUM_OP                                 */
/*   Expects:                                                                */
/*     reg1: vm_TeNumberOp                                                   */
/*     reg2: first popped operand                                            */
/* ------------------------------------------------------------------------- */

    MVM_CASE (VM_OP_NUM_OP): {
      CODE_COVERAGE(77); // Hit
      goto SUB_OP_NUM_OP;
    } // End of case VM_OP_NUM_OP

/* ------------------------------------------------------------------------- */
/*                              VM_OP_BIT_OP                                 */
/*   Expects:                                                                */
/*     reg1: vm_TeBitwiseOp                                                  */
/*     reg2: first popped operand                                            */
/* ------------------------------------------------------------------------- */

    MVM_CASE (VM_OP_BIT_OP): {
      CODE_COVERAGE(92); // Hit
      goto SUB_OP_BIT_OP;
    }

  } // End of primary switch

  // All cases should loop explicitly back
  VM_ASSERT_UNREACHABLE(vm);

/* ------------------------------------------------------------------------- */
/*                             SUB_OP_LOAD_ARG                               */
/*   Expects:                                                                */
/*     reg1: argument index                                                  */
/* ------------------------------------------------------------------------- */
SUB_OP_LOAD_ARG: {
  CODE_COVERAGE(32); // Hit
  reg2 /* argCountAndFlags */ = reg->argCountAndFlags;
  if (reg1 /* argIndex */ < (reg2 & AF_ARG_COUNT_MASK) /* argCount */) {
    CODE_COVERAGE(64); // Hit
    reg1 /* result */ = reg->pArgs[reg1 /* argIndex */];
  } else {
    CODE_COVERAGE(65); // Hit
    reg1 = VM_VALUE_UNDEFINED;
  }
  goto SUB_TAIL_POP_0_PUSH_REG1;
}

/* ------------------------------------------------------------------------- */
/*                               SUB_CALL_SHORT                               */
/*   Expects:                                                                */
/*     reg1: index into short-call table                                     */
/* ------------------------------------------------------------------------- */

SUB_CALL_SHORT: {
  CODE_COVERAGE_UNTESTED(173); // Not hit
  LongPtr lpShortCallTable = getBytecodeSection(vm, BCS_SHORT_CALL_TABLE, NULL);
  LongPtr lpShortCallTableEntry = LongPtr_add(lpShortCallTable, reg1 * sizeof (vm_TsShortCallTableEntry));

  #if MVM_SAFE_MODE
    LongPtr lpShortCallTableEnd;
    getBytecodeSection(vm, BCS_SHORT_CALL_TABLE, &lpShortCallTableEnd);
    VM_ASSERT(vm, lpShortCallTableEntry < lpShortCallTableEnd);
  #endif

  reg2 /* target */ = LongPtr_read2_aligned(lpShortCallTableEntry);
  lpShortCallTableEntry = LongPtr_add(lpShortCallTableEntry, 2);

  // Note: reg1 holds the new argCountAndFlags, but the flags are zero in this situation
  reg1 /* argCountAndFlags */ = LongPtr_read1(lpShortCallTableEntry);

  reg3 /* scope */ = VM_VALUE_UNDEFINED;

  // The high bit of function indicates if this is a call to the host
  bool isHostCall = reg2 & 1;

  if (isHostCall) {
    CODE_COVERAGE_UNTESTED(67); // Not hit
    goto SUB_CALL_HOST_COMMON;
  } else {
    CODE_COVERAGE_UNTESTED(68); // Not hit
    reg2 >>= 1;
    goto SUB_CALL_BYTECODE_FUNC;
  }
} // SUB_CALL_SHORT

/* ------------------------------------------------------------------------- */
/*                              SUB_OP_BIT_OP                                */
/*   Expects:                                                                */
/*     reg1: vm_TeBitwiseOp                                                  */
/*     reg2: first popped operand                                            */
/* ------------------------------------------------------------------------- */
SUB_OP_BIT_OP: {
  int32_t reg1I = 0;
  int32_t reg2I = 0;
  int8_t reg2B = 0;

  reg3 = reg1;

  // Convert second operand to an int32
  reg2I = mvm_toInt32(vm, reg2);

  // If it's a binary operator, then we pop a second operand
  if (reg3 < VM_BIT_OP_DIVIDER_2) {
    CODE_COVERAGE(117); // Hit
    reg1 = POP();
    reg1I = mvm_toInt32(vm, reg1);

    // If we're doing a shift operation, the operand is in the 0-32 range
    if (reg3 < VM_BIT_OP_END_OF_SHIFT_OPERATORS) {
      reg2B = reg2I & 0x1F;
    }
  } else {
    CODE_COVERAGE(118); // Hit
  }

  VM_ASSERT(vm, reg3 < VM_BIT_OP_END);
  MVM_SWITCH (reg3, (VM_BIT_OP_END - 1)) {
    MVM_CASE(VM_BIT_OP_SHR_ARITHMETIC): {
      CODE_COVERAGE(93); // Hit
      reg1I = reg1I >> reg2B;
      break;
    }
    MVM_CASE(VM_BIT_OP_SHR_LOGICAL): {
      CODE_COVERAGE(94); // Hit
      // Cast the number to unsigned int so that the C interprets the shift
      // as unsigned/logical rather than signed/arithmetic.
      reg1I = (int32_t)((uint32_t)reg1I >> reg2B);
      #if MVM_SUPPORT_FLOAT && MVM_PORT_INT32_OVERFLOW_CHECKS
        // This is a rather annoying edge case if you ask me, since all
        // other bitwise operations yield signed int32 results every time.
        // If the shift is by exactly zero units, then negative numbers
        // become positive and overflow the signed-32 bit type. Since we
        // don't have an unsigned 32 bit type, this means they need to be
        // extended to floats.
        // https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Operators/Bitwise_Operators#Signed_32-bit_integers
        if ((reg2B == 0) & (reg1I < 0)) {
          FLUSH_REGISTER_CACHE();
          reg1 = mvm_newNumber(vm, (MVM_FLOAT64)((uint32_t)reg1I));
          CACHE_REGISTERS();
          goto SUB_TAIL_POP_0_PUSH_REG1;
        }
      #endif // MVM_PORT_INT32_OVERFLOW_CHECKS
      break;
    }
    MVM_CASE(VM_BIT_OP_SHL): {
      CODE_COVERAGE(95); // Hit
      reg1I = reg1I << reg2B;
      break;
    }
    MVM_CASE(VM_BIT_OP_OR): {
      CODE_COVERAGE(96); // Hit
      reg1I = reg1I | reg2I;
      break;
    }
    MVM_CASE(VM_BIT_OP_AND): {
      CODE_COVERAGE(97); // Hit
      reg1I = reg1I & reg2I;
      break;
    }
    MVM_CASE(VM_BIT_OP_XOR): {
      CODE_COVERAGE(98); // Hit
      reg1I = reg1I ^ reg2I;
      break;
    }
    MVM_CASE(VM_BIT_OP_NOT): {
      CODE_COVERAGE(99); // Hit
      reg1I = ~reg2I;
      break;
    }
  }

  CODE_COVERAGE(101); // Hit

  // Convert the result from a 32-bit integer
  if ((reg1I >= VM_MIN_INT14) && (reg1I <= VM_MAX_INT14)) {
    CODE_COVERAGE(34); // Hit
    reg1 = VirtualInt14_encode(vm, (uint16_t)reg1I);
  } else {
    CODE_COVERAGE(35); // Hit
    FLUSH_REGISTER_CACHE();
    reg1 = mvm_newInt32(vm, reg1I);
    CACHE_REGISTERS();
  }

  goto SUB_TAIL_POP_0_PUSH_REG1;
} // End of SUB_OP_BIT_OP

/* ------------------------------------------------------------------------- */
/*                             SUB_OP_EXTENDED_1                             */
/*   Expects:                                                                */
/*     reg1: vm_TeOpcodeEx1                                                  */
/* ------------------------------------------------------------------------- */

SUB_OP_EXTENDED_1: {
  CODE_COVERAGE(102); // Hit

  reg3 = reg1;

  VM_ASSERT(vm, reg3 <= VM_OP1_END);
  MVM_SWITCH (reg3, VM_OP1_END - 1) {

/* ------------------------------------------------------------------------- */
/*                              VM_OP1_RETURN                                */
/*   Expects:                                                                */
/*     reg1: vm_TeOpcodeEx1                                                  */
/* ------------------------------------------------------------------------- */

    MVM_CASE (VM_OP1_RETURN): {
      CODE_COVERAGE(107); // Hit
      reg1 = POP();
      goto SUB_RETURN;
    }

    MVM_CASE (VM_OP1_THROW): {
      CODE_COVERAGE(106); // Hit

      reg1 = POP(); // The exception value
      goto SUB_THROW;
    }

/* ------------------------------------------------------------------------- */
/*                                 VM_OP1_CLOSURE_NEW                        */
/*   Expects:                                                                */
/*     reg3: vm_TeOpcodeEx1                                                  */
/* ------------------------------------------------------------------------- */

    MVM_CASE (VM_OP1_CLOSURE_NEW): {
      CODE_COVERAGE(599); // Hit

      FLUSH_REGISTER_CACHE();
      Value* pClosure = mvm_allocate(vm, 4, TC_REF_CLOSURE);
      CACHE_REGISTERS();
      reg1 = ShortPtr_encode(vm, pClosure);
      *pClosure++ = POP(); // The function pointer
      *pClosure = reg->closure; // Capture the current scope

      goto SUB_TAIL_POP_0_PUSH_REG1;
    }

/* ------------------------------------------------------------------------- */
/*                          VM_OP1_NEW                                       */
/*   Expects:                                                                */
/*     Nothing                                                               */
/* ------------------------------------------------------------------------- */

    MVM_CASE (VM_OP1_NEW): {
      CODE_COVERAGE(347); // Hit
      READ_PGM_1(reg1); // arg count
      reg1 /*argCountAndFlags*/ |= AF_PUSHED_FUNCTION;
      goto SUB_NEW;
    }

/* ------------------------------------------------------------------------- */
/*                                 VM_OP1_SCOPE_NEW                          */
/*   Expects:                                                                */
/*     Nothing                                                               */
/* ------------------------------------------------------------------------- */

    MVM_CASE (VM_OP1_SCOPE_NEW): {
      CODE_COVERAGE(605); // Hit
      // A SCOPE_NEW is just like a SCOPE_PUSH without capturing the parent
      reg3 /*capture parent*/ = false;
      goto SUB_OP_SCOPE_PUSH_OR_NEW;
    }

/* ------------------------------------------------------------------------- */
/*                              VM_OP1_TYPE_CODE_OF                          */
/*   Expects:                                                                */
/*     Nothing                                                               */
/* ------------------------------------------------------------------------- */

    MVM_CASE (VM_OP1_TYPE_CODE_OF): {
      CODE_COVERAGE_UNTESTED(607); // Not hit
      reg1 = POP();
      reg1 = mvm_typeOf(vm, reg1);
      goto SUB_TAIL_POP_0_PUSH_REG1;
    }

/* ------------------------------------------------------------------------- */
/*                              VM_OP1_POP                                   */
/*   Expects:                                                                */
/*     Nothing                                                               */
/* ------------------------------------------------------------------------- */

    MVM_CASE (VM_OP1_POP): {
      CODE_COVERAGE(138); // Hit
      pStackPointer--;
      goto SUB_TAIL_POP_0_PUSH_0;
    }

/* ------------------------------------------------------------------------- */
/*                              VM_OP1_TYPEOF                                */
/*   Expects:                                                                */
/*     Nothing                                                               */
/* ------------------------------------------------------------------------- */

    MVM_CASE (VM_OP1_TYPEOF): {
      CODE_COVERAGE(167); // Hit
      // TODO: This is should really be done using some kind of built-in helper
      // function, but we don't support those yet. The trouble with this
      // implementation is that it's doing a string allocation every time. Also
      // the new string is not an interned string so it's expensive to compare
      // `typeof x === y`. Basically this is just a stop-gap.
      reg1 = mvm_typeOf(vm, pStackPointer[-1]);
      VM_ASSERT(vm, reg1 < sizeof typeStringOffsetByType);
      reg1 = typeStringOffsetByType[reg1];
      VM_ASSERT(vm, reg1 < sizeof(TYPE_STRINGS) - 1);
      const char* str = &TYPE_STRINGS[reg1];
      FLUSH_REGISTER_CACHE();
      reg1 = vm_newStringFromCStrNT(vm, str);
      CACHE_REGISTERS();
      goto SUB_TAIL_POP_1_PUSH_REG1;
    }

/* ------------------------------------------------------------------------- */
/*                              VM_OP1_OBJECT_NEW                            */
/*   Expects:                                                                */
/*     (nothing)                                                             */
/* ------------------------------------------------------------------------- */

    MVM_CASE (VM_OP1_OBJECT_NEW): {
      CODE_COVERAGE(112); // Hit
      FLUSH_REGISTER_CACHE();
      TsPropertyList* pObject = GC_ALLOCATE_TYPE(vm, TsPropertyList, TC_REF_PROPERTY_LIST);
      CACHE_REGISTERS();
      reg1 = ShortPtr_encode(vm, pObject);
      pObject->dpNext = VM_VALUE_NULL;
      pObject->dpProto = VM_VALUE_NULL;
      goto SUB_TAIL_POP_0_PUSH_REG1;
    }

/* ------------------------------------------------------------------------- */
/*                               VM_OP1_LOGICAL_NOT                          */
/*   Expects:                                                                */
/*     (nothing)                                                             */
/* ------------------------------------------------------------------------- */

    MVM_CASE (VM_OP1_LOGICAL_NOT): {
      CODE_COVERAGE(113); // Hit
      reg2 = POP(); // value to negate
      reg1 = mvm_toBool(vm, reg2) ? VM_VALUE_FALSE : VM_VALUE_TRUE;
      goto SUB_TAIL_POP_0_PUSH_REG1;
    }

/* ------------------------------------------------------------------------- */
/*                              VM_OP1_OBJECT_GET_1                          */
/*   Expects:                                                                */
/*     reg1: objectValue                                                     */
/*     reg2: propertyName                                                    */
/* ------------------------------------------------------------------------- */

    MVM_CASE (VM_OP1_OBJECT_GET_1): {
      CODE_COVERAGE(114); // Hit
      FLUSH_REGISTER_CACHE();
      err = getProperty(vm, reg->pStackPointer - 2, reg->pStackPointer - 1, reg->pStackPointer - 2);
      CACHE_REGISTERS();
      if (err != MVM_E_SUCCESS) goto SUB_EXIT;
      goto SUB_TAIL_POP_1_PUSH_0;
    }

/* ------------------------------------------------------------------------- */
/*                                 VM_OP1_ADD                                */
/*   Expects:                                                                */
/*     reg1: left operand                                                    */
/*     reg2: right operand                                                   */
/* ------------------------------------------------------------------------- */

    MVM_CASE (VM_OP1_ADD): {
      CODE_COVERAGE(115); // Hit
      reg1 = pStackPointer[-2];
      reg2 = pStackPointer[-1];

      // Special case for adding unsigned 12 bit numbers, for example in most
      // loops. 12 bit unsigned addition does not require any overflow checks
      if (Value_isVirtualUInt12(reg1) && Value_isVirtualUInt12(reg2)) {
        CODE_COVERAGE(116); // Hit
        reg1 = reg1 + reg2 - VirtualInt14_encode(vm, 0);
        goto SUB_TAIL_POP_2_PUSH_REG1;
      } else {
        CODE_COVERAGE(119); // Hit
      }
      if (vm_isString(vm, reg1) || vm_isString(vm, reg2)) {
        CODE_COVERAGE(120); // Hit
        FLUSH_REGISTER_CACHE();
        // Note: the intermediate values are saved back to the stack so that
        // they're preserved if there is a GC collection. Even these conversions
        // can trigger a GC collection
        reg->pStackPointer[-2] = vm_convertToString(vm, reg->pStackPointer[-2]);
        reg->pStackPointer[-1] = vm_convertToString(vm, reg->pStackPointer[-1]);
        reg1 = vm_concat(vm, &reg->pStackPointer[-2], &reg->pStackPointer[-1]);
        CACHE_REGISTERS();
        goto SUB_TAIL_POP_2_PUSH_REG1;
      } else {
        CODE_COVERAGE(121); // Hit
        // Interpret like any of the other numeric operations
        // TODO: If VM_NUM_OP_ADD_NUM might cause a GC collection, then we shouldn't be popping here
        POP();
        reg1 = VM_NUM_OP_ADD_NUM;
        goto SUB_OP_NUM_OP;
      }
    }

/* ------------------------------------------------------------------------- */
/*                                 VM_OP1_EQUAL                              */
/*   Expects:                                                                */
/*     reg1: left operand                                                    */
/*     reg2: right operand                                                   */
/* ------------------------------------------------------------------------- */

    MVM_CASE (VM_OP1_EQUAL): {
      CODE_COVERAGE(122); // Hit
      // TODO: This popping should be done on the egress rather than the ingress
      reg2 = POP();
      reg1 = POP();
      FLUSH_REGISTER_CACHE();
      bool eq = mvm_equal(vm, reg1, reg2);
      CACHE_REGISTERS();
      if (eq) {
        CODE_COVERAGE(483); // Hit
        reg1 = VM_VALUE_TRUE;
      } else {
        CODE_COVERAGE(484); // Hit
        reg1 = VM_VALUE_FALSE;
      }
      goto SUB_TAIL_POP_0_PUSH_REG1;
    }

/* ------------------------------------------------------------------------- */
/*                                 VM_OP1_NOT_EQUAL                          */
/*   Expects:                                                                */
/*     Nothing                                                               */
/* ------------------------------------------------------------------------- */

    MVM_CASE (VM_OP1_NOT_EQUAL): {
      reg1 = pStackPointer[-2];
      reg2 = pStackPointer[-1];
      // TODO: there seem to be so many places where we have to flush the
      // register cache, that I'm wondering if it's actually a net benefit. It
      // would be worth doing an experiment to see if the code size is smaller
      // without the register cache. Also, is it strictly necessary to flush all
      // the registers or can we maybe define a lightweight flush that just
      // flushes the stack pointer?
      FLUSH_REGISTER_CACHE();
      bool eq = mvm_equal(vm, reg1, reg2);
      CACHE_REGISTERS();
      if(eq) {
        CODE_COVERAGE(123); // Hit
        reg1 = VM_VALUE_FALSE;
      } else {
        CODE_COVERAGE(485); // Hit
        reg1 = VM_VALUE_TRUE;
      }
      goto SUB_TAIL_POP_2_PUSH_REG1;
    }

/* ------------------------------------------------------------------------- */
/*                                 VM_OP1_OBJECT_SET_1                       */
/*   Expects:                                                                */
/*     Nothing                                                               */
/* ------------------------------------------------------------------------- */

    MVM_CASE (VM_OP1_OBJECT_SET_1): {
      CODE_COVERAGE(124); // Hit
      FLUSH_REGISTER_CACHE();
      err = setProperty(vm, reg->pStackPointer - 3, reg->pStackPointer - 2, reg->pStackPointer - 1);
      CACHE_REGISTERS();
      if (err != MVM_E_SUCCESS) {
        CODE_COVERAGE_UNTESTED(265); // Not hit
        goto SUB_EXIT;
      } else {
        CODE_COVERAGE(322); // Hit
      }
      goto SUB_TAIL_POP_3_PUSH_0;
    }

  } // End of VM_OP_EXTENDED_1 switch

  // All cases should jump to whatever tail they intend. Nothing should get here
  VM_ASSERT_UNREACHABLE(vm);

} // End of SUB_OP_EXTENDED_1



/* ------------------------------------------------------------------------- */
/*                             SUB_THROW                                     */
/*   Expects:                                                                */
/*     reg1: The exception value                                             */
/* ------------------------------------------------------------------------- */

SUB_THROW: {
  // Find the closest catch block
  regP1 = reg->pCatchTarget;

  // If none, it's an uncaught exception
  if (regP1 == NULL) {
    CODE_COVERAGE(208); // Hit

    if (out_result) {
      *out_result = reg1;
    }
    err = MVM_E_UNCAUGHT_EXCEPTION;
    goto SUB_EXIT;
  } else {
    CODE_COVERAGE(209); // Hit
  }

  VM_ASSERT(vm, Value_isVirtualInt14(regP1[0]));

  VM_ASSERT(vm, pStackPointer >= getBottomOfStack(vm->stack));
  VM_ASSERT(vm, pStackPointer < getTopOfStackSpace(vm->stack));

  // Unwind the stack. regP1 is the stack pointer address we want to land up at
  while (pFrameBase > regP1) {
    CODE_COVERAGE(211); // Hit

    // Near the beginning of mvm_call, we set `catchTarget` to NULL
    // (and then restore at the end), which should direct exceptions through
    // the path of "uncaught exception" above, so no frame here should ever
    // be a host frame.
    VM_ASSERT(vm, !(reg->argCountAndFlags & AF_CALLED_FROM_HOST));

    // In the current frame structure, the size of the preceding frame is
    // saved 4 words ahead of the frame base
    pStackPointer = pFrameBase;
    POP_REGISTERS();
  }

  pStackPointer = regP1;

  // The next catch target is the outer one.
  UNWIND_CATCH_TARGET();

  // Jump to the catch block
  reg2 = pStackPointer[1];
  VM_ASSERT(vm, Value_isBytecodeMappedPtrOrWellKnown(reg2));
  lpProgramCounter = LongPtr_add(vm->lpBytecode, reg2 & ~1);

  // Push the exception to the stack for the catch block to use
  goto SUB_TAIL_POP_0_PUSH_REG1;
}

/* ------------------------------------------------------------------------- */
/*                              SUB_OP_SCOPE_PUSH_OR_NEW                     */
/*   Expects:                                                                */
/*     reg3: true if the last slot should be set to the parent closure       */
/* ------------------------------------------------------------------------- */
SUB_OP_SCOPE_PUSH_OR_NEW: {
  CODE_COVERAGE(645); // Hit
  READ_PGM_1(reg1); // Scope slot count
  FLUSH_REGISTER_CACHE();
  vm_scopePushOrNew(vm, reg1, reg3);
  CACHE_REGISTERS();
  goto SUB_TAIL_POP_0_PUSH_0;
}

/* ------------------------------------------------------------------------- */
/*                             SUB_OP_NUM_OP                                 */
/*   Expects:                                                                */
/*     reg1: vm_TeNumberOp                                                   */
/*     reg2: first popped operand                                            */
/* ------------------------------------------------------------------------- */
SUB_OP_NUM_OP: {
  CODE_COVERAGE(25); // Hit

  int32_t reg1I = 0;
  int32_t reg2I = 0;

  reg3 = reg1;

  // If it's a binary operator, then we pop a second operand
  if (reg3 < VM_NUM_OP_DIVIDER) {
    CODE_COVERAGE(440); // Hit
    reg1 = POP();

    if (toInt32Internal(vm, reg1, &reg1I) != MVM_E_SUCCESS) {
      CODE_COVERAGE(444); // Hit
      #if MVM_SUPPORT_FLOAT
      goto SUB_NUM_OP_FLOAT64;
      #endif // MVM_SUPPORT_FLOAT
    } else {
      CODE_COVERAGE(445); // Hit
    }
  } else {
    CODE_COVERAGE(441); // Hit
    reg1 = 0;
  }

  // Convert second operand to a int32 (or the only operand if it's a unary op)
  if (toInt32Internal(vm, reg2, &reg2I) != MVM_E_SUCCESS) {
    CODE_COVERAGE(442); // Hit
    // If we failed to convert to int32, then we need to process the operation as a float
    #if MVM_SUPPORT_FLOAT
    goto SUB_NUM_OP_FLOAT64;
    #endif // MVM_SUPPORT_FLOAT
  } else {
    CODE_COVERAGE(443); // Hit
  }

  VM_ASSERT(vm, reg3 < VM_NUM_OP_END);
  MVM_SWITCH (reg3, (VM_NUM_OP_END - 1)) {
    MVM_CASE(VM_NUM_OP_LESS_THAN): {
      CODE_COVERAGE(78); // Hit
      reg1 = reg1I < reg2I;
      goto SUB_TAIL_PUSH_REG1_BOOL;
    }
    MVM_CASE(VM_NUM_OP_GREATER_THAN): {
      CODE_COVERAGE(79); // Hit
      reg1 = reg1I > reg2I;
      goto SUB_TAIL_PUSH_REG1_BOOL;
    }
    MVM_CASE(VM_NUM_OP_LESS_EQUAL): {
      CODE_COVERAGE(80); // Hit
      reg1 = reg1I <= reg2I;
      goto SUB_TAIL_PUSH_REG1_BOOL;
    }
    MVM_CASE(VM_NUM_OP_GREATER_EQUAL): {
      CODE_COVERAGE(81); // Hit
      reg1 = reg1I >= reg2I;
      goto SUB_TAIL_PUSH_REG1_BOOL;
    }
    MVM_CASE(VM_NUM_OP_ADD_NUM): {
      CODE_COVERAGE(82); // Hit
      #if MVM_SUPPORT_FLOAT && MVM_PORT_INT32_OVERFLOW_CHECKS
        #if __has_builtin(__builtin_add_overflow)
          if (__builtin_add_overflow(reg1I, reg2I, &reg1I)) {
            goto SUB_NUM_OP_FLOAT64;
          }
        #else // No builtin overflow
          int32_t result = reg1I + reg2I;
          // Check overflow https://blog.regehr.org/archives/1139
          if (((reg1I ^ result) & (reg2I ^ result)) < 0) goto SUB_NUM_OP_FLOAT64;
          reg1I = result;
        #endif // No builtin overflow
      #else // No overflow checks
        reg1I = reg1I + reg2I;
      #endif
      break;
    }
    MVM_CASE(VM_NUM_OP_SUBTRACT): {
      CODE_COVERAGE(83); // Hit
      #if MVM_SUPPORT_FLOAT && MVM_PORT_INT32_OVERFLOW_CHECKS
        #if __has_builtin(__builtin_sub_overflow)
          if (__builtin_sub_overflow(reg1I, reg2I, &reg1I)) {
            goto SUB_NUM_OP_FLOAT64;
          }
        #else // No builtin overflow
          reg2I = -reg2I;
          int32_t result = reg1I + reg2I;
          // Check overflow https://blog.regehr.org/archives/1139
          if (((reg1I ^ result) & (reg2I ^ result)) < 0) goto SUB_NUM_OP_FLOAT64;
          reg1I = result;
        #endif // No builtin overflow
      #else // No overflow checks
        reg1I = reg1I - reg2I;
      #endif
      break;
    }
    MVM_CASE(VM_NUM_OP_MULTIPLY): {
      CODE_COVERAGE(84); // Hit
      #if MVM_SUPPORT_FLOAT && MVM_PORT_INT32_OVERFLOW_CHECKS
        #if __has_builtin(__builtin_mul_overflow)
          if (__builtin_mul_overflow(reg1I, reg2I, &reg1I)) {
            goto SUB_NUM_OP_FLOAT64;
          }
        #else // No builtin overflow
          // There isn't really an efficient way to determine multiplied
          // overflow on embedded devices without accessing the hardware
          // status registers. The fast shortcut here is to just assume that
          // anything more than 14-bit multiplication could overflow a 32-bit
          // integer.
          if (Value_isVirtualInt14(reg1) && Value_isVirtualInt14(reg2)) {
            reg1I = reg1I * reg2I;
          } else {
            goto SUB_NUM_OP_FLOAT64;
          }
        #endif // No builtin overflow
      #else // No overflow checks
        reg1I = reg1I * reg2I;
      #endif
      break;
    }
    MVM_CASE(VM_NUM_OP_DIVIDE): {
      CODE_COVERAGE(85); // Hit
      #if MVM_SUPPORT_FLOAT
        // With division, we leave it up to the user to write code that
        // performs integer division instead of floating point division, so
        // this instruction is always the case where they're doing floating
        // point division.
        goto SUB_NUM_OP_FLOAT64;
      #else // !MVM_SUPPORT_FLOAT
        err = vm_newError(vm, MVM_E_OPERATION_REQUIRES_FLOAT_SUPPORT);
        goto SUB_EXIT;
      #endif
    }
    MVM_CASE(VM_NUM_OP_DIVIDE_AND_TRUNC): {
      CODE_COVERAGE(86); // Hit
      if (reg2I == 0) {
        reg1I = 0;
        break;
      }
      reg1I = reg1I / reg2I;
      break;
    }
    MVM_CASE(VM_NUM_OP_REMAINDER): {
      CODE_COVERAGE(87); // Hit
      if (reg2I == 0) {
        CODE_COVERAGE(26); // Hit
        reg1 = VM_VALUE_NAN;
        goto SUB_TAIL_POP_0_PUSH_REG1;
      }
      CODE_COVERAGE(90); // Hit
      reg1I = reg1I % reg2I;
      break;
    }
    MVM_CASE(VM_NUM_OP_POWER): {
      CODE_COVERAGE(88); // Hit
      #if MVM_SUPPORT_FLOAT
        // Maybe in future we can we implement an integer version.
        goto SUB_NUM_OP_FLOAT64;
      #else // !MVM_SUPPORT_FLOAT
        err = vm_newError(vm, MVM_E_OPERATION_REQUIRES_FLOAT_SUPPORT);
        goto SUB_EXIT;
      #endif
    }
    MVM_CASE(VM_NUM_OP_NEGATE): {
      CODE_COVERAGE(89); // Hit
      #if MVM_SUPPORT_FLOAT && MVM_PORT_INT32_OVERFLOW_CHECKS
        // Note: Zero negates to negative zero, which is not representable as an int32
        if ((reg2I == INT32_MIN) || (reg2I == 0)) goto SUB_NUM_OP_FLOAT64;
      #endif
        reg1I = -reg2I;
      break;
    }
    MVM_CASE(VM_NUM_OP_UNARY_PLUS): {
      reg1I = reg2I;
      break;
    }
  } // End of switch vm_TeNumberOp for int32

  // Convert the result from a 32-bit integer
  if ((reg1I >= VM_MIN_INT14) && (reg1I <= VM_MAX_INT14)) {
    CODE_COVERAGE(103); // Hit
    reg1 = VirtualInt14_encode(vm, (uint16_t)reg1I);
  } else {
    CODE_COVERAGE(104); // Hit
    FLUSH_REGISTER_CACHE();
    reg1 = mvm_newInt32(vm, reg1I);
    CACHE_REGISTERS();
  }

  goto SUB_TAIL_POP_0_PUSH_REG1;
} // End of case SUB_OP_NUM_OP

/* ------------------------------------------------------------------------- */
/*                             SUB_OP_EXTENDED_2                             */
/*   Expects:                                                                */
/*     reg1: vm_TeOpcodeEx2                                                  */
/* ------------------------------------------------------------------------- */

SUB_OP_EXTENDED_2: {
  CODE_COVERAGE(127); // Hit
  reg3 = reg1;

  // All the ex-2 instructions have an 8-bit parameter. This is stored in
  // reg1 for consistency with 4-bit and 16-bit literal modes
  READ_PGM_1(reg1);

  // Some operations pop an operand off the stack. This goes into reg2
  if (reg3 < VM_OP2_DIVIDER_1) {
    CODE_COVERAGE(128); // Hit
    reg2 = POP();
  } else {
    CODE_COVERAGE(129); // Hit
  }

  VM_ASSERT(vm, reg3 < VM_OP2_END);
  MVM_SWITCH (reg3, (VM_OP2_END - 1)) {

/* ------------------------------------------------------------------------- */
/*                             VM_OP2_BRANCH_1                               */
/*   Expects:                                                                */
/*     reg1: signed 8-bit offset to branch to, encoded in 16-bit unsigned    */
/*     reg2: condition to branch on                                          */
/* ------------------------------------------------------------------------- */

    MVM_CASE (VM_OP2_BRANCH_1): {
      CODE_COVERAGE(130); // Hit
      SIGN_EXTEND_REG_1();
      goto SUB_BRANCH_COMMON;
    }

/* ------------------------------------------------------------------------- */
/*                             VM_OP2_STORE_ARG                              */
/*   Expects:                                                                */
/*     reg1: unsigned index of argument in which to store                    */
/*     reg2: value to store                                                  */
/* ------------------------------------------------------------------------- */

    MVM_CASE (VM_OP2_STORE_ARG): {
      CODE_COVERAGE_UNTESTED(131); // Not hit
      #if MVM_DONT_TRUST_BYTECODE
        // The ability to write to argument slots is intended as an optimization
        // feature to elide the parameter variable slots and instead use the
        // argument slots directly. But this only works if the optimizer can
        // prove that unprovided parameters are never written to (or that all
        // parameters are satisfied by arguments). If you don't trust the
        // optimizer, it's possible the callee attempts to write to the
        // caller-provided argument slots that don't exist.
        if (reg1 >= (reg->argCountAndFlags & AF_ARG_COUNT_MASK)) {
          err = vm_newError(vm, MVM_E_INVALID_BYTECODE);
          goto SUB_EXIT;
        }
      #endif
      reg->pArgs[reg1] = reg2;
      goto SUB_TAIL_POP_0_PUSH_0;
    }

/* ------------------------------------------------------------------------- */
/*                             VM_OP2_STORE_SCOPED_2                         */
/*   Expects:                                                                */
/*     reg1: unsigned index of global in which to store                      */
/*     reg2: value to store                                                  */
/* ------------------------------------------------------------------------- */

    MVM_CASE (VM_OP2_STORE_SCOPED_2): {
      CODE_COVERAGE(132); // Hit
      goto SUB_OP_STORE_SCOPED;
    }

/* ------------------------------------------------------------------------- */
/*                             VM_OP2_STORE_VAR_2                            */
/*   Expects:                                                                */
/*     reg1: unsigned index of variable in which to store, relative to SP    */
/*     reg2: value to store                                                  */
/* ------------------------------------------------------------------------- */

    MVM_CASE (VM_OP2_STORE_VAR_2): {
      CODE_COVERAGE_UNTESTED(133); // Not hit
      goto SUB_OP_STORE_VAR;
    }

/* ------------------------------------------------------------------------- */
/*                             VM_OP2_JUMP_1                                 */
/*   Expects:                                                                */
/*     reg1: signed 8-bit offset to branch to, encoded in 16-bit unsigned    */
/* ------------------------------------------------------------------------- */

    MVM_CASE (VM_OP2_JUMP_1): {
      CODE_COVERAGE(136); // Hit
      SIGN_EXTEND_REG_1();
      goto SUB_JUMP_COMMON;
    }

/* ------------------------------------------------------------------------- */
/*                             VM_OP2_CALL_HOST                              */
/*   Expects:                                                                */
/*     reg1: arg count                                                       */
/* ------------------------------------------------------------------------- */

    MVM_CASE (VM_OP2_CALL_HOST): {
      CODE_COVERAGE_UNTESTED(137); // Not hit
      // TODO: Unit tests for the host calling itself etc.

      // Put function index into reg2
      READ_PGM_1(reg2);
      // Note: reg1 is the argCount and also argCountAndFlags, because the flags
      // are all zero in this case. In particular, the target is specified as an
      // instruction literal, so `AF_PUSHED_FUNCTION` is false.
      goto SUB_CALL_HOST_COMMON;
    }

/* ------------------------------------------------------------------------- */
/*                             VM_OP2_CALL_3                                 */
/*   Expects:                                                                */
/*     reg1: arg count | isVoidCall flag 0x80                                */
/* ------------------------------------------------------------------------- */

    MVM_CASE (VM_OP2_CALL_3): {
      CODE_COVERAGE(142); // Hit

      // Note: The first 7 bits of `reg1` are the argument count, and the 8th
      // bit, as per the instruction format, is the `AF_VOID_CALLED` flag. None
      // of the CALL instruction formats use the high byte, so it's reserved for
      // general activation flags. Here we set flag AF_PUSHED_FUNCTION to
      // indicate that a `CALL_3` operation requires that the function pointer
      // is pushed to the stack and needs to be popped at the return point.

      reg3 /* cpsCallback */ = VM_VALUE_UNDEFINED;

      goto SUB_CALL_DYNAMIC;
    }


/* ------------------------------------------------------------------------- */
/*                             VM_OP2_CALL_6                              */
/*   Expects:                                                                */
/*     reg1: index into short-call table                                      */
/* ------------------------------------------------------------------------- */

    MVM_CASE (VM_OP2_CALL_6): {
      CODE_COVERAGE_UNTESTED(145); // Not hit
      goto SUB_CALL_SHORT;
    }

/* ------------------------------------------------------------------------- */
/*                             VM_OP2_LOAD_SCOPED_2                          */
/*   Expects:                                                                */
/*     reg1: unsigned closure scoped variable index                          */
/* ------------------------------------------------------------------------- */

    MVM_CASE (VM_OP2_LOAD_SCOPED_2): {
      CODE_COVERAGE(146); // Hit
      goto SUB_OP_LOAD_SCOPED;
    }

/* ------------------------------------------------------------------------- */
/*                              VM_OP2_LOAD_VAR_2                           */
/*   Expects:                                                                */
/*     reg1: unsigned variable index relative to stack pointer               */
/* ------------------------------------------------------------------------- */

    MVM_CASE (VM_OP2_LOAD_VAR_2): {
      CODE_COVERAGE_UNTESTED(147); // Not hit
      goto SUB_OP_LOAD_VAR;
    }

/* ------------------------------------------------------------------------- */
/*                              VM_OP2_LOAD_ARG_2                           */
/*   Expects:                                                                */
/*     reg1: unsigned variable index relative to stack pointer               */
/* ------------------------------------------------------------------------- */

    MVM_CASE (VM_OP2_LOAD_ARG_2): {
      CODE_COVERAGE_UNTESTED(148); // Not hit
      VM_NOT_IMPLEMENTED(vm);
      err = MVM_E_FATAL_ERROR_MUST_KILL_VM;
      goto SUB_EXIT;
    }

/* ------------------------------------------------------------------------- */
/*                              VM_OP2_EXTENDED_4                            */
/*   Expects:                                                                */
/*     reg1: The Ex-4 instruction opcode                                     */
/* ------------------------------------------------------------------------- */

    MVM_CASE (VM_OP2_EXTENDED_4): {
      CODE_COVERAGE(149); // Hit
      goto SUB_OP_EXTENDED_4;
    }

/* ------------------------------------------------------------------------- */
/*                              VM_OP2_ARRAY_NEW                             */
/*   reg1: Array capacity                                                    */
/* ------------------------------------------------------------------------- */

    MVM_CASE (VM_OP2_ARRAY_NEW): {
      CODE_COVERAGE(100); // Hit

      FLUSH_REGISTER_CACHE();
      reg1 /* arr */ = vm_newArray(vm, reg1 /* capacity */);
      CACHE_REGISTERS();

      PUSH(reg1 /* arr */);

      goto SUB_TAIL_POP_0_PUSH_0;
    }

/* ------------------------------------------------------------------------- */
/*                               VM_OP1_FIXED_ARRAY_NEW_2                    */
/*   Expects:                                                                */
/*     reg1: Fixed-array length (8-bit)                                      */
/* ------------------------------------------------------------------------- */

    MVM_CASE (VM_OP2_FIXED_ARRAY_NEW_2): {
      CODE_COVERAGE_UNTESTED(135); // Not hit
      goto SUB_FIXED_ARRAY_NEW;
    }

  } // End of vm_TeOpcodeEx2 switch

  // All cases should jump to whatever tail they intend. Nothing should get here
  VM_ASSERT_UNREACHABLE(vm);

} // End of SUB_OP_EXTENDED_2

/* ------------------------------------------------------------------------- */
/*                             SUB_FIXED_ARRAY_NEW                           */
/*   Expects:                                                                */
/*     reg1: length of fixed-array to create                                 */
/* ------------------------------------------------------------------------- */

SUB_FIXED_ARRAY_NEW: {
  FLUSH_REGISTER_CACHE();
  uint16_t* arr = mvm_allocate(vm, reg1 * 2, TC_REF_FIXED_LENGTH_ARRAY);
  CACHE_REGISTERS();
  uint16_t* p = arr;
  // Note: when reading a DELETED value from the array, it will read as
  // `undefined`. When fixed-length arrays are used to hold closure values, the
  // `DELETED` value can be used to represent the TDZ.
  while (reg1--)
    *p++ = VM_VALUE_DELETED;
  reg1 = ShortPtr_encode(vm, arr);
  goto SUB_TAIL_POP_0_PUSH_REG1;
}

/* ------------------------------------------------------------------------- */
/*                             SUB_OP_EXTENDED_3                             */
/*   Expects:                                                                */
/*     reg1: vm_TeOpcodeEx3                                                  */
/* ------------------------------------------------------------------------- */

SUB_OP_EXTENDED_3: {
  CODE_COVERAGE(150); // Hit
  reg3 = reg1;

  // Most Ex-3 instructions have a 16-bit parameter
  if (reg3 >= VM_OP3_DIVIDER_1) {
    CODE_COVERAGE(603); // Hit
    READ_PGM_2(reg1);
  } else {
    CODE_COVERAGE(606); // Hit
  }

  if (reg3 >= VM_OP3_DIVIDER_2) {
    CODE_COVERAGE(151); // Hit
    reg2 = POP();
  } else {
    CODE_COVERAGE(152); // Hit
  }

  VM_ASSERT(vm, reg3 < VM_OP3_END);
  MVM_SWITCH (reg3, (VM_OP3_END - 1)) {

/* ------------------------------------------------------------------------- */
/*                             VM_OP3_POP_N                                  */
/*   Expects:                                                                */
/*     Nothing                                                               */
/* ------------------------------------------------------------------------- */

    MVM_CASE (VM_OP3_POP_N): {
      CODE_COVERAGE(602); // Hit
      READ_PGM_1(reg1);
      while (reg1--)
        (void)POP();
      goto SUB_TAIL_POP_0_PUSH_0;
    }

/* -------------------------------------------------------------------------*/
/*                             VM_OP3_SCOPE_DISCARD                         */
/*   Expects:                                                               */
/*     Nothing                                                              */
/* -------------------------------------------------------------------------*/

    MVM_CASE (VM_OP3_SCOPE_DISCARD): {
      CODE_COVERAGE(634); // Hit
      reg->closure = VM_VALUE_UNDEFINED;
      goto SUB_TAIL_POP_0_PUSH_0;
    }

/* ------------------------------------------------------------------------- */
/*                             VM_OP3_SCOPE_CLONE                            */
/*                                                                           */
/*   Clones the top closure scope (which must exist) and sets it as the      */
/*   new scope                                                               */
/*                                                                           */
/*   Expects:                                                                */
/*     Nothing                                                               */
/* ------------------------------------------------------------------------- */

    MVM_CASE (VM_OP3_SCOPE_CLONE): {
      CODE_COVERAGE(635); // Hit

      VM_ASSERT(vm, reg->closure != VM_VALUE_UNDEFINED);
      FLUSH_REGISTER_CACHE();
      Value newScope = vm_cloneContainer(vm, &reg->closure);
      CACHE_REGISTERS();
      reg->closure = newScope;

      goto SUB_TAIL_POP_0_PUSH_0;
    }

/* ------------------------------------------------------------------------- */
/*                             VM_OP3_AWAIT                                  */
/*   Expects:                                                                */
/*     Nothing                                                               */
/* ------------------------------------------------------------------------- */

    MVM_CASE (VM_OP3_AWAIT): {
      /*
      This instruction is invoked at a syntactic `await` point, which is after
      the awaited expression has been pushed to the stack. If the awaited thing
      (e.g. promise) has been elided due to CPS-optimization, the awaited value
      will be VM_VALUE_UNDEFINED
      */
      goto SUB_AWAIT;
    }

/* ------------------------------------------------------------------------- */
/*                             VM_OP3_AWAIT_CALL                             */
/*   Expects:                                                                */
/*     Nothing                                                               */
/* ------------------------------------------------------------------------- */

    MVM_CASE (VM_OP3_AWAIT_CALL): {
      CODE_COVERAGE(667); // Hit
      // reg1 = arg count
      READ_PGM_1(reg1);
      // It doesn't make sense for the arg count word to contain the
      // AF_VOID_CALLED flag because the point of an await-call is that the
      // result is awaited, so it's not a void call.
      VM_ASSERT(vm, (reg1 & AF_ARG_COUNT_MASK) == reg1);

      // Note: the AWAIT instruction will set up the current closure function.
      // This is valid because the callback should only be called
      // asynchronously. And it's efficient because the AWAIT instruction needs
      // to do it anyway if it subscribes to the promise result of the callee.
      reg2 = VM_VALUE_DELETED; // Poison value in case the callee calls the callback synchronously.

      // The current closure can be a continuation closure by assigning its
      // function to the resume point.
      VM_ASSERT(vm, deepTypeOf(vm, reg->closure) == TC_REF_CLOSURE);
      regP1 /* current scope */ = ShortPtr_decode(vm, reg->closure);
      regP1[0] = reg2;

      // Similar to VM_OP2_CALL_3 except that cpsCallback points to the current closure
      reg3 /* cpsCallback */ = reg->closure;

      goto SUB_CALL_DYNAMIC;
    }

/* ------------------------------------------------------------------------- */
/*                             VM_OP3_ASYNC_RESUME                           */
/*   Expects:                                                                */
/*     Nothing                                                               */
/* ------------------------------------------------------------------------- */

    // This instruction is the first instruction executed after an await point
    // in an async function.
    MVM_CASE (VM_OP3_ASYNC_RESUME): {
      CODE_COVERAGE(668); // Hit

      READ_PGM_1(reg1 /* stack restoration slot count */);
      READ_PGM_1(reg2 /* top catch block */);

      // Safety mechanism: wipe the closure function so that if the continuation
      // is called illegally, it will be flagged. Note that there is already a
      // wrapper function around the continuation closure when the host calls it,
      // so this is just for catching internal bugs.
      #if MVM_SAFE_MODE
        regLP1 = vm_findScopedVariable(vm, 0);
        regP1 = (Value*)LongPtr_truncate(vm, regLP1);
        *regP1 = VM_VALUE_DELETED;
      #endif

      // The synchronous stack will be empty when the async function is resumed
      VM_ASSERT(vm, pFrameBase == pStackPointer);

      // Push the synchronous return value onto the stack. It doesn't really
      // matter what this value is because the VM should only be resumed from
      // the job queue (or the host can call a callback, but it should also not
      // be expecting anything in particular for the result). This is kept in
      // var[0] on the stack by common agreement with other operation. E.g.
      // `ASYNC_RETURN` returns the value that's in this slot.
      PUSH(VM_VALUE_UNDEFINED); // pFrameBase[0]

      // Set up a catch target at this location on the stack (var slots 1 and 2)
      VM_ASSERT(vm, pStackPointer == pFrameBase + 1);
      // There should be no parent catch target because async functions can only
      // be resumed from the job queue or a non-reentrant call from the host.
      VM_ASSERT(vm, reg->pCatchTarget == NULL);
      PUSH_CATCH_TARGET(getBuiltin(vm, BIN_ASYNC_CATCH_BLOCK));

      // Restore stack (user defined catch blocks and expression temporaries).
      // Slot 0 and 1 in the closure are for the continuation and callback. The
      // next slots after that are reserved for dumping and restoring the stack
      // state.
      regP1 /* closure */ = (Value*)DynamicPtr_decode_native(vm, reg->closure);
      VM_ASSERT(vm, vm_getAllocationSize(regP1) >= (2 + reg1) * 2);
      regP1 += 2; // Skip over continuation and callback

      TABLE_COVERAGE(reg1 ? 1 : 0, 2, 685); // Hit 2/2
      while (reg1--) {
        PUSH(*regP1);
        // Wipe the closure slot. My reasoning is that async functions may be
        // long-lived, and it's possible that the stack temporaries hold
        // references to large structures, so we don't want them to be
        // GC-reachable for the lifetime of the async function.
        *regP1 = VM_VALUE_DELETED;
        regP1++;
      }

      // Restore the catchTarget. It's statically determined how far behind the
      // stack pointer the catch target is. It will never be null because async
      // functions always have the root catch block.
      TABLE_COVERAGE(reg->pCatchTarget == &pStackPointer[-reg2] ? 1 : 0, 2, 690); // Hit 2/2
      reg->pCatchTarget = &pStackPointer[-reg2];
      VM_ASSERT(vm, reg->pCatchTarget >= &pFrameBase[1]);
      VM_ASSERT(vm, reg->pCatchTarget < pStackPointer);

      // Push asynchronous result to the stack. Note: it's illegal for an agent
      // to participate in CPS and not pass exactly three arguments `(this,
      // isSuccess, result)`.
      VM_ASSERT(vm, (reg->argCountAndFlags & AF_ARG_COUNT_MASK) == 3);

      // Note: the signature here is (this, isSuccess, value)
      reg2 /* isSuccess */ = reg->pArgs[1];
      reg1 /* result */ = reg->pArgs[2];

      if (reg2 /* isSuccess */ == VM_VALUE_FALSE) {
        CODE_COVERAGE(669); // Hit
        // Throw the value in reg1 (the error). The root catch block we pushed
        // earlier will catch it.
        goto SUB_THROW;
      } else {
        CODE_COVERAGE(686); // Hit
      }
      // Microvium CPS protocol requires that the first parameter is a boolean
      // to indicate success or failure
      VM_ASSERT(vm, reg2 == VM_VALUE_TRUE);
      CODE_COVERAGE(670); // Hit

      // Push the result to the stack and then continue with the function
      goto SUB_TAIL_POP_0_PUSH_REG1;
    }

/* ------------------------------------------------------------------------- */
/*                             VM_OP3_JUMP_2                                 */
/*   Expects:                                                                */
/*     reg1: signed offset                                                   */
/* ------------------------------------------------------------------------- */

    MVM_CASE (VM_OP3_JUMP_2): {
      CODE_COVERAGE(153); // Hit
      goto SUB_JUMP_COMMON;
    }

/* ------------------------------------------------------------------------- */
/*                             VM_OP3_LOAD_LITERAL                           */
/*   Expects:                                                                */
/*     reg1: literal value                                                   */
/* ------------------------------------------------------------------------- */

    MVM_CASE (VM_OP3_LOAD_LITERAL): {
      CODE_COVERAGE(154); // Hit
      goto SUB_TAIL_POP_0_PUSH_REG1;
    }

/* ------------------------------------------------------------------------- */
/*                             VM_OP3_LOAD_GLOBAL_3                          */
/*   Expects:                                                                */
/*     reg1: global variable index                                           */
/* ------------------------------------------------------------------------- */

    MVM_CASE (VM_OP3_LOAD_GLOBAL_3): {
      CODE_COVERAGE(155); // Hit
      reg1 = globals[reg1];
      goto SUB_TAIL_POP_0_PUSH_REG1;
    }

/* ------------------------------------------------------------------------- */
/*                             VM_OP3_LOAD_SCOPED_3                          */
/*   Expects:                                                                */
/*     reg1: scoped variable index                                           */
/* ------------------------------------------------------------------------- */

    MVM_CASE (VM_OP3_LOAD_SCOPED_3): {
      CODE_COVERAGE_UNTESTED(600); // Not hit
      goto SUB_OP_LOAD_SCOPED;
    }

/* ------------------------------------------------------------------------- */
/*                             VM_OP3_BRANCH_2                               */
/*   Expects:                                                                */
/*     reg1: signed offset                                                   */
/*     reg2: condition                                                       */
/* ------------------------------------------------------------------------- */

    MVM_CASE (VM_OP3_BRANCH_2): {
      CODE_COVERAGE(156); // Hit
      goto SUB_BRANCH_COMMON;
    }

/* ------------------------------------------------------------------------- */
/*                             VM_OP3_STORE_GLOBAL_3                         */
/*   Expects:                                                                */
/*     reg1: global variable index                                           */
/*     reg2: value to store                                                  */
/* ------------------------------------------------------------------------- */

    MVM_CASE (VM_OP3_STORE_GLOBAL_3): {
      CODE_COVERAGE(157); // Hit
      globals[reg1] = reg2;
      goto SUB_TAIL_POP_0_PUSH_0;
    }

/* ------------------------------------------------------------------------- */
/*                             VM_OP3_STORE_SCOPED_3                         */
/*   Expects:                                                                */
/*     reg1: scoped variable index                                           */
/*     reg2: value to store                                                  */
/* ------------------------------------------------------------------------- */

    MVM_CASE (VM_OP3_STORE_SCOPED_3): {
      CODE_COVERAGE_UNTESTED(601); // Not hit
      goto SUB_OP_STORE_SCOPED;
    }

/* ------------------------------------------------------------------------- */
/*                             VM_OP3_OBJECT_GET_2                           */
/*   Expects:                                                                */
/*     reg1: property key value                                              */
/*     reg2: object value                                                    */
/* ------------------------------------------------------------------------- */

    MVM_CASE (VM_OP3_OBJECT_GET_2): {
      CODE_COVERAGE_UNTESTED(158); // Not hit
      VM_NOT_IMPLEMENTED(vm);
      err = MVM_E_FATAL_ERROR_MUST_KILL_VM;
      goto SUB_EXIT;
    }

/* ------------------------------------------------------------------------- */
/*                             VM_OP3_OBJECT_SET_2                           */
/*   Expects:                                                                */
/*     reg1: property key value                                              */
/*     reg2: value                                                           */
/* ------------------------------------------------------------------------- */

    MVM_CASE (VM_OP3_OBJECT_SET_2): {
      CODE_COVERAGE_UNTESTED(159); // Not hit
      VM_NOT_IMPLEMENTED(vm);
      err = MVM_E_FATAL_ERROR_MUST_KILL_VM;
      goto SUB_EXIT;
    }

  } // End of vm_TeOpcodeEx3 switch
  // All cases should jump to whatever tail they intend. Nothing should get here
  VM_ASSERT_UNREACHABLE(vm);
} // End of SUB_OP_EXTENDED_3


/* ------------------------------------------------------------------------- */
/*                             VM_OP3_OBJECT_SET_2                           */
/*   Expects:                                                                */
/*     reg1: The Ex-4 instruction opcode                                     */
/* ------------------------------------------------------------------------- */
SUB_OP_EXTENDED_4: {
  MVM_SWITCH(reg1, (VM_NUM_OP4_END - 1)) {

/* ------------------------------------------------------------------------- */
/*                             VM_OP4_START_TRY                              */
/*   Expects: nothing                                                        */
/* ------------------------------------------------------------------------- */

    MVM_CASE(VM_OP4_START_TRY): {
      CODE_COVERAGE(206); // Hit

      // Location to jump to if there's an exception
      READ_PGM_2(reg2);
      PUSH_CATCH_TARGET(reg2);

      goto SUB_TAIL_POP_0_PUSH_0;
    } // End of VM_OP4_START_TRY

    MVM_CASE(VM_OP4_END_TRY): {
      CODE_COVERAGE(207); // Hit

      // Note: EndTry can be invoked either at the normal ending of a `try`
      // block, or during a `return` out of a try block. In the former case, the
      // stack will already be at the level it was after the StartTry, but in
      // the latter case the stack level could be anything since `return` won't
      // go to the effort of popping intermediate variables off the stack.

      VM_ASSERT(vm, reg->pCatchTarget != NULL); // Must be in a try block (StartTry must have been called)
      pStackPointer = reg->pCatchTarget;
      UNWIND_CATCH_TARGET();
      VM_ASSERT(vm, pStackPointer >= pFrameBase); // EndTry can only end a try within the current frame

      goto SUB_TAIL_POP_0_PUSH_0;
    } // End of VM_OP4_END_TRY

    MVM_CASE(VM_OP4_OBJECT_KEYS): {
      CODE_COVERAGE(223); // Hit

      // Note: leave object on the stack in case a GC cycle is triggered by the array allocation
      FLUSH_REGISTER_CACHE();
      err = vm_objectKeys(vm, &reg->pStackPointer[-1]);
      // TODO: We could maybe eliminate the common CACHE_REGISTERS operation if
      // the exit path checked the flag and cached for us.
      CACHE_REGISTERS();

      goto SUB_TAIL_POP_0_PUSH_0; // Pop the object and push the keys
    } // End of VM_OP4_OBJECT_KEYS

    MVM_CASE(VM_OP4_UINT8_ARRAY_NEW): {
      CODE_COVERAGE(324); // Hit

      FLUSH_REGISTER_CACHE();
      err = vm_uint8ArrayNew(vm, &reg->pStackPointer[-1]);
      CACHE_REGISTERS();

      goto SUB_TAIL_POP_0_PUSH_0;
    } // End of VM_OP4_OBJECT_KEYS

/* ------------------------------------------------------------------------- */
/*                          VM_OP4_CLASS_CREATE                              */
/*   Expects:                                                                */
/*     Nothing                                                               */
/* ------------------------------------------------------------------------- */

    MVM_CASE (VM_OP4_CLASS_CREATE): {
      CODE_COVERAGE(614); // Hit
      // TODO: I think we could save some flash space if we grouped all the
      // opcodes together according to whether they flush the register cache.
      // Also maybe they could be dispatched through a lookup table.
      FLUSH_REGISTER_CACHE();
      TsClass* pClass = mvm_allocate(vm, sizeof (TsClass), TC_REF_CLASS);
      CACHE_REGISTERS();
      pClass->constructorFunc = pStackPointer[-2];
      pClass->staticProps = pStackPointer[-1];
      pStackPointer[-2] = ShortPtr_encode(vm, pClass);
      goto SUB_TAIL_POP_1_PUSH_0;
    }

/* ------------------------------------------------------------------------- */
/*                          VM_OP4_TYPE_CODE_OF                              */
/*   Expects:                                                                */
/*     Nothing                                                               */
/* ------------------------------------------------------------------------- */

    MVM_CASE (VM_OP4_TYPE_CODE_OF): {
      CODE_COVERAGE(631); // Hit
      reg1 = mvm_typeOf(vm, pStackPointer[-1]);
      reg1 = VirtualInt14_encode(vm, reg1);
      goto SUB_TAIL_POP_1_PUSH_REG1;
    }

/* ------------------------------------------------------------------------- */
/*                          VM_OP4_LOAD_REG_CLOSURE                          */
/*   Expects:                                                                */
/*     Nothing                                                               */
/* ------------------------------------------------------------------------- */

    MVM_CASE (VM_OP4_LOAD_REG_CLOSURE): {
      CODE_COVERAGE(644); // Hit
      reg1 = reg->closure;
      goto SUB_TAIL_POP_0_PUSH_REG1;
    }

/* ------------------------------------------------------------------------- */
/*                          VM_OP4_SCOPE_PUSH                                */
/*   Expects:                                                                */
/*     Nothing                                                               */
/* ------------------------------------------------------------------------- */

    MVM_CASE (VM_OP4_SCOPE_PUSH): {
      CODE_COVERAGE(648); // Hit
      reg3 /*capture parent*/ = true;
      goto SUB_OP_SCOPE_PUSH_OR_NEW;
    }

/* ------------------------------------------------------------------------- */
/*                             VM_OP4_SCOPE_POP                              */
/*   Expects:                                                                */
/*     Nothing                                                               */
/* ------------------------------------------------------------------------- */
    MVM_CASE (VM_OP4_SCOPE_POP): {
      CODE_COVERAGE(649); // Hit
      reg1 = reg->closure;
      VM_ASSERT(vm, reg1 != VM_VALUE_UNDEFINED);
      LongPtr lpClosure = DynamicPtr_decode_long(vm, reg1);
      uint16_t headerWord = readAllocationHeaderWord_long(lpClosure);
      uint16_t size = vm_getAllocationSizeExcludingHeaderFromHeaderWord(headerWord);
      // The pointer to the parent scope is the last slot in the closure
      reg1 = LongPtr_read2_aligned(LongPtr_add(lpClosure, size - 2));
      reg->closure = reg1;

      VM_ASSERT(vm, vm_getTypeCodeFromHeaderWord(headerWord) == TC_REF_CLOSURE);
      VM_ASSERT(vm, size >= 2);
      VM_ASSERT(vm, (deepTypeOf(vm, reg1) == TC_REF_CLOSURE) || (deepTypeOf(vm, reg1) == TC_VAL_DELETED));

      goto SUB_TAIL_POP_0_PUSH_0;
    }

/** -------------------------------------------------------------------------
 *                             VM_OP4_SCOPE_SAVE
 *
 * Saves the current closure by pushing it to the stack.
 *
 *   Expects:
 *     Nothing
 * ------------------------------------------------------------------------- */
    MVM_CASE (VM_OP4_SCOPE_SAVE): {
      CODE_COVERAGE(728); // Hit
      PUSH(reg->closure);
      goto SUB_TAIL_POP_0_PUSH_0;
    }

/* ------------------------------------------------------------------------- */
/*                            VM_OP4_ASYNC_START                             */
/*   Expects:                                                                */
/*     Nothing                                                               */
/*                                                                           */
/*   This should be the first instruction in an async function.              */
/* ------------------------------------------------------------------------- */
    MVM_CASE (VM_OP4_ASYNC_START): {
      CODE_COVERAGE(696); // Hit
      READ_PGM_1(reg1); // Closure size and parent reference flag

      // Reserve a slot for the result. Note that `ASYNC_START` is the first
      // instruction in an async function, so the result is stored at `var[0]`
      VM_ASSERT(vm, pFrameBase == pStackPointer);
      PUSH(VM_VALUE_UNDEFINED);

      FLUSH_REGISTER_CACHE();

      TABLE_COVERAGE((reg1 & 0x80) ? 1 : 0, 2, 683); // Hit 2/2
      TABLE_COVERAGE((reg1 & 0x7F) > 2 ? 1 : 0, 2, 684); // Hit 2/2

      // Acquire the callback that this async function needs to call when it's
      // done. If caller used CPS, the callback is the one provided by the
      // caller, otherwise this will synthesize a Promise and return a callback
      // that resolves or rejects the promise.
      reg2 = vm_asyncStartUnsafe(vm,
        reg->pFrameBase /* synchronous result slot */
      );
      vm_push(vm, reg2); // GC-reachable, because vm_scopePushOrNew performs an allocation

      // Create closure scope for async function
      regP1 /* scope */ = vm_scopePushOrNew(vm,
        reg1 & 0x7F, // slotCount
        reg1 & 0x80 // isParentCapturing
      );
      // The callback gets stored in
      regP1[1] /* callback */ = vm_pop(vm);

      CACHE_REGISTERS();

      // Async catch target (logic basically copied from VM_OP4_START_TRY)
      VM_ASSERT(vm, pStackPointer == pFrameBase + 1);
      PUSH_CATCH_TARGET(getBuiltin(vm, BIN_ASYNC_CATCH_BLOCK));

      goto SUB_TAIL_POP_0_PUSH_0;
    }

/* ------------------------------------------------------------------------- */
/*                            VM_OP4_ASYNC_RETURN                            */
/*   Expects:                                                                */
/*     Nothing                                                               */
/*                                                                           */
/* ------------------------------------------------------------------------- */
    MVM_CASE (VM_OP4_ASYNC_RETURN): {
      // This operation is used in place of a normal RETURN when compiling an
      // async function. It indirectly calls the callback function with the
      // result instead of passing it to the synchronous caller (it does so via
      // the job queue).

      CODE_COVERAGE(732); // Not hit

      reg2 /* result */ = POP();

      // Pop the async catch block. We know that this is always stored in the
      // same slot. It doesn't matter what's on top of it.
      pStackPointer = &pFrameBase[1];
      UNWIND_CATCH_TARGET();

      PUSH(/* result */ reg2);
      PUSH(/* isSuccess */ VM_VALUE_TRUE);
      goto SUB_ASYNC_COMPLETE;
    }

/* ------------------------------------------------------------------------- */
/*                            VM_OP4_ENQUEUE_JOB                             */
/*   Expects:                                                                */
/*     Nothing                                                               */
/*                                                                           */
/* ------------------------------------------------------------------------- */
    MVM_CASE (VM_OP4_ENQUEUE_JOB): {
      // This instruction enqueues the current closure to the job queue (for the
      // moment there is only one job queue, for executing async callbacks)
      CODE_COVERAGE_UNTESTED(671); // Not hit
      // Need to flush registers because `vm_enqueueJob` can trigger GC collection
      FLUSH_REGISTER_CACHE();
      vm_enqueueJob(vm, reg->closure);
      CACHE_REGISTERS();
      goto SUB_TAIL_POP_0_PUSH_0;
    }

/* ------------------------------------------------------------------------- */
/*                            VM_OP4_ASYNC_COMPLETE                             */
/*   Expects:                                                                */
/*     Nothing                                                               */
/*                                                                           */
/* ------------------------------------------------------------------------- */
    MVM_CASE (VM_OP4_ASYNC_COMPLETE): {
      // This instruction implements the completion of an async function. The
      // main functionality is in SUB_ASYNC_COMPLETE which is shared between the
      // 3 different async completion paths: return (AsyncReturn), catch
      // (builtin BIN_ASYNC_CATCH_BLOCK), and host-callback (builtin
      // BIN_ASYNC_HOST_CALLBACK).

      CODE_COVERAGE(697); // Hit

      goto SUB_ASYNC_COMPLETE;
    }

  } // End of switch inside SUB_OP_EXTENDED_4
} // End of SUB_OP_EXTENDED_4

/* -------------------------------------------------------------------------
 *                             SUB_AWAIT
 *
 * Persists the current stack values to the closure, unwinds the stack, and
 * returns to the caller. If awaited value is a promise, it also subscribes to
 * the promise.
 *
 *   Expects:
 *     - value to await is at the top of the stack
 *     - in an async function with corresponding stack and closure structure
 *
 * ------------------------------------------------------------------------- */
SUB_AWAIT: {
  CODE_COVERAGE(666); // Hit

  reg1 /* value to await */ = POP();

  // We need to preserve the stack by copying it to the closure. regP1 is
  // the cursor on the stack that we're copying out of and regP2 is the
  // cursor on the heap (closure) that we're copying into.

  // We only need to preserve slots from `&pFrameBase[3]` onwards because
  // the first 3 slots are the synchronous return value and the top-level
  // catch block.
  regP1 = &pFrameBase[3];
  VM_ASSERT(vm, pStackPointer >= regP1);
  // var[0] is the synchronous return value and var[1-2] are the top-level
  // catch block. Don't need to copy these.
  reg2 /*closure*/ = reg->closure;
  VM_ASSERT(vm, reg2 != VM_VALUE_DELETED);
  // Note: the closure must be in RAM because we're modifying it
  regP2 = DynamicPtr_decode_native(vm, reg2 /*closure*/);
  VM_ASSERT(vm, vm_getAllocationType(regP2) == TC_REF_CLOSURE);
  // The closure must be large enough to store all the stack variables. The
  // +4 here is 4 bytes for the first 2 slots of the closure which hold the
  // continuation function pointer and the callback function pointer.
  VM_ASSERT(vm, vm_getAllocationSize(regP2) >= ((intptr_t)pStackPointer - (intptr_t)regP1) + 4);

  /*
  Await/resume bytecode structure

    - [1B]: VM_OP3_AWAIT instruction (synchronous return point)
    - [0-3B]: padding to 4-byte boundary
    - [2B]: function header
    - [2B]: VM_OP3_ASYNC_RESUME + 8-bit slot count + 8-bit catchTarget info
  */

  // Round up to nearest 4-byte boundary to find the start of the
  // continuation (since this needs to be addressable and bytecode is only
  // addressable at 4-byte alignment). This is a bit cumbersome because I'm
  // not assuming that LongPtr can be directly cast to an integer type.
  reg2 /* pc offset in bytecode */ = LongPtr_sub(lpProgramCounter, vm->lpBytecode);
  reg2 /* resume point offset in bytecode */ = (reg2 + (
    + 2 // skip over function header
    + 3 // round up to 4-byte boundary
    )) & 0xFFFC;

  // The resume point should be immediately preceeded by a function header
  VM_ASSERT(vm,
    vm_getTypeCodeFromHeaderWord(
      LongPtr_read2_aligned(LongPtr_add(vm->lpBytecode, reg2 - 2))
    ) == TC_REF_FUNCTION);

  // The first instruction at the resume point is expected to be the async-resume instruction
  VM_ASSERT(vm, LongPtr_read1(LongPtr_add(vm->lpBytecode, reg2)) == ((VM_OP_EXTENDED_3 << 4) | VM_OP3_ASYNC_RESUME));

  regP2[0] /* resume point bytecode pointer */ = vm_encodeBytecodeOffsetAsPointer(vm, reg2);


  // Preserve the stack
  regP2 = &regP2[2]; // Skip continuation pointer and callback slot
  TABLE_COVERAGE(regP1 < pStackPointer ? 1 : 0, 2, 687); // Hit 2/2
  while (regP1 < pStackPointer) {
    *regP2++ = *regP1++;
  }

  // Unwind the exception stack
  pStackPointer = &pFrameBase[1]; // The catch block is always stored in slots 1-2
  VM_ASSERT(vm, pStackPointer[1] == getBuiltin(vm, BIN_ASYNC_CATCH_BLOCK));
  UNWIND_CATCH_TARGET();

  // Optimization: if the AWAIT instruction is awaiting the result of a
  // function call, then the call was compiled as an AWAIT_CALL instruction
  // to pass a continuation callback to the callee. If the callee supports
  // CPS then it will "accept" the continuation by returning
  // VM_VALUE_DELETED as the result, to indicate an elided promise.
  if (reg1 /* value to await */ == VM_VALUE_DELETED) {
    CODE_COVERAGE(688); // Hit
    // Return the synchronous return value which is specified as being in
    // var[0] for all async functions. The synchronous return value could be
    // VM_VALUE_UNDEFINED if we're currently in a state where we're resumed
    // from the job queue, or if the call is a void-call, or it could be
    // VM_VALUE_DELETED if the caller used CPS, or it could be a promise if
    // the caller was not void-calling and not await-calling. The value is
    // established in the `ASYNC_START` operation or `ASYNC_RESUME`
    // operation.
    reg1 = pFrameBase[0];
    goto SUB_RETURN;
  }

  CODE_COVERAGE(689); // Hit

  // If the value to await is not elided by a CPS-optimized call, then it
  // must be a promise. If it's not a promise, then we have a type error.
  // This doesn't match the ECMAScript standard because in normal JS you can
  // await anything, but I think it's a reasonable behavior and a subset of
  // the full spec.

  TeTypeCode tc = deepTypeOf(vm, reg1 /* value to await */);
  if (tc != TC_REF_PROPERTY_LIST) {
    CODE_COVERAGE_ERROR_PATH(705); // Not hit
    // TODO: type errors should actually be catchable
    err = vm_newError(vm, MVM_E_TYPE_ERROR_AWAIT_NON_PROMISE);
    goto SUB_EXIT;
  }
  regP1 = ShortPtr_decode(vm, reg1 /* value to await */);
  // Brand check
  if (regP1[VM_OIS_PROTO] != getBuiltin(vm, BIN_PROMISE_PROTOTYPE)) {
    CODE_COVERAGE_ERROR_PATH(706); // Not hit
    err = vm_newError(vm, MVM_E_TYPE_ERROR_AWAIT_NON_PROMISE);
    goto SUB_EXIT;
  }


  FLUSH_REGISTER_CACHE();
  mvm_subscribeToPromise(vm, reg1 /* promise */, reg->closure);
  CACHE_REGISTERS();

  // The stack has been unwound, so it should just be the synchronous return
  // value remaining
  VM_ASSERT(vm, pStackPointer == pFrameBase + 1);
  reg1 = POP(); // Return value
  goto SUB_RETURN;
}

/* -------------------------------------------------------------------------
 *                             SUB_ASYNC_COMPLETE
 *
 * This subroutine implements the completion of an async function, which
 * schedules any callbacks to be called on the job queue and then returns from
 * the current function. This subroutine is shared between the 3 different async
 * completion paths: return (AsyncReturn), catch (builtin
 * BIN_ASYNC_CATCH_BLOCK), and host-callback (builtin BIN_ASYNC_HOST_CALLBACK).
 *
 *   Expects:
 *
 *     - result/error and isSuccess on the stack
 *     - callbackOrPromise in scope[1] of the current closure
 *     - the synchronous return value in var[0]
 *
 * ------------------------------------------------------------------------- */
SUB_ASYNC_COMPLETE: {
  CODE_COVERAGE(698); // Hit

  // I think all paths leading here will get the stack into a consistent state
  VM_ASSERT(vm, pStackPointer == pFrameBase + 3);

  reg2 = POP(); // isSuccess
  reg2 = mvm_toBool(vm, reg2) ? VM_VALUE_TRUE : VM_VALUE_FALSE; // Coerce to boolean
  reg3 = POP(); // result/error
  reg1 = vm_readScopedFromThisClosure(vm, 1); // callbackOrPromise

  FLUSH_REGISTER_CACHE();
  TeTypeCode tc = deepTypeOf(vm, reg1);
  if (tc == TC_VAL_NO_OP_FUNC) {
    // If the callback is a no-op, then we don't need to schedule it on the job
    CODE_COVERAGE(664); // Hit
  } else if (tc == TC_REF_CLOSURE) {
    // The callback is a direct continuation
    CODE_COVERAGE(665); // Hit
    vm_scheduleContinuation(vm, reg1, reg2, reg3);
  } else {
    // Otherwise, the callback slot holds a promise. This happens if the current
    // async operation was not called in an await-call or void-call, so a
    // promise was synthesized.
    CODE_COVERAGE(699); // Hit
    VM_ASSERT(vm, tc == TC_REF_PROPERTY_LIST);

    // Assuming promises are in RAM because we can subscribe to them any time
    // and there is currently no static analysis that can prove otherwise.
    Value* pPromise = ShortPtr_decode(vm, reg1);
    // The promise is generated internally, and the slot is not accessible by
    // user code, so it will always be a promise if it's not a function
    // callback.
    VM_ASSERT(vm, pPromise[VM_OIS_PROTO] == getBuiltin(vm, BIN_PROMISE_PROTOTYPE));
    VM_ASSERT(vm, vm_getAllocationSize(pPromise) >= 8); // At least 4 slots

    // The promise is guaranteed to be a in pending state because AsyncComplete
    // is the only way to transition out of the pending state, and this will
    // only be invoked once. This closure self-destructs by setting closure slot
    // 0 to a no-op function, so that successive calls will have no effect.
    VM_ASSERT(vm, pPromise[VM_OIS_PROMISE_STATUS] == VM_PROMISE_STATUS_PENDING);

    Value callbackList = pPromise[VM_OIS_PROMISE_OUT];

    // Mark the promise as settled
    pPromise[VM_OIS_PROMISE_STATUS] = reg2 == VM_VALUE_TRUE ? VM_PROMISE_STATUS_RESOLVED : VM_PROMISE_STATUS_REJECTED;
    pPromise[VM_OIS_PROMISE_OUT] = reg3; // Note: need to assign this before vm_scheduleContinuation to avoid GC issues

    tc = deepTypeOf(vm, callbackList);
    if (tc == TC_VAL_UNDEFINED) {
      // Subscriber list is empty
      CODE_COVERAGE(719); // Hit
    } else if (tc == TC_REF_CLOSURE) {
      // Single subscriber
      CODE_COVERAGE(720); // Hit
      vm_scheduleContinuation(vm, callbackList, reg2, reg3);
    } else {
      // Multiple subscribers
      CODE_COVERAGE(721); // Hit
      VM_ASSERT(vm, tc == TC_REF_ARRAY);
      TsArray* pArray = (TsArray*)ShortPtr_decode(vm, callbackList);
      int len = VirtualInt14_decode(vm, pArray->viLength);
      vm_push(vm, reg3 /* resultOrError */); // GC-reachable
      vm_push(vm, pArray->dpData); // GC-reachable
      TABLE_COVERAGE(len > 2 ? 1 : 0, 2, 722); // Hit 2/2
      for (int i = 0; i < len; i++) {
        // Note: the subscribers list is may move due to GC collections
        // caused by scheduling the continuation.
        Value* subscribers = ShortPtr_decode(vm, reg->pStackPointer[-1]);
        Value callback = subscribers[i];
        vm_scheduleContinuation(vm, callback, reg2, reg->pStackPointer[-2]);
      }
      vm_pop(vm); // dpData
      vm_pop(vm); // resultOrError
    }
  }
  CACHE_REGISTERS();

  // Invalidate the current closure so if it's called again it won't do anything
  vm_writeScopedToThisClosure(vm, 0, VM_VALUE_NO_OP_FUNC);

  VM_ASSERT(vm, pStackPointer == pFrameBase + 1); // I think at this point the stack should be empty except for the return value
  reg1 = pFrameBase[0]; // Synchronous return value (e.g. the Promise)
  goto SUB_RETURN;
}

/* ------------------------------------------------------------------------- */
/*                             SUB_BRANCH_COMMON                             */
/*   Expects:                                                                */
/*     reg1: signed 16-bit amount to jump by if the condition is truthy      */
/*     reg2: condition to branch on                                          */
/* ------------------------------------------------------------------------- */
SUB_BRANCH_COMMON: {
  CODE_COVERAGE(160); // Hit
  if (mvm_toBool(vm, reg2)) {
    lpProgramCounter = LongPtr_add(lpProgramCounter, (int16_t)reg1);
  }
  goto SUB_TAIL_POP_0_PUSH_0;
}

/* ------------------------------------------------------------------------- */
/*                             SUB_JUMP_COMMON                               */
/*   Expects:                                                                */
/*     reg1: signed 16-bit amount to jump by                                 */
/* ------------------------------------------------------------------------- */
SUB_JUMP_COMMON: {
  CODE_COVERAGE(161); // Hit
  lpProgramCounter = LongPtr_add(lpProgramCounter, (int16_t)reg1);
  goto SUB_TAIL_POP_0_PUSH_0;
}

/* ------------------------------------------------------------------------- */
/*                                                                           */
/*                                  SUB_RETURN                               */
/*                                                                           */
/*   Return from the current frame                                           */
/*                                                                           */
/*   Expects:                                                                */
/*     reg1: the return value                                                */
/* ------------------------------------------------------------------------- */
SUB_RETURN: {
  CODE_COVERAGE(105); // Hit

  // Pop variables
  pStackPointer = pFrameBase;

  // Save argCountAndFlags from this frame
  reg3 = reg->argCountAndFlags;

  // Restore caller state
  POP_REGISTERS();

  // If the catch target isn't earlier than the stack pointer then possibly the
  // catch blocks weren't unwound properly (e.g. the compiler didn't generate
  // matching EndTry instructions).
  VM_ASSERT(vm, reg->pCatchTarget < pStackPointer);

  goto SUB_POP_ARGS;
}

/* ------------------------------------------------------------------------- */
/*                                                                           */
/*                                SUB_POP_ARGS                               */
/*                                                                           */
/*   The second part of a "RETURN". Assumes that we're already in the        */
/*   caller stack frame by this point.                                       */
/*                                                                           */
/*   Expects:                                                                */
/*     reg1: returning result                                                */
/*     reg3: argCountAndFlags for callee frame                               */
/* ------------------------------------------------------------------------- */
SUB_POP_ARGS: {
  // Pop arguments
  pStackPointer -= (reg3 & AF_ARG_COUNT_MASK);

  // Pop function reference
  if (reg3 & AF_PUSHED_FUNCTION) {
    CODE_COVERAGE(108); // Hit
    (void)POP();
  } else {
    CODE_COVERAGE_UNTESTED(109); // Hit
  }

  // We don't preserve this register across function calls, so when we return
  // from a function, we no longer know what the callback is for the caller
  // frame. VM_VALUE_DELETED is used as a poison value here.
  reg->cpsCallback = VM_VALUE_DELETED;

  // Called from the host?
  if (reg3 & AF_CALLED_FROM_HOST) {
    CODE_COVERAGE(221); // Hit
    goto SUB_RETURN_TO_HOST;
  } else if (reg3 & AF_VOID_CALLED) {
    CODE_COVERAGE(733); // Not hit
    // The call operation was a void call, so don't push the return value
    goto SUB_TAIL_POP_0_PUSH_0;
  } else {
    CODE_COVERAGE(111); // Hit
    // The call operation was a non-void-call, so push the return value
    goto SUB_TAIL_POP_0_PUSH_REG1;
  }
}

/* ------------------------------------------------------------------------- */
/*                                                                           */
/*                            SUB_RETURN_TO_HOST                             */
/*                                                                           */
/*   Return control to the host                                              */
/*                                                                           */
/*   This is after popping the arguments                                     */
/*                                                                           */
/*   Expects:                                                                */
/*     reg1: the return value                                                */
/* ------------------------------------------------------------------------- */
SUB_RETURN_TO_HOST: {
  CODE_COVERAGE(110); // Hit

  // Provide the return value to the host
  if (out_result) {
    *out_result = reg1;
  }

  // Next job in job queue
  if ((reg->jobQueue != VM_VALUE_UNDEFINED) && (pStackPointer == getBottomOfStack(vm->stack))) {
    CODE_COVERAGE(680); // Hit

    // Whatever the result has been set to for the primary call target, we don't
    // want to change to the result of any job
    out_result = NULL;

    FLUSH_REGISTER_CACHE();
    reg1 /* argCountAndFlags */ = 0 | AF_CALLED_FROM_HOST; // No args, and return to host when complete
    reg2 /* target */ = vm_dequeueJob(vm);
    VM_ASSERT(vm, deepTypeOf(vm, reg2) == TC_REF_CLOSURE); // I expect it to be a closure, although not technically required here
    reg3 /* cpsCallback */ = VM_VALUE_UNDEFINED;
    CACHE_REGISTERS();

    goto SUB_CALL;
  } else {
    CODE_COVERAGE(681); // Hit
  }

  goto SUB_EXIT;
}

/* ------------------------------------------------------------------------- */
/*                                                                           */
/*                                    SUB_CALL                               */
/*                                                                           */
/*   Performs a dynamic call to a given function value                       */
/*                                                                           */
/*   Expects:                                                                */
/*     reg1: argCountAndFlags excluding AF_PUSHED_FUNCTION                   */
/*     reg3: new value for CPS callback                                      */
/* ------------------------------------------------------------------------- */
SUB_CALL_DYNAMIC: {
  reg1 /* argCountAndFlags */ |= AF_PUSHED_FUNCTION;
  reg2 /* target */ = pStackPointer[-(int16_t)(reg1 & AF_ARG_COUNT_MASK) - 1]; // The function was pushed before the arguments
  goto SUB_CALL;
}

/* ------------------------------------------------------------------------- */
/*                                                                           */
/*                                    SUB_NEW                                */
/*                                                                           */
/*   Performs a dynamic call to a given function value                       */
/*                                                                           */
/*   Expects:                                                                */
/*     reg1: argCountAndFlags including AF_PUSHED_FUNCTION                   */
/*     The stack should have the class, this (undefined), and args           */
/* ------------------------------------------------------------------------- */
SUB_NEW: {
  regP1 = &pStackPointer[-(uint8_t)reg1 - 1]; // Pointer to class
  reg2 /*class*/ = regP1[0];

  // The class instance must be on the stack in the position where the function would normally go
  VM_ASSERT(vm, reg1 /*argCountAndFlags*/ & AF_PUSHED_FUNCTION);

  // Can only `new` classes in Microvium
  if (deepTypeOf(vm, reg2) != TC_REF_CLASS) {
    err = vm_newError(vm, MVM_E_USING_NEW_ON_NON_CLASS);
    goto SUB_EXIT;
  }

  // We've already checked that the target of the `new` operation is a
  // class. A class cannot existed without a `prototype` property. If the
  // class was created at compile time, the "prototype" string will be
  // embedded in the bytecode because the class definition uses it. If the
  // class was created at runtime, the "prototype" string will *also* be
  // embedded in the bytecode because classes at runtime are only created by
  // sequences of instructions that also includes reference to the
  // "prototype" string. So either way, the fact that we're at this point in
  // the code means that the "prototype" string must exist as a builtin.
  VM_ASSERT(vm, getBuiltin(vm, BIN_STR_PROTOTYPE) != VM_VALUE_UNDEFINED);

  regLP1 = DynamicPtr_decode_long(vm, reg2);
  regP1[0] /*func*/ = READ_FIELD_2(regLP1, TsClass, constructorFunc);
  // Note: this trashes the `this` slot, but it's ok because we set it later to the new object
  regP1[1] /*props*/ = READ_FIELD_2(regLP1, TsClass, staticProps);

  PUSH(getBuiltin(vm, BIN_STR_PROTOTYPE)); // "prototype" string

  // Get the prototype property of the class. Now regP1[1] is the prototype.
  FLUSH_REGISTER_CACHE();
  getProperty(vm, &regP1[1], &reg->pStackPointer[-1], &regP1[1]);

  vm_pop(vm); // "prototype" string

  TeTypeCode tc = deepTypeOf(vm, regP1[1] /* prototype */);
  reg2 /* internalSlotCount */ = 0;
  if (tc == TC_REF_PROPERTY_LIST) {
    CODE_COVERAGE(723); // Hit
    regP2 /* pPrototype */ = (Value*)ShortPtr_decode(vm, regP1[1]);
    // Look for the magic value that tells us how many prototype slots there
    // are.
    if ((vm_getAllocationSize(regP2) >= 4) &&
      (regP2[VM_OIS_PROTO_SLOT_MAGIC_KEY] == VM_PROTO_SLOT_MAGIC_KEY_VALUE)
    ) {
      reg2 /* internalSlotCount */ = VirtualInt14_decode(vm, regP2[VM_OIS_PROTO_SLOT_COUNT]);
    }
  } else if (tc == TC_VAL_NULL) {
    CODE_COVERAGE_UNTESTED(724); // Not hit
  } else {
    CODE_COVERAGE_ERROR_PATH(725); // Not hit
    err = vm_newError(vm, MVM_E_CLASS_PROTOTYPE_MUST_BE_NULL_OR_OBJECT);
    goto SUB_EXIT;
  }

  Value* pObject = mvm_allocate(vm, sizeof(TsPropertyList) + reg2 * sizeof(Value), TC_REF_PROPERTY_LIST);
  Value* p = pObject;
  *p++ = VM_VALUE_NULL; // dpNext
  *p++ = regP1[1]; // dpProto

  // Internal slots
  if (reg2) {
    CODE_COVERAGE(726); // Hit
    regP2 /* pPrototype */ = ShortPtr_decode(vm, regP1[1] /* dpProto */);
    // Make sure the prototype actually has the slots we want to read
    VM_ASSERT(vm, vm_getAllocationSize(regP2) >= 4 + reg2);
    regP2 = &regP2[4]; // Skip header and the magic number and slot count
    while (reg2--) {
      *p++ = *regP2++;
    }
  } else {
    CODE_COVERAGE(727); // Hit
  }

  regP1[1] /* this */ = ShortPtr_encode(vm, pObject);

  CACHE_REGISTERS();

  if (err != MVM_E_SUCCESS) goto SUB_EXIT;

  // The slot that was used for the class is now used for the function reference
  reg1 /*argCountAndFlags*/ |= AF_PUSHED_FUNCTION;
  reg2 = regP1[0];
  reg3 /* cpsCallback */ = VM_VALUE_UNDEFINED;

  goto SUB_CALL;
}
/* ------------------------------------------------------------------------- */
/*                                                                           */
/*                                    SUB_CALL                               */
/*                                                                           */
/*   Performs a dynamic call to a given function value                       */
/*                                                                           */
/*   Expects:                                                                */
/*     reg1: argCountAndFlags for the new frame                              */
/*     reg2: target function value to call                                   */
/*     reg3: new value for CPS callback                                      */
/* ------------------------------------------------------------------------- */
SUB_CALL: {
  CODE_COVERAGE(224); // Hit

  reg->cpsCallback = reg3;

  reg3 /* scope */ = VM_VALUE_UNDEFINED;

  while (true) {
    TeTypeCode tc = deepTypeOf(vm, reg2 /* target */);
    if (tc == TC_REF_FUNCTION) {
      CODE_COVERAGE(141); // Hit
      // The following trick of assuming the function offset is just
      // `target >>= 1` is only true if the function is in ROM.
      VM_ASSERT(vm, DynamicPtr_isRomPtr(vm, reg2 /* target */));
      reg2 &= 0xFFFE;
      goto SUB_CALL_BYTECODE_FUNC;
    } else if (tc == TC_REF_HOST_FUNC) {
      CODE_COVERAGE(143); // Hit
      LongPtr lpHostFunc = DynamicPtr_decode_long(vm, reg2 /* target */);
      reg2 = READ_FIELD_2(lpHostFunc, TsHostFunc, indexInImportTable);
      goto SUB_CALL_HOST_COMMON;
    } else if (tc == TC_REF_CLOSURE) {
      CODE_COVERAGE(598); // Hit

      // Closures are their own scope
      reg3 /* scope */ = reg2;

      LongPtr lpClosure = DynamicPtr_decode_long(vm, reg2 /* target */);
      reg2 /* target */ = READ_FIELD_2(lpClosure, TsClosure, target);

      // Redirect the call to closure target
      continue;
    } else if (tc == TC_VAL_NO_OP_FUNC) {
      CODE_COVERAGE(653); // Hit
      reg3 /* callee argCountAndFlags */ = reg1;
      reg1 /* result */ = VM_VALUE_UNDEFINED;
      goto SUB_POP_ARGS;
    } else {
      CODE_COVERAGE_UNTESTED(264); // Not hit
      // Other value types are not callable
      err = vm_newError(vm, MVM_E_TYPE_ERROR_TARGET_IS_NOT_CALLABLE);
      goto SUB_EXIT;
    }
  }
}

/* ------------------------------------------------------------------------- */
/*                          SUB_CALL_HOST_COMMON                             */
/*   Expects:                                                                */
/*     reg1: argCountAndFlags                                                */
/*     reg2: index in import table                                           */
/* ------------------------------------------------------------------------- */
SUB_CALL_HOST_COMMON: {
  CODE_COVERAGE(162); // Hit

  // Note: the interface with the host doesn't include the `this` pointer as the
  // first argument, so `args` points to the *next* argument.
  reg3 /* argCount */ = (reg1 & AF_ARG_COUNT_MASK) - 1;

  // Allocating the result on the stack so that it's reachable by the GC
  Value* pResult = pStackPointer++;
  *pResult = VM_VALUE_UNDEFINED;

  // The function `mvm_asyncStart` needs to know the state of the callee flag
  // AF_VOID_CALLED, but we need to save the original state to restore later.
  uint16_t saveArgCountAndFlags = reg->argCountAndFlags;
  reg->argCountAndFlags = reg1;

  VM_ASSERT(vm, reg2 < vm_getResolvedImportCount(vm));
  mvm_TfHostFunction hostFunction = vm_getResolvedImports(vm)[reg2];
  mvm_HostFunctionID hostFunctionID = vm_getHostFunctionId(vm, reg2);

  FLUSH_REGISTER_CACHE();

  /*
  Note: this subroutine does not call PUSH_REGISTERS to save the frame boundary.
  Calls to the host can be thought of more like machine instructions than
  distinct CALL operations in this sense, since they operate within the frame of
  the caller.

  This needs to work even if the host in turn calls the VM again during the call
  out to the host. When the host calls the VM again, it will push a new stack
  frame.
  */

  #if (MVM_SAFE_MODE)
    // Take a copy of the registers so we can see later that they're restored to
    // their correct values.
    VM_ASSERT_NOT_USING_CACHED_REGISTERS(vm);
    vm_TsRegisters regCopy = *reg;
    // Except that the `closure` register may point to a heap value, so we need
    // to track if it moves.
    mvm_Handle hClosureCopy;
    mvm_initializeHandle(vm, &hClosureCopy);
    mvm_handleSet(&hClosureCopy, reg->closure);
  #endif

  regP1 /* pArgs */ = reg->pStackPointer - reg3 - 1;

  // Call the host function
  err = hostFunction(vm, hostFunctionID, pResult, regP1, (uint8_t)reg3);

  #if (MVM_SAFE_MODE)
    VM_ASSERT_NOT_USING_CACHED_REGISTERS(vm);
    regCopy.closure = mvm_handleGet(&hClosureCopy);
    mvm_releaseHandle(vm, &hClosureCopy);

    /*
    The host function should leave the VM registers in the same state.

    `pStackPointer` can be modified temporarily because the host may call back
    into the VM, but it should be restored again by the time the host returns,
    otherwise the stack is unbalanced.

    The other registers (e.g. lpProgramCounter) should only be modified by
    bytecode instructions, which can be if the host calls back into the VM. But
    if the host calls back into the VM, it will go through SUB_CALL which
    performs a PUSH_REGISTERS to save the previous machine state, and then will
    restore the machine state when it returns.

    This check is also what confirms that we don't need a FLUSH_REGISTER_CACHE
    and CACHE_REGISTERS around the host call, since the host doesn't modify or
    use these registers, even if it calls back into the VM (with the exception
    of the stack pointer which is used but restored again afterward).
    */
    regCopy.cpsCallback = reg->cpsCallback; // The cpsCallback register is not preserved
    regCopy.jobQueue = reg->jobQueue; // The job queue may change
    VM_ASSERT(vm, memcmp(&regCopy, reg, sizeof regCopy) == 0);
  #endif

  CACHE_REGISTERS();

  // Restore caller argCountAndFlags
  reg->argCountAndFlags = saveArgCountAndFlags;

  // Represents an exception thrown by the host function that wasn't caught by
  // the host. The pResult should reference the exception object.
  if (err == MVM_E_UNCAUGHT_EXCEPTION) {
    CODE_COVERAGE_UNTESTED(734); // Not hit
    reg1 = *pResult;
    err = MVM_E_SUCCESS;
    // The throw will unwind the stack to the closest catch block, so we don't
    // need to worry about unwinding the arguments off the stack.
    goto SUB_THROW;
  } else if (err != MVM_E_SUCCESS) {
    CODE_COVERAGE_ERROR_PATH(735); // Not hit
    goto SUB_EXIT;
  } else {
    CODE_COVERAGE(736); // Not hit
  }

  reg3 = reg1; // Callee argCountAndFlags
  reg1 = *pResult;

  // Pop the result slot
  POP();

  goto SUB_POP_ARGS;
}

/* ------------------------------------------------------------------------- */
/*                         SUB_CALL_BYTECODE_FUNC                            */
/*                                                                           */
/*   Calls a bytecode function                                               */
/*                                                                           */
/*   Expects:                                                                */
/*     reg1: new argCountAndFlags                                            */
/*     reg2: offset of target function in bytecode                           */
/*     reg3: scope, if reg1 & AF_SCOPE, else unused                          */
/* ------------------------------------------------------------------------- */
SUB_CALL_BYTECODE_FUNC: {
  CODE_COVERAGE(163); // Hit

  regP1 /* pArgs */ = pStackPointer - (reg1 & AF_ARG_COUNT_MASK);
  regLP1 /* lpReturnAddress */ = lpProgramCounter;

  // Move PC to point to new function code
  lpProgramCounter = LongPtr_add(vm->lpBytecode, reg2);

  reg2 /* function header */ = LongPtr_read2_aligned(LongPtr_add(lpProgramCounter, -2));

  // If it's a continuation (async resume point), we actually want the function
  // header of the containing function
  if (reg2 & VM_FUNCTION_HEADER_CONTINUATION_FLAG) {
    CODE_COVERAGE(737); // Not hit
    reg2 /* back pointer */ = reg2 & VM_FUNCTION_HEADER_BACK_POINTER_MASK;
    reg2 /* function header */ = LongPtr_read2_aligned(LongPtr_add(lpProgramCounter, - reg2 * 4 - 2));
  } else {
    CODE_COVERAGE(738); // Not hit
  }

  // Check the stack space required (before we PUSH_REGISTERS). Note that the
  // frame size in words is stored in the header itself
  reg2 /* requiredFrameSizeWords */ = reg2 /* function header */ & VM_FUNCTION_HEADER_STACK_HEIGHT_MASK;
  reg2 /* requiredFrameSizeWords */ += VM_FRAME_BOUNDARY_SAVE_SIZE_WORDS;
  // The +5 is for various temporaries that `mvm_call` pushes to the stack, and
  // the result slot if we call the host
  err = vm_requireStackSpace(vm, pStackPointer, reg2 /* requiredFrameSizeWords */ + 5);
  if (err != MVM_E_SUCCESS) {
    CODE_COVERAGE_ERROR_PATH(226); // Not hit
    goto SUB_EXIT;
  }

  // Save old registers to the stack
  PUSH_REGISTERS(regLP1);

  // Set up new frame
  pFrameBase = pStackPointer;
  reg->argCountAndFlags = reg1;
  reg->closure = reg3;
  reg->pArgs = regP1;

  goto SUB_TAIL_POP_0_PUSH_0;
} // End of SUB_CALL_BYTECODE_FUNC

/* ------------------------------------------------------------------------- */
/*                             SUB_NUM_OP_FLOAT64                            */
/*   Expects:                                                                */
/*     reg1: left operand (second pop), or zero for unary ops                */
/*     reg2: right operand (first pop), or single operand for unary ops      */
/*     reg3: vm_TeNumberOp                                                   */
/* ------------------------------------------------------------------------- */
#if MVM_SUPPORT_FLOAT
SUB_NUM_OP_FLOAT64: {
  CODE_COVERAGE_UNIMPLEMENTED(447); // Hit

  MVM_FLOAT64 reg1F = 0;
  if (reg1) reg1F = mvm_toFloat64(vm, reg1);
  MVM_FLOAT64 reg2F = mvm_toFloat64(vm, reg2);

  VM_ASSERT(vm, reg3 < VM_NUM_OP_END);
  MVM_SWITCH (reg3, (VM_NUM_OP_END - 1)) {
    MVM_CASE(VM_NUM_OP_LESS_THAN): {
      CODE_COVERAGE(449); // Hit
      reg1 = reg1F < reg2F;
      goto SUB_TAIL_PUSH_REG1_BOOL;
    }
    MVM_CASE(VM_NUM_OP_GREATER_THAN): {
      CODE_COVERAGE(450); // Hit
      reg1 = reg1F > reg2F;
      goto SUB_TAIL_PUSH_REG1_BOOL;
    }
    MVM_CASE(VM_NUM_OP_LESS_EQUAL): {
      CODE_COVERAGE(451); // Hit
      reg1 = reg1F <= reg2F;
      goto SUB_TAIL_PUSH_REG1_BOOL;
    }
    MVM_CASE(VM_NUM_OP_GREATER_EQUAL): {
      CODE_COVERAGE(452); // Hit
      reg1 = reg1F >= reg2F;
      goto SUB_TAIL_PUSH_REG1_BOOL;
    }
    MVM_CASE(VM_NUM_OP_ADD_NUM): {
      CODE_COVERAGE(453); // Hit
      reg1F = reg1F + reg2F;
      break;
    }
    MVM_CASE(VM_NUM_OP_SUBTRACT): {
      CODE_COVERAGE(454); // Hit
      reg1F = reg1F - reg2F;
      break;
    }
    MVM_CASE(VM_NUM_OP_MULTIPLY): {
      CODE_COVERAGE(455); // Hit
      reg1F = reg1F * reg2F;
      break;
    }
    MVM_CASE(VM_NUM_OP_DIVIDE): {
      CODE_COVERAGE(456); // Hit
      reg1F = reg1F / reg2F;
      break;
    }
    MVM_CASE(VM_NUM_OP_DIVIDE_AND_TRUNC): {
      CODE_COVERAGE(457); // Hit
      reg1F = mvm_float64ToInt32((reg1F / reg2F));
      break;
    }
    MVM_CASE(VM_NUM_OP_REMAINDER): {
      CODE_COVERAGE(458); // Hit
      reg1F = fmod(reg1F, reg2F);
      break;
    }
    MVM_CASE(VM_NUM_OP_POWER): {
      CODE_COVERAGE(459); // Hit
      if (!isfinite(reg2F) && ((reg1F == 1.0) || (reg1F == -1.0))) {
        reg1 = VM_VALUE_NAN;
        goto SUB_TAIL_POP_0_PUSH_REG1;
      }
      reg1F = pow(reg1F, reg2F);
      break;
    }
    MVM_CASE(VM_NUM_OP_NEGATE): {
      CODE_COVERAGE(460); // Hit
      reg1F = -reg2F;
      break;
    }
    MVM_CASE(VM_NUM_OP_UNARY_PLUS): {
      CODE_COVERAGE(461); // Hit
      reg1F = reg2F;
      break;
    }
  } // End of switch vm_TeNumberOp for float64

  // Convert the result from a float
  FLUSH_REGISTER_CACHE();
  reg1 = mvm_newNumber(vm, reg1F);
  CACHE_REGISTERS();
  goto SUB_TAIL_POP_0_PUSH_REG1;
} // End of SUB_NUM_OP_FLOAT64
#endif // MVM_SUPPORT_FLOAT

/* --------------------------------------------------------------------------
                                     TAILS

These "tails" are the common epilogues to various instructions. Instructions in
general must keep their arguments on the stack right until the end, to prevent
any pointer arguments from becoming dangling if the instruction triggers a GC
collection. So popping the arguments is done at the end of the instruction, and
the number of pops is common to many different instructions.
 * -------------------------------------------------------------------------- */

SUB_TAIL_PUSH_REG1_BOOL:
  CODE_COVERAGE(489); // Hit
  reg1 = reg1 ? VM_VALUE_TRUE : VM_VALUE_FALSE;
  goto SUB_TAIL_POP_0_PUSH_REG1;

SUB_TAIL_POP_2_PUSH_REG1:
  CODE_COVERAGE(227); // Hit
  pStackPointer -= 1;
  goto SUB_TAIL_POP_1_PUSH_REG1;

SUB_TAIL_POP_0_PUSH_REG1:
  CODE_COVERAGE(164); // Hit
  PUSH(reg1);
  goto SUB_TAIL_POP_0_PUSH_0;

SUB_TAIL_POP_3_PUSH_0:
  CODE_COVERAGE(611); // Hit
  pStackPointer -= 3;
  goto SUB_TAIL_POP_0_PUSH_0;

SUB_TAIL_POP_1_PUSH_0:
  CODE_COVERAGE(617); // Hit
  pStackPointer -= 1;
  goto SUB_TAIL_POP_0_PUSH_0;

SUB_TAIL_POP_1_PUSH_REG1:
  CODE_COVERAGE(126); // Hit
  pStackPointer[-1] = reg1;
  goto SUB_TAIL_POP_0_PUSH_0;

SUB_TAIL_POP_0_PUSH_0:
  CODE_COVERAGE(125); // Hit
  if (err != MVM_E_SUCCESS) goto SUB_EXIT;
  goto SUB_DO_NEXT_INSTRUCTION;

SUB_EXIT:
  CODE_COVERAGE(165); // Hit

  #if MVM_SAFE_MODE
  FLUSH_REGISTER_CACHE();
  VM_ASSERT(vm, registerValuesAtEntry.pStackPointer <= reg->pStackPointer);
  VM_ASSERT(vm, registerValuesAtEntry.pFrameBase <= reg->pFrameBase);
  #endif

  // I don't think there's anything that can happen during mvm_call that can
  // justify the values of the registers at exit needing being different to
  // those at entry. Restoring the entry registers here means that if we have an
  // error or uncaught exception at any time during the call (including the case
  // where it's within nested calls) then at least we unwind the stack and
  // restore the original program counter, catchTarget, stackPointer etc.
  // `registerValuesAtEntry` was also captured before we pushed the mvm_call
  // arguments to the stack, so this also effectively pops the arguments off the
  // stack.
  registerValuesAtEntry.jobQueue = reg->jobQueue; // Except the job queue needs to be preserved
  registerValuesAtEntry.closure = reg->closure; // And the closure may point to the GC so it may change physical value if there are garbage collections during the call.
  *reg = registerValuesAtEntry;

  // If the stack is empty, we can free it. It may not be empty if this is a
  // reentrant call, in which case there would be other frames below this one.
  if (reg->pStackPointer == getBottomOfStack(vm->stack)) {
    CODE_COVERAGE(222); // Hit

    vm_free(vm, vm->stack);
    vm->stack = NULL;
  }

  return err;
} // End of mvm_call

/**
 * Creates a new array of length 0 and the given capacity and initializes the
 * allocated slots to VM_VALUE_DELETED.
 */
static Value vm_newArray(VM* vm, uint16_t capacity) {
  VM_ASSERT_NOT_USING_CACHED_REGISTERS(vm);
  TABLE_COVERAGE(capacity ? 1 : 0, 2, 371); // Hit 2/2

  TsArray* arr = GC_ALLOCATE_TYPE(vm, TsArray, TC_REF_ARRAY);
  Value result = ShortPtr_encode(vm, arr);

  arr->viLength = VirtualInt14_encode(vm, 0);
  arr->dpData = VM_VALUE_NULL;

  if (capacity) {
    vm_push(vm, result); // GC-reachable
    uint16_t* pData = mvm_allocate(vm, capacity * 2, TC_REF_FIXED_LENGTH_ARRAY);
    result = vm_pop(vm);
    arr = ShortPtr_decode(vm, result); // Invalidated
    arr->dpData = ShortPtr_encode(vm, pData);
    uint16_t* p = pData;
    uint16_t n = capacity;
    while (n--)
      *p++ = VM_VALUE_DELETED;
  }

  return result;
}

/**
 * Add an element to an array. Note that the array may need to expand, so the
 * arguments should be passed in by reference to stable slots (e.g. stack slots
 * or registers).
 */
static void vm_arrayPush(VM* vm, Value* pvArr, Value* pvItem) {
  VM_ASSERT_NOT_USING_CACHED_REGISTERS(vm);

  CODE_COVERAGE(710); // Hit

  TsArray* pArr = ShortPtr_decode(vm, *pvArr);
  uint16_t length = VirtualInt14_decode(vm, pArr->viLength);
  uint16_t capacity;

  if (pArr->dpData == VM_VALUE_NULL) {
    CODE_COVERAGE_UNTESTED(711); // Not hit
    capacity = 0;
  } else {
    CODE_COVERAGE(712); // Hit
    capacity = vm_getAllocationSize(ShortPtr_decode(vm, pArr->dpData)) / 2;
  }

  // Need to expand?
  if (length >= capacity) {
    CODE_COVERAGE(713); // Hit
    // Slow path
    capacity = capacity * 2;
    if (capacity < VM_ARRAY_INITIAL_CAPACITY) {
      capacity = VM_ARRAY_INITIAL_CAPACITY;
    }
    // We're only adding 1 item to the array, so if we're doubling the capacity
    // then we know that the new capacity will at least contain the new item.
    VM_ASSERT(vm, capacity >= length + 1);

    growArray(vm, pvArr, length + 1, capacity);
  }

  // Write the item to the array
  pArr = ShortPtr_decode(vm, *pvArr); // May have moved
  uint16_t* pData = ShortPtr_decode(vm, pArr->dpData);
  pData[length] = *pvItem;
  pArr->viLength = VirtualInt14_encode(vm, length + 1);
}

static void vm_scheduleContinuation(VM* vm, Value continuation, Value isSuccess, Value resultOrError) {
  VM_ASSERT_NOT_USING_CACHED_REGISTERS(vm);
  CODE_COVERAGE(714); // Hit

  vm_push(vm, resultOrError); // anchor to GC
  vm_push(vm, continuation); // anchor to GC

  Value* closure = mvm_allocate(vm, 4 * 2, TC_REF_CLOSURE);

  closure[0] = getBuiltin(vm, BIN_ASYNC_CONTINUE);
  closure[1] = vm_pop(vm); // continuation
  closure[2] = isSuccess;
  closure[3] = vm_pop(vm); // resultOrError

  vm_enqueueJob(vm, ShortPtr_encode(vm, closure));
}


/**
 * Creates a new closure with `slotCount` slots and sets it as the active
 * closure. If `captureParent` is true then the last slot of the new closure
 * will be set to reference the previously active closure.
 */
static uint16_t* vm_scopePushOrNew(VM* vm, int slotCount, bool captureParent) {
  VM_ASSERT_NOT_USING_CACHED_REGISTERS(vm);
  int size = slotCount * 2;

  uint16_t* newScope = mvm_allocate(vm, size, TC_REF_CLOSURE);

  uint16_t* p = newScope;
  while (--slotCount) { // Note: pre-decrement so will stop one short of the end
    *p++ = VM_VALUE_DELETED; // Initial slot values
  }
  // Last slot
  if (captureParent) {
    CODE_COVERAGE(646); // Hit
    *p = vm->stack->reg.closure; // Reference to parent (last slot)
  } else {
    CODE_COVERAGE(647); // Hit
    *p = VM_VALUE_DELETED;
  }
  // Add to the scope chain
  vm->stack->reg.closure = ShortPtr_encode(vm, newScope);

  return newScope;
}

/**
 * Same as mvm_call but takes a `thisValue`. I expect this to be the less common
 * case, so I've separated it out to avoid the interface complexity of passing a
 * `thisValue` when it's not needed.
 */
TeError mvm_callEx(VM* vm, Value targetFunc, Value thisValue, Value* out_result, Value* args, uint8_t argCount) {
  mvm_TeError err;

  CODE_COVERAGE_UNTESTED(659); // Hit

  if (!vm->stack) {
    CODE_COVERAGE_UNTESTED(660); // Not hit
    err = vm_createStackAndRegisters(vm);
    if (err != MVM_E_SUCCESS) {
      return err;
    }
  } else {
    CODE_COVERAGE_UNTESTED(661); // Hit
  }

  err = vm_requireStackSpace(vm, vm->stack->reg.pStackPointer, argCount + 2);
  if (err) return err;

  // Put the this value on the stack without bumping the stack pointer. I do it
  // this way because mvm_call has checks on the stack balance so we can't just
  // push it here and expect mvm_call to pop it later. The first position on the
  // stack is reserved for the target function, and the second value will be the
  // `this` value.
  vm->stack->reg.pStackPointer[1] = thisValue;
  // This is a little bit of a hack to tell mvm_call that `this` is already on
  // the stack. I didn't want to play with the arguments to mvm_call because
  // it's a public interface, and I didn't want to pass the `this` value through
  // a register because that's less space efficient when this feature is not
  // used.
  vm->stack->reg.argCountAndFlags |= AF_OVERRIDE_THIS;

  return mvm_call(vm, targetFunc, out_result, args, argCount);
}

const Value mvm_undefined = VM_VALUE_UNDEFINED;
const Value mvm_null = VM_VALUE_NULL;

static inline uint16_t vm_getAllocationSize(void* pAllocation) {
  CODE_COVERAGE(12); // Hit
  return vm_getAllocationSizeExcludingHeaderFromHeaderWord(((uint16_t*)pAllocation)[-1]);
}

static inline TeTypeCode vm_getAllocationType(void* pAllocation) {
  CODE_COVERAGE(682); // Hit
  return vm_getTypeCodeFromHeaderWord(((uint16_t*)pAllocation)[-1]);
}

static inline uint16_t vm_getAllocationSize_long(LongPtr lpAllocation) {
  CODE_COVERAGE(514); // Hit
  uint16_t headerWord = LongPtr_read2_aligned(LongPtr_add(lpAllocation, -2));
  return vm_getAllocationSizeExcludingHeaderFromHeaderWord(headerWord);
}

static inline mvm_TeBytecodeSection vm_sectionAfter(VM* vm, mvm_TeBytecodeSection section) {
  CODE_COVERAGE(13); // Hit
  VM_ASSERT(vm, section < BCS_SECTION_COUNT - 1);
  return (mvm_TeBytecodeSection)((uint8_t)section + 1);
}

static inline TeTypeCode vm_getTypeCodeFromHeaderWord(uint16_t headerWord) {
  CODE_COVERAGE(1); // Hit
  // The type code is in the high byte because it's the byte that occurs closest
  // to the allocation itself, potentially allowing us in future to omit the
  // size in the allocation header for some kinds of allocations.
  return (TeTypeCode)(headerWord >> 12);
}

static inline uint16_t vm_makeHeaderWord(VM* vm, TeTypeCode tc, uint16_t size) {
  CODE_COVERAGE(210); // Hit
  VM_ASSERT(vm, size <= MAX_ALLOCATION_SIZE);
  VM_ASSERT(vm, tc <= 0xF);
  return ((tc << 12) | size);
}

static inline VirtualInt14 VirtualInt14_encode(VM* vm, int16_t i) {
  CODE_COVERAGE(14); // Hit
  VM_ASSERT(vm, (i >= VM_MIN_INT14) && (i <= VM_MAX_INT14));
  return VIRTUAL_INT14_ENCODE(i);
}

static inline int16_t VirtualInt14_decode(VM* vm, VirtualInt14 viInt) {
  CODE_COVERAGE(16); // Hit
  VM_ASSERT(vm, Value_isVirtualInt14(viInt));
  return (int16_t)viInt >> 2;
}

static void setHeaderWord(VM* vm, void* pAllocation, TeTypeCode tc, uint16_t size) {
  CODE_COVERAGE(36); // Hit
  ((uint16_t*)pAllocation)[-1] = vm_makeHeaderWord(vm, tc, size);
}

// Returns the allocation size, excluding the header itself
static inline uint16_t vm_getAllocationSizeExcludingHeaderFromHeaderWord(uint16_t headerWord) {
  CODE_COVERAGE(2); // Hit
  // Note: The header size is measured in bytes and not words mainly to account
  // for string allocations, which would be inconvenient to align to word
  // boundaries.
  return headerWord & 0xFFF;
}

#if MVM_SAFE_MODE
static bool Value_encodesBytecodeMappedPtr(Value value) {
  CODE_COVERAGE(37); // Hit
  return ((value & 3) == 1) && value >= VM_VALUE_WELLKNOWN_END;
}
#endif // MVM_SAFE_MODE

static inline uint16_t getSectionOffset(LongPtr lpBytecode, mvm_TeBytecodeSection section) {
  CODE_COVERAGE(38); // Hit
  LongPtr lpSection = LongPtr_add(lpBytecode, OFFSETOF(mvm_TsBytecodeHeader, sectionOffsets) + section * 2);
  uint16_t offset = LongPtr_read2_aligned(lpSection);
  return offset;
}

#if MVM_SAFE_MODE
static inline uint16_t vm_getResolvedImportCount(VM* vm) {
  CODE_COVERAGE(41); // Hit
  uint16_t importTableSize = getSectionSize(vm, BCS_IMPORT_TABLE);
  uint16_t importCount = importTableSize / sizeof(vm_TsImportTableEntry);
  return importCount;
}
#endif // MVM_SAFE_MODE

#if MVM_SAFE_MODE
/**
 * Returns true if the value is a pointer which points to ROM. Null is not a
 * value that points to ROM.
 */
static bool DynamicPtr_isRomPtr(VM* vm, DynamicPtr dp) {
  CODE_COVERAGE(39); // Hit
  VM_ASSERT(vm, !Value_isVirtualInt14(dp));

  if (dp == VM_VALUE_NULL) {
    CODE_COVERAGE_UNTESTED(47); // Not hit
    return false;
  }

  if (Value_isShortPtr(dp)) {
    CODE_COVERAGE_UNTESTED(52); // Not hit
    return false;
  }
  CODE_COVERAGE(91); // Hit

  VM_ASSERT(vm, Value_encodesBytecodeMappedPtr(dp));
  VM_ASSERT(vm, vm_sectionAfter(vm, BCS_ROM) < BCS_SECTION_COUNT);

  uint16_t offset = dp & 0xFFFE;

  return (offset >= getSectionOffset(vm->lpBytecode, BCS_ROM))
    & (offset < getSectionOffset(vm->lpBytecode, vm_sectionAfter(vm, BCS_ROM)));
}
#endif // MVM_SAFE_MODE

TeError mvm_restore(mvm_VM** result, MVM_LONG_PTR_TYPE lpBytecode, size_t bytecodeSize_, void* context, mvm_TfResolveImport resolveImport) {
  // Note: these are declared here because some compilers give warnings when "goto" bypasses some variable declarations
  mvm_TfHostFunction* resolvedImports;
  uint16_t importTableOffset;
  LongPtr lpImportTableStart;
  LongPtr lpImportTableEnd;
  mvm_TfHostFunction* resolvedImport;
  LongPtr lpImportTableEntry;
  uint16_t initialHeapOffset;
  uint16_t initialHeapSize;

  CODE_COVERAGE(3); // Hit

  if (MVM_PORT_VERSION != MVM_EXPECTED_PORT_FILE_VERSION) {
    return MVM_E_PORT_FILE_VERSION_MISMATCH;
  }

  #if MVM_SAFE_MODE
    uint16_t x = 0x4243;
    bool isLittleEndian = ((uint8_t*)&x)[0] == 0x43;
    VM_ASSERT(NULL, isLittleEndian);
    VM_ASSERT(NULL, sizeof (ShortPtr) == 2);
  #endif

  TeError err = MVM_E_SUCCESS;
  VM* vm = NULL;

  // Bytecode size field is located at the second word
  if (bytecodeSize_ < sizeof (mvm_TsBytecodeHeader)) {
    CODE_COVERAGE_ERROR_PATH(21); // Not hit
    return MVM_E_INVALID_BYTECODE;
  }
  mvm_TsBytecodeHeader header;
  memcpy_long(&header, lpBytecode, sizeof header);

  // Note: the restore function takes an explicit bytecode size because there
  // may be a size inherent to the medium from which the bytecode image comes,
  // and we don't want to accidentally read past the end of this space just
  // because the header apparently told us we could (since we could be reading a
  // corrupt header).
  uint16_t bytecodeSize = header.bytecodeSize;
  if (bytecodeSize != bytecodeSize_) {
    CODE_COVERAGE_ERROR_PATH(240); // Not hit
    return MVM_E_INVALID_BYTECODE;
  }

  uint16_t expectedCRC = header.crc;
  if (!MVM_CHECK_CRC16_CCITT(LongPtr_add(lpBytecode, 8), (uint16_t)bytecodeSize - 8, expectedCRC)) {
    CODE_COVERAGE_ERROR_PATH(54); // Not hit
    return MVM_E_BYTECODE_CRC_FAIL;
  }

  if (bytecodeSize < header.headerSize) {
    CODE_COVERAGE_ERROR_PATH(241); // Not hit
    return MVM_E_INVALID_BYTECODE;
  }

  if (header.bytecodeVersion != MVM_ENGINE_MAJOR_VERSION) {
    CODE_COVERAGE_ERROR_PATH(430); // Not hit
    return MVM_E_WRONG_BYTECODE_VERSION;
  }

  if (MVM_ENGINE_MINOR_VERSION < header.requiredEngineVersion) {
    CODE_COVERAGE_ERROR_PATH(247); // Not hit
    return MVM_E_REQUIRES_LATER_ENGINE;
  }

  uint32_t featureFlags = header.requiredFeatureFlags;;
  if (MVM_SUPPORT_FLOAT && !(featureFlags & (1 << FF_FLOAT_SUPPORT))) {
    CODE_COVERAGE_ERROR_PATH(180); // Not hit
    return MVM_E_BYTECODE_REQUIRES_FLOAT_SUPPORT;
  }

  err = vm_validatePortFileMacros(lpBytecode, &header, context);
  if (err) return err;

  uint16_t importTableSize = header.sectionOffsets[vm_sectionAfter(vm, BCS_IMPORT_TABLE)] - header.sectionOffsets[BCS_IMPORT_TABLE];
  uint16_t importCount = importTableSize / sizeof (vm_TsImportTableEntry);

  uint16_t globalsSize = header.sectionOffsets[vm_sectionAfter(vm, BCS_GLOBALS)] - header.sectionOffsets[BCS_GLOBALS];

  size_t allocationSize = sizeof(mvm_VM) +
    sizeof(mvm_TfHostFunction) * importCount +  // Import table
    globalsSize; // Globals
  vm = (VM*)MVM_CONTEXTUAL_MALLOC(allocationSize, context);
  if (!vm) {
    CODE_COVERAGE_ERROR_PATH(139); // Not hit
    err = MVM_E_MALLOC_FAIL;
    goto SUB_EXIT;
  }
  #if MVM_SAFE_MODE
    memset(vm, 0xCC, allocationSize);
  #endif
  memset(vm, 0, sizeof (mvm_VM));
  resolvedImports = vm_getResolvedImports(vm);
  vm->context = context;
  vm->lpBytecode = lpBytecode;
  vm->globals = (void*)(resolvedImports + importCount);
  #ifdef MVM_GAS_COUNTER
  vm->stopAfterNInstructions = -1;
  #endif

  importTableOffset = header.sectionOffsets[BCS_IMPORT_TABLE];
  lpImportTableStart = LongPtr_add(lpBytecode, importTableOffset);
  lpImportTableEnd = LongPtr_add(lpImportTableStart, importTableSize);
  // Resolve imports (linking)
  resolvedImport = resolvedImports;
  lpImportTableEntry = lpImportTableStart;
  while (lpImportTableEntry < lpImportTableEnd) {
    CODE_COVERAGE(431); // Hit
    mvm_HostFunctionID hostFunctionID = READ_FIELD_2(lpImportTableEntry, vm_TsImportTableEntry, hostFunctionID);
    lpImportTableEntry = LongPtr_add(lpImportTableEntry, sizeof (vm_TsImportTableEntry));
    mvm_TfHostFunction handler = NULL;
    err = resolveImport(hostFunctionID, context, &handler);
    if (err != MVM_E_SUCCESS) {
      CODE_COVERAGE_ERROR_PATH(432); // Not hit
      goto SUB_EXIT;
    }
    if (!handler) {
      CODE_COVERAGE_ERROR_PATH(433); // Not hit
      err = MVM_E_UNRESOLVED_IMPORT;
      goto SUB_EXIT;
    } else {
      CODE_COVERAGE(434); // Hit
    }
    *resolvedImport++ = handler;
  }

  // The GC is empty to start
  gc_freeGCMemory(vm);

  // Initialize data
  memcpy_long(vm->globals, getBytecodeSection(vm, BCS_GLOBALS, NULL), globalsSize);

  // Initialize heap
  initialHeapOffset = header.sectionOffsets[BCS_HEAP];
  initialHeapSize = bytecodeSize - initialHeapOffset;
  vm->heapSizeUsedAfterLastGC = initialHeapSize;
  vm->heapHighWaterMark = initialHeapSize;

  if (initialHeapSize) {
    CODE_COVERAGE(435); // Hit
    if (initialHeapSize > MVM_MAX_HEAP_SIZE) {
      MVM_FATAL_ERROR(vm, MVM_E_OUT_OF_MEMORY);
    }
    // The initial heap needs to be 2-byte aligned because we start appending
    // new allocations to the end of it directly.
    VM_ASSERT(vm, initialHeapSize % 2 == 0);
    gc_createNextBucket(vm, initialHeapSize, initialHeapSize);
    VM_ASSERT(vm, !vm->pLastBucket->prev); // Only one bucket
    uint16_t* heapStart = getBucketDataBegin(vm->pLastBucket);
    memcpy_long(heapStart, LongPtr_add(lpBytecode, initialHeapOffset), initialHeapSize);
    vm->pLastBucket->pEndOfUsedSpace = (uint16_t*)((intptr_t)vm->pLastBucket->pEndOfUsedSpace + initialHeapSize);

    // The running VM assumes the invariant that all pointers to the heap are
    // represented as ShortPtr (and no others). We only need to call
    // `loadPointers` if there is an initial heap at all, otherwise there
    // will be no pointers to it.
    loadPointers(vm, (uint8_t*)heapStart);
  } else {
    CODE_COVERAGE(436); // Hit
  }

  #if MVM_DEBUG_UTILS
  // Dummy code to prevent optimizer collection of debug utils, which may only
  // be used in the debugger and so might be optimized out unless we pretend to
  // use them. This code should never execute but I'm hoping that the optimizer
  // doesn't realize that.
  if ((intptr_t)vm == -1) {
    mvm_checkHeap(vm);
    mvm_readHeapCount(vm);
    mvm_checkValue(vm, 0);
    mvm_readCallStack(vm, 0);
    mvm_readHeap(vm, 0);
  }
  #endif

SUB_EXIT:
  if (err != MVM_E_SUCCESS) {
    CODE_COVERAGE_ERROR_PATH(437); // Not hit
    *result = NULL;
    if (vm) {
      vm_free(vm, vm);
      vm = NULL;
    } else {
      CODE_COVERAGE_ERROR_PATH(438); // Not hit
    }
  } else {
    CODE_COVERAGE(439); // Hit
  }
  *result = vm;
  return err;
}

static inline uint16_t getBytecodeSize(VM* vm) {
  CODE_COVERAGE_UNTESTED(168); // Not hit
  LongPtr lpBytecodeSize = LongPtr_add(vm->lpBytecode, OFFSETOF(mvm_TsBytecodeHeader, bytecodeSize));
  return LongPtr_read2_aligned(lpBytecodeSize);
}

static LongPtr getBytecodeSection(VM* vm, mvm_TeBytecodeSection id, LongPtr* out_end) {
  CODE_COVERAGE(170); // Hit
  LongPtr lpBytecode = vm->lpBytecode;
  LongPtr lpSections = LongPtr_add(lpBytecode, OFFSETOF(mvm_TsBytecodeHeader, sectionOffsets));
  LongPtr lpSection = LongPtr_add(lpSections, id * 2);
  uint16_t offset = LongPtr_read2_aligned(lpSection);
  LongPtr result = LongPtr_add(lpBytecode, offset);
  if (out_end) {
    CODE_COVERAGE(171); // Hit
    uint16_t endOffset;
    if (id == BCS_SECTION_COUNT - 1) {
      endOffset = getBytecodeSize(vm);
    } else {
      LongPtr lpNextSection = LongPtr_add(lpSection, 2);
      endOffset = LongPtr_read2_aligned(lpNextSection);
    }
    *out_end = LongPtr_add(lpBytecode, endOffset);
  } else {
    CODE_COVERAGE(172); // Hit
  }
  return result;
}

static uint16_t getSectionSize(VM* vm, mvm_TeBytecodeSection section) {
  CODE_COVERAGE(174); // Hit
  uint16_t sectionStart = getSectionOffset(vm->lpBytecode, section);
  uint16_t sectionEnd;
  if (section == BCS_SECTION_COUNT - 1) {
    CODE_COVERAGE_UNTESTED(175); // Not hit
    sectionEnd = getBytecodeSize(vm);
  } else {
    CODE_COVERAGE(177); // Hit
    VM_ASSERT(vm, section < BCS_SECTION_COUNT);
    sectionEnd = getSectionOffset(vm->lpBytecode, vm_sectionAfter(vm, section));
  }
  VM_ASSERT(vm, sectionEnd >= sectionStart);
  return sectionEnd - sectionStart;
}

/**
 * Called at startup to translate all the pointers that point to GC memory into
 * ShortPtr for efficiency and to maintain invariants assumed in other places in
 * the code.
 */
static void loadPointers(VM* vm, uint8_t* heapStart) {
  CODE_COVERAGE(178); // Hit
  uint16_t n;
  uint16_t v;
  uint16_t* p;

  // Roots in global variables
  uint16_t globalsSize = getSectionSize(vm, BCS_GLOBALS);
  p = vm->globals;
  n = globalsSize / 2;
  TABLE_COVERAGE(n ? 1 : 0, 2, 179); // Hit 1/2
  while (n--) {
    v = *p;
    if (Value_isShortPtr(v)) {
      *p = ShortPtr_encode(vm, heapStart + v);
    }
    p++;
  }

  // Pointers in heap memory
  p = (uint16_t*)heapStart;
  VM_ASSERT(vm, vm->pLastBucketEndCapacity == vm->pLastBucket->pEndOfUsedSpace);
  uint16_t* heapEnd = vm->pLastBucketEndCapacity;
  while (p < heapEnd) {
    CODE_COVERAGE(181); // Hit
    uint16_t header = *p++;
    uint16_t size = vm_getAllocationSizeExcludingHeaderFromHeaderWord(header);
    uint16_t words = (size + 1) / 2;
    TeTypeCode tc = vm_getTypeCodeFromHeaderWord(header);

    if (tc < TC_REF_DIVIDER_CONTAINER_TYPES) { // Non-container types
      CODE_COVERAGE(182); // Hit
      p += words;
      continue;
    } // Else, container types
    CODE_COVERAGE(183); // Hit

    while (words--) {
      v = *p;
      if (Value_isShortPtr(v)) {
        *p = ShortPtr_encode(vm, heapStart + v);
      }
      p++;
    }
  }
}

void* mvm_getContext(VM* vm) {
  return vm->context;
}

// Note: mvm_free frees the VM, while vm_free is the counterpart to vm_malloc
void mvm_free(VM* vm) {
  CODE_COVERAGE(166); // Hit

  gc_freeGCMemory(vm);

  // The stack may be allocated if `mvm_free` is called from the an error
  // handler, right before terminating the thread or longjmp'ing out of the VM.
  #if MVM_SAFE_MODE
    if (vm->stack) {
      // This at least zeros out the registers, so the machine will crash early if
      // someone tries to the let it run after mvm_free
      memset(vm->stack, 0, sizeof(*vm->stack));
    }
  #endif
  // A compliant implementation of `free` will already check for null
  vm_free(vm, vm->stack);

  VM_EXEC_SAFE_MODE(memset(vm, 0, sizeof(*vm)));
  vm_free(vm, vm);
}

/**
 * @param sizeBytes Size in bytes of the allocation, *excluding* the header
 * @param typeCode The type code to insert into the header
 */
MVM_HIDDEN void* mvm_allocate(VM* vm, uint16_t sizeBytes,  uint8_t /*TeTypeCode*/ typeCode) {
  uint16_t* p;
  uint16_t* end;

  if (sizeBytes >= (MAX_ALLOCATION_SIZE + 1)) {
    CODE_COVERAGE_ERROR_PATH(353); // Not hit
    MVM_FATAL_ERROR(vm, MVM_E_ALLOCATION_TOO_LARGE);
  } else {
    CODE_COVERAGE(354); // Hit
  }

  // If we happened to trigger a GC collection, we need to know that the
  // registers are flushed, if they're allocated at all
  VM_ASSERT_NOT_USING_CACHED_REGISTERS(vm);

  CODE_COVERAGE(184); // Hit
  TsBucket* pBucket;
  const uint16_t sizeIncludingHeader = (sizeBytes + 3) & 0xFFFE;
  // + 2 bytes header, round up to 2-byte boundary
  VM_ASSERT(vm, (sizeIncludingHeader & 1) == 0);

  // Minimum allocation size is 4 bytes, because that's the size of a
  // tombstone. Note that nothing in code will attempt to allocate less,
  // since even a 1-char string (+null terminator) is a 4-byte allocation.
  VM_ASSERT(vm, sizeIncludingHeader >= 4);

  VM_POTENTIAL_GC_POINT(vm);

RETRY:
  pBucket = vm->pLastBucket;
  if (!pBucket) {
    CODE_COVERAGE(185); // Hit
    goto GROW_HEAP_AND_RETRY;
  }
  p = pBucket->pEndOfUsedSpace;
  end = (uint16_t*)((intptr_t)p + sizeIncludingHeader);
  if (end > vm->pLastBucketEndCapacity) {
    CODE_COVERAGE(186); // Hit
    goto GROW_HEAP_AND_RETRY;
  }
  pBucket->pEndOfUsedSpace = end;

  // Write header
  *p++ = vm_makeHeaderWord(vm, (TeTypeCode)typeCode, sizeBytes);

  return p;

GROW_HEAP_AND_RETRY:
  CODE_COVERAGE(187); // Hit
  gc_createNextBucket(vm, MVM_ALLOCATION_BUCKET_SIZE, sizeIncludingHeader);
  goto RETRY;
}

// Slow fallback for mvm_allocateWithConstantHeader
static void* mvm_allocateWithConstantHeaderSlow(VM* vm, uint16_t header) {
  CODE_COVERAGE(188); // Hit

  // If we happened to trigger a GC collection, we need to know that the
  // registers are flushed, if they're allocated at all
  VM_ASSERT(vm, !vm->stack || !vm->stack->reg.usingCachedRegisters);

  uint16_t size = vm_getAllocationSizeExcludingHeaderFromHeaderWord(header);
  TeTypeCode tc = vm_getTypeCodeFromHeaderWord(header);
  return mvm_allocate(vm, size, tc);
}

/*
 * This function is like mvm_allocate except that it's optimized for
 * situations where:
 *
 *   1. The header can be precomputed to a C constant, rather than assembling it
 *      from the size and type
 *   2. The size is known to be a multiple of 2 and at least 2 bytes
 *
 * This is more efficient in some cases because it has fewer checks and
 * preprocessing to do. This function can probably be inlined in some cases.
 *
 * Note: the size is passed separately rather than computed from the header
 * because this function is optimized for cases where the size is known at
 * compile time (and even better if this function is inlined).
 *
 * WARNING: this does not initialize the data in the allocation.
 */
static inline void* mvm_allocateWithConstantHeader(VM* vm, uint16_t header, uint16_t sizeIncludingHeader) {
  CODE_COVERAGE(189); // Hit

  uint16_t* p;
  uint16_t* end;

  // If we happened to trigger a GC collection, we need to know that the
  // registers are flushed, if they're allocated at all
  VM_ASSERT(vm, !vm->stack || !vm->stack->reg.usingCachedRegisters);

  VM_ASSERT(vm, sizeIncludingHeader % 2 == 0);
  VM_ASSERT(vm, sizeIncludingHeader >= 4);
  VM_ASSERT(vm, vm_getAllocationSizeExcludingHeaderFromHeaderWord(header) == sizeIncludingHeader - 2);

  VM_POTENTIAL_GC_POINT(vm);

  TsBucket* pBucket = vm->pLastBucket;
  if (!pBucket) {
    CODE_COVERAGE(190); // Hit
    goto SLOW;
  }
  p = pBucket->pEndOfUsedSpace;
  end = (uint16_t*)((intptr_t)p + sizeIncludingHeader);
  if (end > vm->pLastBucketEndCapacity) {
    CODE_COVERAGE(191); // Hit
    goto SLOW;
  }

  pBucket->pEndOfUsedSpace = end;
  *p++ = header;
  return p;

SLOW:
  CODE_COVERAGE(192); // Hit
  return mvm_allocateWithConstantHeaderSlow(vm, header);
}

// Looks for a variable in the closure scope chain based on its index. Scope
// records can be stored in ROM in some optimized cases, so this returns a long
// pointer.
static LongPtr vm_findScopedVariable(VM* vm, uint16_t varIndex) {
  // Slots are 2 bytes
  uint16_t offset = varIndex << 1;
  Value scope = vm->stack->reg.closure;
  while (true)
  {
    // The bytecode is corrupt or the compiler has a bug if we hit the bottom of
    // the scope chain without finding the variable.
    VM_ASSERT(vm, scope != VM_VALUE_DELETED);

    LongPtr lpArr = DynamicPtr_decode_long(vm, scope);
    uint16_t headerWord = readAllocationHeaderWord_long(lpArr);
    VM_ASSERT(vm, vm_getTypeCodeFromHeaderWord(headerWord) == TC_REF_CLOSURE);
    uint16_t arraySize = vm_getAllocationSizeExcludingHeaderFromHeaderWord(headerWord);
    if (offset < arraySize) {
      return LongPtr_add(lpArr, offset);
    } else {
      offset -= arraySize;
      // The reference to the parent is kept in the second slot
      scope = LongPtr_read2_aligned(LongPtr_add(lpArr, arraySize - 2));
    }
  }
}

// Read a scoped variable from the current closure. Assumes the current closure
// is stored in RAM.
static inline Value vm_readScopedFromThisClosure(VM* vm, uint16_t varIndex) {
  CODE_COVERAGE(700); // Hit
  Value* closure = ShortPtr_decode(vm, vm->stack->reg.closure);
  Value* slot = &closure[varIndex];
  VM_ASSERT(vm, slot == LongPtr_truncate(vm, vm_findScopedVariable(vm, varIndex)));
  return *slot;
}

// Read a scoped variable from the current closure. Assumes the current closure
// is stored in RAM.
static inline void vm_writeScopedToThisClosure(VM* vm, uint16_t varIndex, Value value) {
  CODE_COVERAGE(701); // Hit
  Value* closure = ShortPtr_decode(vm, vm->stack->reg.closure);
  Value* slot = &closure[varIndex];
  VM_ASSERT(vm, slot == LongPtr_truncate(vm, vm_findScopedVariable(vm, varIndex)));
  *slot = value;
}

static inline void* getBucketDataBegin(TsBucket* bucket) {
  CODE_COVERAGE(193); // Hit
  return (void*)(bucket + 1);
}

/** The used heap size, excluding spare capacity in the last block, but
 * including any uncollected garbage. */
static uint16_t getHeapSize(VM* vm) {
  TsBucket* lastBucket = vm->pLastBucket;
  if (lastBucket) {
    CODE_COVERAGE(194); // Hit
    return getBucketOffsetEnd(lastBucket);
  } else {
    CODE_COVERAGE(195); // Hit
    return 0;
  }
}

void mvm_getMemoryStats(VM* vm, mvm_TsMemoryStats* r) {
  CODE_COVERAGE(627); // Hit
  VM_ASSERT(NULL, vm != NULL);
  VM_ASSERT(vm, r != NULL);

  memset(r, 0, sizeof *r);

  // Core size
  r->coreSize = sizeof(VM);
  r->fragmentCount++;

  // Import table size
  r->importTableSize = getSectionSize(vm, BCS_IMPORT_TABLE) / sizeof (vm_TsImportTableEntry) * sizeof(mvm_TfHostFunction);

  // Global variables size
  r->globalVariablesSize = getSectionSize(vm, BCS_IMPORT_TABLE);

  r->stackHighWaterMark = vm->stackHighWaterMark;

  r->virtualHeapHighWaterMark = vm->heapHighWaterMark;

  // Running Parameters
  vm_TsStack* stack = vm->stack;
  if (stack) {
    CODE_COVERAGE(628); // Hit
    r->fragmentCount++;
    vm_TsRegisters* reg = &stack->reg;
    r->registersSize = sizeof *reg;
    r->stackHeight = (uint8_t*)reg->pStackPointer - (uint8_t*)getBottomOfStack(vm->stack);
    r->stackAllocatedCapacity = MVM_STACK_SIZE;
  }

  // Heap Stats
  TsBucket* pLastBucket = vm->pLastBucket;
  size_t heapOverheadSize = 0;
  if (pLastBucket) {
    CODE_COVERAGE(629); // Hit
    TsBucket* b;
    for (b = pLastBucket; b; b = b->prev) {
      r->fragmentCount++;
      heapOverheadSize += sizeof (TsBucket); // Extra space for bucket header
    }
    r->virtualHeapUsed = getHeapSize(vm);
    if (r->virtualHeapUsed > r->virtualHeapHighWaterMark)
      r->virtualHeapHighWaterMark = r->virtualHeapUsed;
    r->virtualHeapAllocatedCapacity = pLastBucket->offsetStart + (uint16_t)(uintptr_t)vm->pLastBucketEndCapacity - (uint16_t)(uintptr_t)getBucketDataBegin(pLastBucket);
  }

  // Total size
  r->totalSize =
    r->coreSize +
    r->importTableSize +
    r->globalVariablesSize +
    r->registersSize +
    r->stackAllocatedCapacity +
    r->virtualHeapAllocatedCapacity +
    heapOverheadSize;
}

/**
 * Expand the VM heap by allocating a new "bucket" of memory from the host.
 *
 * @param bucketSize The ideal size of the contents of the new bucket
 * @param minBucketSize The smallest the bucketSize can be reduced and still be valid
 */
static void gc_createNextBucket(VM* vm, uint16_t bucketSize, uint16_t minBucketSize) {
  CODE_COVERAGE(7); // Hit
  uint16_t heapSize = getHeapSize(vm);

  if (bucketSize < minBucketSize) {
    CODE_COVERAGE_UNTESTED(196); // Not hit
    bucketSize = minBucketSize;
  }

  VM_ASSERT(vm, minBucketSize <= bucketSize);

  // If this tips us over the top of the heap, then we run a collection
  if (heapSize + bucketSize > MVM_MAX_HEAP_SIZE) {
    CODE_COVERAGE_UNTESTED(197); // Hit
    mvm_runGC(vm, false);
    heapSize = getHeapSize(vm);
  }

  // Can't fit?
  if (heapSize + minBucketSize > MVM_MAX_HEAP_SIZE) {
    CODE_COVERAGE_ERROR_PATH(5); // Not hit
    MVM_FATAL_ERROR(vm, MVM_E_OUT_OF_MEMORY);
  }

  // Can fit, but only by chopping the end off the new bucket?
  if (heapSize + bucketSize > MVM_MAX_HEAP_SIZE) {
    CODE_COVERAGE_UNTESTED(6); // Not hit
    bucketSize = MVM_MAX_HEAP_SIZE - heapSize;
  }

  size_t allocSize = sizeof (TsBucket) + bucketSize;
  TsBucket* bucket = vm_malloc(vm, allocSize);
  if (!bucket) {
    CODE_COVERAGE_ERROR_PATH(198); // Not hit
    MVM_FATAL_ERROR(vm, MVM_E_MALLOC_FAIL);
  }
  #if MVM_SAFE_MODE
    memset(bucket, 0x7E, allocSize);
  #endif
  bucket->prev = vm->pLastBucket;
  bucket->next = NULL;
  bucket->pEndOfUsedSpace = getBucketDataBegin(bucket);

  TABLE_COVERAGE(bucket->prev ? 1 : 0, 2, 11); // Hit 2/2

  // Note: we start the next bucket at the allocation cursor, not at what we
  // previously called the end of the previous bucket
  bucket->offsetStart = heapSize;
  vm->pLastBucketEndCapacity = (uint16_t*)((intptr_t)bucket->pEndOfUsedSpace + bucketSize);
  if (vm->pLastBucket) {
    CODE_COVERAGE(199); // Hit
    vm->pLastBucket->next = bucket;
  } else {
    CODE_COVERAGE(200); // Hit
  }
  vm->pLastBucket = bucket;
}

static void gc_freeGCMemory(VM* vm) {
  CODE_COVERAGE(10); // Hit
  TABLE_COVERAGE(vm->pLastBucket ? 1 : 0, 2, 201); // Hit 2/2
  while (vm->pLastBucket) {
    CODE_COVERAGE(169); // Hit
    TsBucket* prev = vm->pLastBucket->prev;
    vm_free(vm, vm->pLastBucket);
    TABLE_COVERAGE(prev ? 1 : 0, 2, 202); // Hit 1/2
    vm->pLastBucket = prev;
  }
  vm->pLastBucketEndCapacity = NULL;
}

#if MVM_INCLUDE_SNAPSHOT_CAPABILITY || (!MVM_NATIVE_POINTER_IS_16_BIT && !MVM_USE_SINGLE_RAM_PAGE)
/**
 * Given a pointer `ptr` into the heap, this returns the equivalent offset from
 * the start of the heap (0 meaning that `ptr` points to the beginning of the
 * heap).
 *
 * This is used in 2 places:
 *
 *   1. On a 32-bit machine, this is used to get a 16-bit equivalent encoding for ShortPtr
 *   2. On any machine, this is used in serializePtr for creating snapshots
 */
static uint16_t pointerOffsetInHeap(VM* vm, TsBucket* pLastBucket, void* ptr) {
  CODE_COVERAGE(203); // Hit
  /*
   * This algorithm iterates through the buckets in the heap backwards. Although
   * this is technically linear cost, in reality I expect that the pointer will
   * be found in the very first searched bucket almost all the time. This is
   * because the GC compacts everything into a single bucket, and because the
   * most recently bucket is also likely to be the most frequently accessed.
   *
   * See ShortPtr_decode for more description
   */
  TsBucket* bucket = pLastBucket;
  while (bucket) {
    // Note: using `<=` here because the pointer is permitted to point to the
    // end of the heap.
    if ((ptr >= (void*)bucket) && (ptr <= (void*)bucket->pEndOfUsedSpace)) {
      CODE_COVERAGE(204); // Hit
      uint16_t offsetInBucket = (uint16_t)((intptr_t)ptr - (intptr_t)getBucketDataBegin(bucket));
      VM_ASSERT(vm, offsetInBucket < 0x8000);
      uint16_t offsetInHeap = bucket->offsetStart + offsetInBucket;

      // It isn't strictly necessary that all short pointers are 2-byte aligned,
      // but it probably indicates a mistake somewhere if a short pointer is not
      // 2-byte aligned, since `Value` cannot be a `ShortPtr` unless it's 2-byte
      // aligned.
      VM_ASSERT(vm, (offsetInHeap & 1) == 0);

      VM_ASSERT(vm, offsetInHeap < getHeapSize(vm));

      return offsetInHeap;
    } else {
      CODE_COVERAGE(205); // Hit
    }

    bucket = bucket->prev;
  }

  // A failure here means we're trying to encode a pointer that doesn't map
  // to something in GC memory, which is a mistake.
  MVM_FATAL_ERROR(vm, MVM_E_UNEXPECTED);
  return 0;
}
#endif // MVM_INCLUDE_SNAPSHOT_CAPABILITY || (!MVM_NATIVE_POINTER_IS_16_BIT && !MVM_USE_SINGLE_RAM_PAGE)

// Encodes a bytecode offset as a Value
static inline Value vm_encodeBytecodeOffsetAsPointer(VM* vm, uint16_t offset) {
  // Only offsets with 4-byte alignment can be represented as VM values
  VM_ASSERT(vm, offset & 0xFFFC);
  // Bytecode pointers end in binary 01
  return offset | 1;
}

#if MVM_NATIVE_POINTER_IS_16_BIT
  static inline void* ShortPtr_decode(VM* vm, ShortPtr ptr) {
    return (void*)ptr;
  }
  static inline ShortPtr ShortPtr_encode(VM* vm, void* ptr) {
    return (ShortPtr)ptr;
  }
  static inline ShortPtr ShortPtr_encodeInToSpace(gc_TsGCCollectionState* gc, void* ptr) {
    return (ShortPtr)ptr;
  }
#elif MVM_USE_SINGLE_RAM_PAGE
  static inline void* ShortPtr_decode(VM* vm, ShortPtr ptr) {
    /**
     * Minor performance note:
     *
     * I think I recall that the ARM instruction set can inline 16-bit literal
     * values but not 32-bit values. This is one of the reasons why this uses
     * the "high bits" and not just some arbitrary pointer addition. Basically,
     * I'm trying to make this as efficient as possible, since pointers are used
     * everywhere
     */
    return (void*)(((intptr_t)MVM_RAM_PAGE_ADDR) | ptr);
  }
  static inline ShortPtr ShortPtr_encode(VM* vm, void* ptr) {
    VM_ASSERT(vm, ((intptr_t)ptr - (intptr_t)MVM_RAM_PAGE_ADDR) <= 0xFFFF);
    return (ShortPtr)(uintptr_t)ptr;
  }
  static inline ShortPtr ShortPtr_encodeInToSpace(gc_TsGCCollectionState* gc, void* ptr) {
    VM_ASSERT(gc->vm, ((intptr_t)ptr - (intptr_t)MVM_RAM_PAGE_ADDR) <= 0xFFFF);
    return (ShortPtr)(uintptr_t)ptr;
  }
#else // !MVM_NATIVE_POINTER_IS_16_BIT && !MVM_USE_SINGLE_RAM_PAGE
  static void* ShortPtr_decode(VM* vm, ShortPtr shortPtr) {
    // It isn't strictly necessary that all short pointers are 2-byte aligned,
    // but it probably indicates a mistake somewhere if a short pointer is not
    // 2-byte aligned, since `Value` cannot be a `ShortPtr` unless it's 2-byte
    // aligned. Among other things, this catches VM_VALUE_NULL.
    VM_ASSERT(vm, (shortPtr & 1) == 0);

    // The shortPtr is treated as an offset into the heap
    uint16_t offsetInHeap = shortPtr;
    VM_ASSERT(vm, offsetInHeap < getHeapSize(vm));

    /*
    Note: this is a linear search through the buckets, but a redeeming factor is
    that GC compacts the heap into a single bucket, so the number of buckets is
    small at any one time. Also, most-recently-allocated data are likely to be
    in the last bucket and accessed fastest. Also, the representation of the
    function is only needed on more powerful platforms. For 16-bit platforms,
    the implementation of ShortPtr_decode is a no-op.
    */

    TsBucket* bucket = vm->pLastBucket;
    while (true) {
      // All short pointers must map to some memory in a bucket, otherwise the pointer is corrupt
      VM_ASSERT(vm, bucket != NULL);

      if (offsetInHeap >= bucket->offsetStart) {
        uint16_t offsetInBucket = offsetInHeap - bucket->offsetStart;
        void* result = (void*)((intptr_t)getBucketDataBegin(bucket) + offsetInBucket);
        return result;
      }
      bucket = bucket->prev;
    }
  }

  /**
   * Like ShortPtr_encode except conducted against an arbitrary bucket list.
   *
   * Used internally by ShortPtr_encode and ShortPtr_encodeInToSpace.
   */
  static inline ShortPtr ShortPtr_encode_generic(VM* vm, TsBucket* pLastBucket, void* ptr) {
    return pointerOffsetInHeap(vm, pLastBucket, ptr);
  }

  // Encodes a pointer as pointing to a value in the current heap
  static inline ShortPtr ShortPtr_encode(VM* vm, void* ptr) {
    return ShortPtr_encode_generic(vm, vm->pLastBucket, ptr);
  }

  // Encodes a pointer as pointing to a value in the _new_ heap (tospace) during
  // an ongoing garbage collection.
  static inline ShortPtr ShortPtr_encodeInToSpace(gc_TsGCCollectionState* gc, void* ptr) {
    return ShortPtr_encode_generic(gc->vm, gc->lastBucket, ptr);
  }
#endif

static LongPtr BytecodeMappedPtr_decode_long(VM* vm, BytecodeMappedPtr ptr) {
  CODE_COVERAGE(214); // Hit

  // BytecodeMappedPtr values are treated as offsets into a bytecode image if
  // you zero the lowest 2 bits
  uint16_t offsetInBytecode = ptr & 0xFFFC;

  LongPtr lpBytecode = vm->lpBytecode;

  // A BytecodeMappedPtr can either point to ROM or via a global variable to
  // RAM. Here to discriminate the two, we're assuming the handles section comes
  // first
  VM_ASSERT(vm, BCS_ROM < BCS_GLOBALS);
  uint16_t globalsOffset = getSectionOffset(lpBytecode, BCS_GLOBALS);

  if (offsetInBytecode < globalsOffset) { // Points to ROM section?
    CODE_COVERAGE(215); // Hit
    VM_ASSERT(vm, offsetInBytecode >= getSectionOffset(lpBytecode, BCS_ROM));
    VM_ASSERT(vm, offsetInBytecode < getSectionOffset(lpBytecode, vm_sectionAfter(vm, BCS_ROM)));
    VM_ASSERT(vm, (offsetInBytecode & 3) == 0);

    // The pointer just references ROM
    return LongPtr_add(lpBytecode, offsetInBytecode);
  } else { // Else, must point to RAM via a global variable
    CODE_COVERAGE(216); // Hit
    VM_ASSERT(vm, offsetInBytecode >= getSectionOffset(lpBytecode, BCS_GLOBALS));
    VM_ASSERT(vm, offsetInBytecode < getSectionOffset(lpBytecode, vm_sectionAfter(vm, BCS_GLOBALS)));
    VM_ASSERT(vm, (offsetInBytecode & 3) == 0);

    uint16_t offsetInGlobals = offsetInBytecode - globalsOffset;
    Value handleValue = *(Value*)((intptr_t)vm->globals + offsetInGlobals);

    // Note: handle values can't be null, because handles are used to point from
    // ROM to RAM and ROM will never change. So if the value in ROM was null
    // then it will always be null and not need a handle. And if the value in
    // ROM points to an allocation in RAM then that allocation is permanently
    // reachable.
    VM_ASSERT(vm, Value_isShortPtr(handleValue));

    return LongPtr_new(ShortPtr_decode(vm, handleValue));
  }
}

static LongPtr DynamicPtr_decode_long(VM* vm, DynamicPtr ptr) {
  CODE_COVERAGE(217); // Hit

  if (Value_isShortPtr(ptr))  {
    CODE_COVERAGE(218); // Hit
    return LongPtr_new(ShortPtr_decode(vm, ptr));
  }

  if (ptr == VM_VALUE_NULL || ptr == VM_VALUE_UNDEFINED) {
    CODE_COVERAGE(219); // Hit
    return LongPtr_new(NULL);
  }
  CODE_COVERAGE(242); // Hit

  // This function is for decoding pointers, so if this isn't a pointer then
  // there's a problem.
  VM_ASSERT(vm, !Value_isVirtualInt14(ptr));

  // At this point, it's not a short pointer, so it must be a bytecode-mapped
  // pointer
  VM_ASSERT(vm, Value_encodesBytecodeMappedPtr(ptr));

  // I'm expecting this to be inlined by the compiler
  return BytecodeMappedPtr_decode_long(vm, ptr);
}

/*
 * Decode a DynamicPtr when the target is known to live in natively-addressable
 * memory (i.e. heap memory). If the target might be in ROM, use
 * DynamicPtr_decode_long.
 */
static void* DynamicPtr_decode_native(VM* vm, DynamicPtr ptr) {
  CODE_COVERAGE(253); // Hit
  LongPtr lp = DynamicPtr_decode_long(vm, ptr);
  void* p = LongPtr_truncate(vm, lp);
  // Assert that the resulting native pointer is equivalent to the long pointer.
  // I.e. that we didn't lose anything in the truncation (i.e. that it doesn't
  // point to ROM).
  VM_ASSERT(vm, LongPtr_new(p) == lp);
  return p;
}

// I'm using inline wrappers around the port macros because I want to add a
// layer of type safety.
static inline LongPtr LongPtr_new(void* p) {
  CODE_COVERAGE(284); // Hit
  return MVM_LONG_PTR_NEW(p);
}
static inline void* LongPtr_truncate(VM* vm, LongPtr lp) {
  CODE_COVERAGE(332); // Hit
  void* result = MVM_LONG_PTR_TRUNCATE(lp);
  VM_ASSERT(vm, lp == LongPtr_new(result));
  return result;
}
static inline LongPtr LongPtr_add(LongPtr lp, int16_t offset) {
  CODE_COVERAGE(333); // Hit
  return MVM_LONG_PTR_ADD(lp, offset);
}
static inline int16_t LongPtr_sub(LongPtr lp1, LongPtr lp2) {
  CODE_COVERAGE(334); // Hit
  return (int16_t)(MVM_LONG_PTR_SUB(lp1, lp2));
}
static inline uint8_t LongPtr_read1(LongPtr lp) {
  CODE_COVERAGE(335); // Hit
  return (uint8_t)(MVM_READ_LONG_PTR_1(lp));
}
// Read a 16-bit value from a long pointer, if the target is 16-bit aligned
static inline uint16_t LongPtr_read2_aligned(LongPtr lp) {
  CODE_COVERAGE(336); // Hit
  // Expect an even boundary. Weird things happen on some platforms if you try
  // to read unaligned memory through aligned instructions.
  VM_ASSERT(0, ((uint16_t)(uintptr_t)lp & 1) == 0);
  return (uint16_t)(MVM_READ_LONG_PTR_2(lp));
}
// Read a 16-bit value from a long pointer, if the target is not 16-bit aligned
static inline uint16_t LongPtr_read2_unaligned(LongPtr lp) {
  CODE_COVERAGE(626); // Hit
  return (uint32_t)(MVM_READ_LONG_PTR_1(lp)) |
    ((uint32_t)(MVM_READ_LONG_PTR_1((MVM_LONG_PTR_ADD(lp, 1)))) << 8);
}
static inline uint32_t LongPtr_read4(LongPtr lp) {
  // We don't often read 4 bytes, since the word size for microvium is 2 bytes.
  // When we do need to, I think it's safer to just read it as 2 separate words
  // since we don't know for sure that we're not executing on a 32 bit machine
  // that can't do unaligned access. All memory in microvium is at least 16-bit
  // aligned, with the exception of bytecode instructions, but those do not
  // contain 32-bit literals.
  CODE_COVERAGE(337); // Hit
  return (uint32_t)(MVM_READ_LONG_PTR_2(lp)) |
    ((uint32_t)(MVM_READ_LONG_PTR_2((MVM_LONG_PTR_ADD(lp, 2)))) << 16);
}

static uint16_t getBucketOffsetEnd(TsBucket* bucket) {
  CODE_COVERAGE(338); // Hit
  return bucket->offsetStart + (uint16_t)(uintptr_t)bucket->pEndOfUsedSpace - (uint16_t)(uintptr_t)getBucketDataBegin(bucket);
}

static uint16_t gc_getHeapSize(gc_TsGCCollectionState* gc) {
  CODE_COVERAGE(351); // Hit
  TsBucket* pLastBucket = gc->lastBucket;
  if (pLastBucket) {
    CODE_COVERAGE(352); // Hit
    return getBucketOffsetEnd(pLastBucket);
  } else {
    CODE_COVERAGE(355); // Hit
    return 0;
  }
}

static void gc_newBucket(gc_TsGCCollectionState* gc, uint16_t newSpaceSize, uint16_t minNewSpaceSize) {
  CODE_COVERAGE(356); // Hit
  uint16_t heapSize = gc_getHeapSize(gc);

  if (newSpaceSize < minNewSpaceSize) {
    CODE_COVERAGE_UNTESTED(357); // Not hit
    newSpaceSize = minNewSpaceSize;
  } else {
    CODE_COVERAGE(358); // Hit
  }

  // Since this is during a GC, it should be impossible for us to need more heap
  // than is allowed, since the original heap should never have exceeded the
  // MVM_MAX_HEAP_SIZE.
  VM_ASSERT(NULL, heapSize + minNewSpaceSize <= MVM_MAX_HEAP_SIZE);

  // Can fit, but only by chopping the end off the new bucket?
  if (heapSize + newSpaceSize > MVM_MAX_HEAP_SIZE) {
    CODE_COVERAGE_UNTESTED(8); // Not hit
    newSpaceSize = MVM_MAX_HEAP_SIZE - heapSize;
  } else {
    CODE_COVERAGE(360); // Hit
  }

  TsBucket* pBucket = (TsBucket*)vm_malloc(gc->vm, sizeof (TsBucket) + newSpaceSize);
  if (!pBucket) {
    CODE_COVERAGE_ERROR_PATH(376); // Not hit
    MVM_FATAL_ERROR(NULL, MVM_E_MALLOC_FAIL);
    return;
  }
  pBucket->next = NULL;
  uint16_t* pDataInBucket = (uint16_t*)(pBucket + 1);
  if (((intptr_t)pDataInBucket) & 1) {
    CODE_COVERAGE_ERROR_PATH(377); // Not hit
    MVM_FATAL_ERROR(NULL, MVM_E_MALLOC_MUST_RETURN_POINTER_TO_EVEN_BOUNDARY);
    return;
  }
  pBucket->offsetStart = heapSize;
  pBucket->prev = gc->lastBucket;
  pBucket->pEndOfUsedSpace = getBucketDataBegin(pBucket);
  if (!gc->firstBucket) {
    CODE_COVERAGE(392); // Hit
    gc->firstBucket = pBucket;
  } else {
    CODE_COVERAGE(393); // Hit
  }
  if (gc->lastBucket) {
    CODE_COVERAGE(394); // Hit
    gc->lastBucket->next = pBucket;
  } else {
    CODE_COVERAGE(395); // Hit
  }
  gc->lastBucket = pBucket;
  gc->lastBucketEndCapacity = (uint16_t*)((intptr_t)pDataInBucket + newSpaceSize);
}

static void gc_processShortPtrValue(gc_TsGCCollectionState* gc, Value* pValue) {
  CODE_COVERAGE(407); // Hit

  uint16_t* writePtr;
  const Value spSrc = *pValue;
  VM* const vm = gc->vm;

  uint16_t* const pSrc = (uint16_t*)ShortPtr_decode(vm, spSrc);
  // ShortPtr is defined as not encoding null
  VM_ASSERT(vm, pSrc != NULL);

  const uint16_t headerWord = pSrc[-1];

  // If there's a tombstone, then we've already collected this allocation
  if (headerWord == TOMBSTONE_HEADER) {
    CODE_COVERAGE(464); // Hit
    *pValue = pSrc[0];
    return;
  } else {
    CODE_COVERAGE(465); // Hit
    // Tombstones always have exactly the header `TOMBSTONE_HEADER`. If it has
    // the type-code of TC_REF_TOMBSTONE but is not a tombstone then it's likely
    // a corrupt header or pValue does not point to a legitimate allocation.
    VM_ASSERT(vm, vm_getTypeCodeFromHeaderWord(headerWord) != TC_REF_TOMBSTONE);
  }
  // Otherwise, we need to move the allocation

SUB_MOVE_ALLOCATION:
  // Note: the variables before this point are `const` because an allocation
  // movement can be aborted half way and tried again (in particular, see the
  // property list compaction). It's only right at the end of this function
  // where the writePtr is "committed" to the gc structure.

  VM_ASSERT(vm, gc->lastBucket != NULL);
  writePtr = gc->lastBucket->pEndOfUsedSpace;
  uint16_t size = vm_getAllocationSizeExcludingHeaderFromHeaderWord(headerWord);
  uint16_t words = (size + 3) / 2; // Rounded up, including header

  // Check we have space
  if (writePtr + words > gc->lastBucketEndCapacity) {
    CODE_COVERAGE(466); // Hit
    uint16_t minRequiredSpace = words * 2;
    gc_newBucket(gc, MVM_ALLOCATION_BUCKET_SIZE, minRequiredSpace);

    goto SUB_MOVE_ALLOCATION;
  } else {
    CODE_COVERAGE(467); // Hit
  }

  // Write the header
  *writePtr++ = headerWord;
  words--;

  uint16_t* pOld = pSrc;
  uint16_t* pNew = writePtr;

  // Copy the allocation body
  uint16_t* readPtr = pSrc;
  while (words--)
    *writePtr++ = *readPtr++;

  // Dynamic arrays and property lists are compacted here
  TeTypeCode tc = vm_getTypeCodeFromHeaderWord(headerWord);
  if (tc == TC_REF_ARRAY) {
    CODE_COVERAGE(468); // Hit
    TsArray* arr = (TsArray*)pNew;
    DynamicPtr dpData = arr->dpData;
    if (dpData != VM_VALUE_NULL) {
      CODE_COVERAGE(469); // Hit
      VM_ASSERT(vm, Value_isShortPtr(dpData));

      // Note: this decodes the pointer against fromspace
      TsFixedLengthArray* pData = ShortPtr_decode(vm, dpData);

      uint16_t len = VirtualInt14_decode(vm, arr->viLength);
      #if MVM_SAFE_MODE
        uint16_t headerWord = readAllocationHeaderWord(pData);
        uint16_t dataTC = vm_getTypeCodeFromHeaderWord(headerWord);
        // Note: because dpData is a unique pointer, we can be sure that it
        // hasn't already been moved in response to some other reference to
        // it (it's not a tombstone yet).
        VM_ASSERT(vm, dataTC == TC_REF_FIXED_LENGTH_ARRAY);
        uint16_t dataSize = vm_getAllocationSizeExcludingHeaderFromHeaderWord(headerWord);
        uint16_t capacity = dataSize / 2;
        VM_ASSERT(vm, len <= capacity);
      #endif

      if (len > 0) {
        CODE_COVERAGE(470); // Hit
        // We just truncate the fixed-length-array to match the programmed
        // length of the dynamic array, which is necessarily equal or less than
        // its previous value. The GC will copy the data later and update the
        // data pointer as it would normally do when following pointers.
        setHeaderWord(vm, pData, TC_REF_FIXED_LENGTH_ARRAY, len * 2);
      } else {
        CODE_COVERAGE_UNTESTED(472); // Not hit
        // Or if there's no length, we can remove the data altogether.
        arr->dpData = VM_VALUE_NULL;
      }
    } else {
      CODE_COVERAGE(473); // Hit
    }
  } else if (tc == TC_REF_PROPERTY_LIST) {
    CODE_COVERAGE(474); // Hit
    TsPropertyList* props = (TsPropertyList*)pNew;

    Value dpNext = props->dpNext;

    // If the object has children (detached extensions to the main
    // allocation), we take this opportunity to compact them into the parent
    // allocation to save space and improve access performance.
    if (dpNext != VM_VALUE_NULL) {
      CODE_COVERAGE(478); // Hit
      // Note: The "root" property list counts towards the total but its
      // fields do not need to be copied because it's already copied, above
      uint16_t headerWord = readAllocationHeaderWord(props);
      uint16_t allocationSize = vm_getAllocationSizeExcludingHeaderFromHeaderWord(headerWord);
      uint16_t totalPropCount = (allocationSize - sizeof(TsPropertyList)) / 4;

      do {
        // Note: while `next` is not strictly a ShortPtr in general, when used
        // within GC allocations it will never point to an allocation in ROM
        // or data memory, since it's only used to extend objects with new
        // properties.
        VM_ASSERT(vm, Value_isShortPtr(dpNext));
        TsPropertyList* child = (TsPropertyList*)ShortPtr_decode(vm, dpNext);

        uint16_t headerWord = readAllocationHeaderWord(child);
        uint16_t allocationSize = vm_getAllocationSizeExcludingHeaderFromHeaderWord(headerWord);
        uint16_t childPropCount = (allocationSize - sizeof(TsPropertyList)) / 4;
        totalPropCount += childPropCount;

        uint16_t* end = writePtr + childPropCount;
        // Check we have space for the new properties
        if (end > gc->lastBucketEndCapacity) {
          CODE_COVERAGE(479); // Hit
          // If we don't have space, we need to revert and try again. The
          // "revert" isn't explict. It depends on the fact that the gc.writePtr
          // hasn't been committed yet, and no mutations have been applied to
          // the source memory (i.e. the tombstone hasn't been written yet).
          uint16_t minRequiredSpace = sizeof (TsPropertyList) + totalPropCount * 4;
          gc_newBucket(gc, MVM_ALLOCATION_BUCKET_SIZE, minRequiredSpace);
          goto SUB_MOVE_ALLOCATION;
        } else {
          CODE_COVERAGE(480); // Hit
        }

        uint16_t* pField = (uint16_t*)(child + 1);

        // Copy the child fields directly into the parent
        while (childPropCount--) {
          *writePtr++ = *pField++; // key
          *writePtr++ = *pField++; // value
        }
        dpNext = child->dpNext;
        TABLE_COVERAGE(dpNext ? 1 : 0, 2, 490); // Hit 1/2
      } while (dpNext != VM_VALUE_NULL);

      // We've collapsed all the lists into one, so let's adjust the header
      uint16_t newSize = sizeof (TsPropertyList) + totalPropCount * 4;
      if (newSize > MAX_ALLOCATION_SIZE) {
        CODE_COVERAGE_ERROR_PATH(491); // Not hit
        MVM_FATAL_ERROR(vm, MVM_E_ALLOCATION_TOO_LARGE);
        return;
      }

      setHeaderWord(vm, props, TC_REF_PROPERTY_LIST, newSize);
      props->dpNext = VM_VALUE_NULL;
    }
  } else {
    CODE_COVERAGE(492); // Hit
  }

  // Commit the move (grow the target heap and add the tombstone)

  gc->lastBucket->pEndOfUsedSpace = writePtr;

  ShortPtr spNew = ShortPtr_encodeInToSpace(gc, pNew);

  pOld[-1] = TOMBSTONE_HEADER;
  pOld[0] = spNew; // Forwarding pointer

  *pValue = spNew;
}

static inline void gc_processValue(gc_TsGCCollectionState* gc, Value* pValue) {
  // Note: only short pointer values are allowed to point to GC memory,
  // and we only need to follow references that go to GC memory.
  if (Value_isShortPtr(*pValue)) {
    CODE_COVERAGE(446); // Hit
    gc_processShortPtrValue(gc, pValue);
  } else {
    CODE_COVERAGE(463); // Hit
  }
}

void mvm_runGC(VM* vm, bool squeeze) {
  CODE_COVERAGE(593); // Hit

  /*
  This is a semispace collection model based on Cheney's algorithm
  https://en.wikipedia.org/wiki/Cheney%27s_algorithm. It collects by moving
  reachable allocations from the fromspace to the tospace and then releasing the
  fromspace. It starts by moving allocations reachable by the roots, and then
  iterates through moved allocations, checking the pointers therein, moving the
  allocations they reference.

  When an object is moved, the space it occupied is changed to a tombstone
  (TC_REF_TOMBSTONE) which contains a forwarding pointer. When a pointer in
  tospace is seen to point to an allocation in fromspace, if the fromspace
  allocation is a tombstone then the pointer can be updated to the forwarding
  pointer.

  This algorithm relies on allocations in tospace each have a header. Some
  allocations, such as property cells, don't have a header, but will only be
  found in fromspace. When copying objects into tospace, the detached property
  cells are merged into the object's head allocation.

  Note: all pointer _values_ are only processed once each (since their
  corresponding container is only processed once). This means that fromspace and
  tospace can be treated as distinct spaces. An unprocessed pointer is
  interpreted in terms of _fromspace_. Forwarding pointers and pointers in
  processed allocations always reference _tospace_.
  */

  #if MVM_VERY_EXPENSIVE_MEMORY_CHECKS
  mvm_checkHeap(vm);
  #endif

  uint16_t n;
  uint16_t* p;

  uint16_t heapSize = getHeapSize(vm);
  if (heapSize > vm->heapHighWaterMark)
    vm->heapHighWaterMark = heapSize;

  // A collection of variables shared by GC routines
  gc_TsGCCollectionState gc;
  memset(&gc, 0, sizeof gc);
  gc.vm = vm;

  // We don't know how big the heap needs to be, so we just allocate the same
  // amount of space as used last time and then expand as-needed
  uint16_t estimatedSize = vm->heapSizeUsedAfterLastGC;

  #if MVM_VERY_EXPENSIVE_MEMORY_CHECKS
    // Move the heap address space by 2 bytes on each cycle (overflows at 256).
    vm->gc_heap_shift += 2;
    if (vm->gc_heap_shift == 0) {
      // Minimum of 2 bytes just so we have consistency when it overflows
      vm->gc_heap_shift = 2;
    }
    // We shift up the address space by `gc_heap_shift` amount by just
    // allocating a bucket of that size at the beginning and marking it full.
    gc_newBucket(&gc, vm->gc_heap_shift, 0);
    // The heap must be parsable, so we need to have an allocation header to
    // mark the space. In general, we do not allow allocations to be smaller
    // than 4 bytes because a tombstone is 4 bytes. However, there can be no
    // references to this "allocation" so no tombstone is required, so it can
    // be as small as 2 bytes. I'm using a string here because it's a
    // "non-container" type, so the GC will not interpret its contents.
    VM_ASSERT(vm, vm->gc_heap_shift >= 2);
    *gc.lastBucket->pEndOfUsedSpace = vm_makeHeaderWord(vm, TC_REF_STRING, vm->gc_heap_shift - 2);
  #endif // MVM_VERY_EXPENSIVE_MEMORY_CHECKS

  if (!estimatedSize) {
    CODE_COVERAGE(494); // Hit
    // Actually the value-copying algorithm can't deal with creating the heap from nothing, and
    // I don't want to slow it down by adding extra checks, so we always create at least a small
    // heap.
    estimatedSize = 64;
  } else {
    CODE_COVERAGE(493); // Hit
  }
  gc_newBucket(&gc, estimatedSize, 0);

  // Roots in global variables (including indirection handles)
  // Note: Interned strings are referenced from a handle and so will be GC'd here
  // TODO: It would actually be good to have a test case showing that the string interning table is handled properly during GC
  uint16_t globalsSize = getSectionSize(vm, BCS_GLOBALS);
  p = vm->globals;
  n = globalsSize / 2;
  TABLE_COVERAGE(n ? 1 : 0, 2, 495); // Hit 1/2
  while (n--)
    gc_processValue(&gc, p++);

  // Roots in gc_handles
  mvm_Handle* handle = vm->gc_handles;
  TABLE_COVERAGE(handle ? 1 : 0, 2, 496); // Hit 2/2
  while (handle) {
    gc_processValue(&gc, &handle->_value);
    TABLE_COVERAGE(handle->_next ? 1 : 0, 2, 497); // Hit 2/2
    handle = handle->_next;
  }

  // Roots on the stack or registers
  vm_TsStack* stack = vm->stack;
  if (stack) {
    CODE_COVERAGE(498); // Hit
    vm_TsRegisters* reg = &stack->reg;
    VM_ASSERT(vm, reg->usingCachedRegisters == false);

    // Roots in registers
    gc_processValue(&gc, &reg->closure);
    gc_processValue(&gc, &reg->cpsCallback);
    gc_processValue(&gc, &reg->jobQueue);

    // Roots on call stack
    uint16_t* beginningOfStack = getBottomOfStack(stack);
    uint16_t* beginningOfFrame = reg->pFrameBase;
    uint16_t* endOfFrame = reg->pStackPointer;

    while (true) {
      VM_ASSERT(vm, beginningOfFrame >= beginningOfStack);

      // Loop through words in frame
      p = beginningOfFrame;
      while (p != endOfFrame) {
        VM_ASSERT(vm, p < endOfFrame);
        // TODO: It would be an interesting exercise to see if the GC can be written into a single function so that we don't need to pass around the &gc struct everywhere
        gc_processValue(&gc, p++);
      }

      if (beginningOfFrame == beginningOfStack) {
        break;
      }
      VM_ASSERT(vm, beginningOfFrame >= beginningOfStack);

      // The following statements assume a particular stack shape
      VM_ASSERT(vm, VM_FRAME_BOUNDARY_VERSION == 2);

      // Skip over the registers that are saved during a CALL instruction
      endOfFrame = beginningOfFrame - 4;

      // The saved scope pointer
      Value* pScope = endOfFrame + 1;
      gc_processValue(&gc, pScope);

      // The first thing saved during a CALL is the size of the preceding frame
      beginningOfFrame = (uint16_t*)((uint8_t*)endOfFrame - *endOfFrame);

      TABLE_COVERAGE(beginningOfFrame == beginningOfStack ? 1 : 0, 2, 499); // Hit 2/2
    }
  } else {
    CODE_COVERAGE(500); // Hit
  }

  // Now we process moved allocations to make sure objects they point to are
  // also moved, and to update pointers to reference the new space

  TsBucket* bucket = gc.firstBucket;
  TABLE_COVERAGE(bucket ? 1 : 0, 2, 501); // Hit 1/2
  // Loop through buckets
  while (bucket) {
    uint16_t* p = (uint16_t*)getBucketDataBegin(bucket);

    // Loop through allocations in bucket. Note that this loop will hit exactly
    // the end of the bucket even when there are multiple buckets, because empty
    // space in a bucket is truncated when a new one is created (in
    // gc_processValue)
    while (p != bucket->pEndOfUsedSpace) { // Hot loop
      VM_ASSERT(vm, p < bucket->pEndOfUsedSpace);
      uint16_t header = *p++;
      uint16_t size = vm_getAllocationSizeExcludingHeaderFromHeaderWord(header);

      uint16_t* next = p + ((size + 1) >> 1); // round up

      // Note: we're comparing the header words here to compare the type code.
      // The RHS here is constant
      if (header < (uint16_t)(TC_REF_DIVIDER_CONTAINER_TYPES << 12)) { // Non-container types
        CODE_COVERAGE(502); // Hit
        p = next;
        continue;
      } else { // Else, container types
        CODE_COVERAGE(505); // Hit

        // Note: we round down in calculating the number of words in the container
        // that may contain a valid pointer. In particular this allows zero-length
        // containers to have a size of 1 byte, which is rounded up to a 2-byte
        // allocation (the minimum size large enough for the tombstone) but rounded
        // down to zer when treated as the container dimension.
        uint16_t words = size >> 1; // round down
        while (words--) { // Hot loop
          if (Value_isShortPtr(*p))
            gc_processValue(&gc, p);
          p++;
        }
        p = next;
      }
    }

    // Go to next bucket
    bucket = bucket->next;
    TABLE_COVERAGE(bucket ? 1 : 0, 2, 506); // Hit 2/2
  }

  // Release old heap
  TsBucket* oldBucket = vm->pLastBucket;
  TABLE_COVERAGE(oldBucket ? 1 : 0, 2, 507); // Hit 2/2
  while (oldBucket) {
    TsBucket* prev = oldBucket->prev;
    vm_free(vm, oldBucket);
    oldBucket = prev;
  }

  // Adopt new heap
  vm->pLastBucket = gc.lastBucket;
  vm->pLastBucketEndCapacity = gc.lastBucketEndCapacity;

  uint16_t finalUsedSize = getHeapSize(vm);
  vm->heapSizeUsedAfterLastGC = finalUsedSize;

  if (squeeze && (finalUsedSize != estimatedSize)) {
    CODE_COVERAGE(508); // Hit
    /*
    Note: The most efficient way to calculate the exact size needed for the heap
    is actually to run a collection twice. The collection algorithm itself is
    almost as efficient as any size-counting algorithm in terms of running time
    since it needs to iterate the whole reachability graph and all the pointers
    contained therein. But having a distinct size-counting algorithm is less
    efficient in terms of the amount of code-space (ROM) used, since it must
    duplicate much of the logic to parse the heap. It also needs to keep
    separate flags to know what it's already counted or not, and these flags
    would presumably take up space in the headers that isn't otherwise needed.

    Furthermore, it's suspected that a common case is where the VM is repeatedly
    used to perform the same calculation, such as a "tick" or "check" function,
    that does basically the same thing every time and so lands up in the same
    equilibrium size each time. With this squeeze implementation we would only
    run the GC once each time, since the estimated size would be correct most of
    the time.

    In conclusion, I decided that the best way to "squeeze" the heap is to just
    run the collection twice. The first time will tell us the exact size, and
    then if that's different to what we estimated then we perform the collection
    again, now with the exact target size, so that there is no unused space
    malloc'd from the host, and no unnecessary mallocs from the host.

    Note: especially for small programs, the squeeze could make a significant
    difference to the idle memory usage. A program that goes from 18 bytes to 20
    bytes will cause a whole new bucket to be allocated for the additional 2B,
    leaving 254B unused (if the bucket size is 256B). The "squeeze" pass will
    compact everything into a single 20B allocation.
    */
    mvm_runGC(vm, false);
  } else {
    CODE_COVERAGE(509); // Hit
  }
}

/**
 * Create the call VM call stack and registers
 */
TeError vm_createStackAndRegisters(VM* vm) {
  CODE_COVERAGE(225); // Hit
  // This is freed again at the end of mvm_call. Note: the allocated
  // memory includes the registers, which are part of the vm_TsStack
  // structure
  vm_TsStack* stack = vm_malloc(vm, sizeof (vm_TsStack) + MVM_STACK_SIZE);
  if (!stack) {
    CODE_COVERAGE_ERROR_PATH(231); // Not hit
    return vm_newError(vm, MVM_E_MALLOC_FAIL);
  }
  vm->stack = stack;
  vm_TsRegisters* reg = &stack->reg;
  memset(reg, 0, sizeof *reg);
  // The stack grows upward. The bottom is the lowest address.
  uint16_t* bottomOfStack = getBottomOfStack(stack);
  reg->pFrameBase = bottomOfStack;
  reg->pStackPointer = bottomOfStack;
  reg->lpProgramCounter = vm->lpBytecode; // This is essentially treated as a null value
  reg->argCountAndFlags = 0;
  reg->closure = VM_VALUE_UNDEFINED;
  reg->pCatchTarget = NULL;
  reg->cpsCallback = VM_VALUE_DELETED;
  reg->jobQueue = VM_VALUE_UNDEFINED;
  VM_ASSERT(vm, reg->pArgs == 0);

  return MVM_E_SUCCESS;
}

// Lowest address on stack
static inline uint16_t* getBottomOfStack(vm_TsStack* stack) {
  CODE_COVERAGE(510); // Hit
  return (uint16_t*)(stack + 1);
}

// Highest possible address on stack (+1) before overflow
static inline uint16_t* getTopOfStackSpace(vm_TsStack* stack) {
  CODE_COVERAGE(511); // Hit
  return getBottomOfStack(stack) + MVM_STACK_SIZE / 2;
}

#if MVM_DEBUG
// Some utility functions, mainly to execute in the debugger (could also be copy-pasted as expressions in some cases)
uint16_t dbgStackDepth(VM* vm) {
  return (uint16_t)((uint16_t*)vm->stack->reg.pStackPointer - (uint16_t*)(vm->stack + 1));
}
uint16_t* dbgStack(VM* vm) {
  return (uint16_t*)(vm->stack + 1);
}
uint16_t dbgPC(VM* vm) {
  return (uint16_t)((intptr_t)vm->stack->reg.lpProgramCounter - (intptr_t)vm->lpBytecode);
}
#endif // MVM_DEBUG

/**
 * Checks that we have enough stack space for the given size, and updates the
 * high water mark.
 */
static TeError vm_requireStackSpace(VM* vm, uint16_t* pStackPointer, uint16_t sizeRequiredInWords) {
  uint16_t* pStackHighWaterMark = pStackPointer + ((intptr_t)sizeRequiredInWords);
  if (pStackHighWaterMark > getTopOfStackSpace(vm->stack)) {
    CODE_COVERAGE_ERROR_PATH(233); // Not hit

    // TODO(low): Since we know the max stack depth for the function, we could
    // actually grow the stack dynamically rather than allocate it fixed size.
    // Actually, it seems likely that we could allocate the VM stack on the C
    // stack, since it's a fixed-size structure anyway.
    //
    // (A way to do the allocation on the stack would be to perform a nested
    // call to mvm_call, and the allocation can be at the beginning of
    // mvm_call). Otherwise we could just malloc, which has the advantage of
    // simplicity and we can grow the stack at any time.
    //
    // Rather than a segmented stack, it might also be simpler to just grow the
    // stack size and copy across old data. This has the advantage of keeping
    // the GC simple.
    return vm_newError(vm, MVM_E_STACK_OVERFLOW);
  }

  // Stack high-water mark
  uint16_t stackHighWaterMark = (uint16_t)((intptr_t)pStackHighWaterMark - (intptr_t)getBottomOfStack(vm->stack));
  if (stackHighWaterMark > vm->stackHighWaterMark) {
    vm->stackHighWaterMark = stackHighWaterMark;
  }

  return MVM_E_SUCCESS;
}

TeError vm_resolveExport(VM* vm, mvm_VMExportID id, Value* result) {
  CODE_COVERAGE(17); // Hit

  LongPtr exportTableEnd;
  LongPtr exportTable = getBytecodeSection(vm, BCS_EXPORT_TABLE, &exportTableEnd);

  // See vm_TsExportTableEntry
  LongPtr exportTableEntry = exportTable;
  while (exportTableEntry < exportTableEnd) {
    CODE_COVERAGE(234); // Hit
    mvm_VMExportID exportID = LongPtr_read2_aligned(exportTableEntry);
    if (exportID == id) {
      CODE_COVERAGE(235); // Hit
      LongPtr pExportValue = LongPtr_add(exportTableEntry, 2);
      mvm_VMExportID exportValue = LongPtr_read2_aligned(pExportValue);
      *result = exportValue;
      return MVM_E_SUCCESS;
    } else {
      CODE_COVERAGE(236); // Hit
    }
    exportTableEntry = LongPtr_add(exportTableEntry, sizeof (vm_TsExportTableEntry));
  }

  *result = VM_VALUE_UNDEFINED;
  return vm_newError(vm, MVM_E_UNRESOLVED_EXPORT);
}

TeError mvm_resolveExports(VM* vm, const mvm_VMExportID* idTable, Value* resultTable, uint8_t count) {
  CODE_COVERAGE(18); // Hit
  TeError err = MVM_E_SUCCESS;
  while (count--) {
    CODE_COVERAGE(237); // Hit
    TeError tempErr = vm_resolveExport(vm, *idTable++, resultTable++);
    if (tempErr != MVM_E_SUCCESS) {
      CODE_COVERAGE_ERROR_PATH(238); // Not hit
      err = tempErr;
    } else {
      CODE_COVERAGE(239); // Hit
    }
  }
  return err;
}

#if MVM_SAFE_MODE
static bool vm_isHandleInitialized(VM* vm, const mvm_Handle* handle) {
  CODE_COVERAGE(22); // Hit
  mvm_Handle* h = vm->gc_handles;
  while (h) {
    CODE_COVERAGE(243); // Hit
    if (h == handle) {
      CODE_COVERAGE_UNTESTED(244); // Not hit
      return true;
    }
    else {
      CODE_COVERAGE(245); // Hit
    }
    h = h->_next;
  }
  return false;
}
#endif // MVM_SAFE_MODE

void mvm_initializeHandle(VM* vm, mvm_Handle* handle) {
  CODE_COVERAGE(19); // Hit
  VM_ASSERT(vm, !vm_isHandleInitialized(vm, handle));
  handle->_next = vm->gc_handles;
  vm->gc_handles = handle;
  handle->_value = VM_VALUE_UNDEFINED;
}

void vm_cloneHandle(VM* vm, mvm_Handle* target, const mvm_Handle* source) {
  CODE_COVERAGE_UNTESTED(20); // Not hit
  VM_ASSERT(vm, !vm_isHandleInitialized(vm, source));
  mvm_initializeHandle(vm, target);
  target->_value = source->_value;
}

TeError mvm_releaseHandle(VM* vm, mvm_Handle* handle) {
  // This function doesn't contain coverage markers because node hits this path
  // non-deterministically.
  mvm_Handle** h = &vm->gc_handles;
  while (*h) {
    if (*h == handle) {
      *h = handle->_next;
      handle->_value = VM_VALUE_UNDEFINED;
      handle->_next = NULL;
      return MVM_E_SUCCESS;
    }
    h = &((*h)->_next);
  }
  handle->_value = VM_VALUE_UNDEFINED;
  handle->_next = NULL;
  return vm_newError(vm, MVM_E_INVALID_HANDLE);
}

#if MVM_SUPPORT_FLOAT
static Value vm_float64ToStr(VM* vm, Value value) {
  CODE_COVERAGE(619); // Hit

  VM_ASSERT_NOT_USING_CACHED_REGISTERS(vm); // Because we allocate a new string

  // I don't think this is 100% compliant, but it's probably fine for most use
  // cases, and most importantly it's small.

  double x = mvm_toFloat64(vm, value);

  char buf[64];
  char* p = buf;

  // NaN should be represented as VM_VALUE_NAN not a float with NaN
  VM_ASSERT(vm, !isnan(x));

  if (isinf(x)) {
    CODE_COVERAGE(621); // Hit
    if (x < 0) {
      CODE_COVERAGE(622); // Hit
      *p++ = '-';
    }
    memcpy(p, "Infinity", 9);
    p += 8;
  } else {
    CODE_COVERAGE(657); // Hit
    p += MVM_SNPRINTF(p, sizeof buf, "%.15g", x);
    VM_ASSERT(vm, p < buf + sizeof buf);
  }

  return mvm_newString(vm, buf, p - buf);
}
#endif //  MVM_SUPPORT_FLOAT

static Value vm_intToStr(VM* vm, int32_t i) {
  CODE_COVERAGE(618); // Hit
  VM_ASSERT_NOT_USING_CACHED_REGISTERS(vm);

  char buf[32];
  size_t size;

  size = MVM_SNPRINTF(buf, sizeof buf, "%" PRId32, i);
  VM_ASSERT(vm, size < sizeof buf);

  return mvm_newString(vm, buf, size);
}

static Value vm_convertToString(VM* vm, Value value) {
  CODE_COVERAGE(23); // Hit
  VM_ASSERT_NOT_USING_CACHED_REGISTERS(vm);

  TeTypeCode type = deepTypeOf(vm, value);
  const char* constStr;

  switch (type) {
    case TC_VAL_INT14:
    case TC_REF_INT32: {
      CODE_COVERAGE(246); // Hit
      int32_t i = vm_readInt32(vm, type, value);
      return vm_intToStr(vm, i);
    }
    case TC_REF_FLOAT64: {
      CODE_COVERAGE(248); // Not hit
      #if MVM_SUPPORT_FLOAT
      return vm_float64ToStr(vm, value);
      #else
      constStr = "";
      #endif
      break;
    }
    case TC_REF_STRING: {
      CODE_COVERAGE(249); // Hit
      return value;
    }
    case TC_REF_INTERNED_STRING: {
      CODE_COVERAGE(250); // Hit
      return value;
    }
    case TC_REF_PROPERTY_LIST: {
      CODE_COVERAGE_UNTESTED(251); // Not hit
      constStr = "[Object]";
      break;
    }
    case TC_REF_CLOSURE: {
      CODE_COVERAGE_UNTESTED(365); // Not hit
      constStr = "[Function]";
      break;
    }
    case TC_REF_FIXED_LENGTH_ARRAY:
    case TC_REF_ARRAY: {
      CODE_COVERAGE_UNTESTED(252); // Not hit
      constStr = "[Object]";
      break;
    }
    case TC_REF_FUNCTION: {
      CODE_COVERAGE_UNTESTED(254); // Not hit
      constStr = "[Function]";
      break;
    }
    case TC_REF_HOST_FUNC: {
      CODE_COVERAGE_UNTESTED(255); // Not hit
      constStr = "[Function]";
      break;
    }
    case TC_REF_UINT8_ARRAY: {
      CODE_COVERAGE_UNTESTED(256); // Not hit
      constStr = "[Object]";
      break;
    }
    case TC_REF_CLASS: {
      CODE_COVERAGE_UNTESTED(596); // Not hit
      constStr = "[Function]";
      break;
    }
    case TC_REF_VIRTUAL: {
      CODE_COVERAGE_UNTESTED(597); // Not hit
      VM_NOT_IMPLEMENTED(vm);
      return MVM_E_FATAL_ERROR_MUST_KILL_VM;
    }
    case TC_REF_SYMBOL: {
      CODE_COVERAGE_UNTESTED(257); // Not hit
      VM_NOT_IMPLEMENTED(vm);
      return MVM_E_FATAL_ERROR_MUST_KILL_VM;
    }
    case TC_VAL_UNDEFINED: {
      CODE_COVERAGE(258); // Hit
      constStr = "undefined";
      break;
    }
    case TC_VAL_NULL: {
      CODE_COVERAGE(259); // Hit
      constStr = "null";
      break;
    }
    case TC_VAL_TRUE: {
      CODE_COVERAGE(260); // Hit
      constStr = "true";
      break;
    }
    case TC_VAL_FALSE: {
      CODE_COVERAGE(261); // Hit
      constStr = "false";
      break;
    }
    case TC_VAL_NAN: {
      CODE_COVERAGE(262); // Not hit
      constStr = "NaN";
      break;
    }
    case TC_VAL_NEG_ZERO: {
      CODE_COVERAGE(263); // Hit
      constStr = "0";
      break;
    }
    case TC_VAL_STR_LENGTH: {
      CODE_COVERAGE(266); // Hit
      return value;
    }
    case TC_VAL_STR_PROTO: {
      CODE_COVERAGE_UNTESTED(267); // Not hit
      return value;
    }
    case TC_VAL_NO_OP_FUNC: {
      CODE_COVERAGE(654); // Hit
      constStr = "[Function]";
      break;
    }
    case TC_VAL_DELETED: {
      return VM_UNEXPECTED_INTERNAL_ERROR(vm);
    }
    default: return VM_UNEXPECTED_INTERNAL_ERROR(vm);
  }

  return vm_newStringFromCStrNT(vm, constStr);
}

static Value vm_concat(VM* vm, Value* left, Value* right) {
  CODE_COVERAGE(553); // Hit
  VM_ASSERT_NOT_USING_CACHED_REGISTERS(vm);

  uint16_t leftSize = vm_stringSizeUtf8(vm, *left);
  uint16_t rightSize = vm_stringSizeUtf8(vm, *right);

  uint8_t* data;
  // Note: this allocation can cause a GC collection which could cause the
  // strings to move in memory
  Value value = vm_allocString(vm, leftSize + rightSize, (void**)&data);

  LongPtr lpLeftStr = vm_getStringData(vm, *left);
  LongPtr lpRightStr = vm_getStringData(vm, *right);
  memcpy_long(data, lpLeftStr, leftSize);
  memcpy_long(data + leftSize, lpRightStr, rightSize);
  return value;
}

/* Returns the deep type code of the value, looking through pointers and boxing */
static TeTypeCode deepTypeOf(VM* vm, Value value) {
  CODE_COVERAGE(27); // Hit

  if (Value_isShortPtr(value)) {
    CODE_COVERAGE(0); // Hit
    void* p = ShortPtr_decode(vm, value);
    uint16_t headerWord = readAllocationHeaderWord(p);
    TeTypeCode typeCode = vm_getTypeCodeFromHeaderWord(headerWord);
    return typeCode;
  } else {
    CODE_COVERAGE(515); // Hit
  }

  if (Value_isVirtualInt14(value)) {
    CODE_COVERAGE(295); // Hit
    return TC_VAL_INT14;
  } else {
    CODE_COVERAGE(516); // Hit
  }

  VM_ASSERT(vm, Value_isBytecodeMappedPtrOrWellKnown(value));

  // Check for "well known" values such as TC_VAL_UNDEFINED
  if (value < VM_VALUE_WELLKNOWN_END) {
    CODE_COVERAGE(296); // Hit
    return (TeTypeCode)((value >> 2) + 0x11);
  } else {
    CODE_COVERAGE(297); // Hit
  }

  LongPtr p = DynamicPtr_decode_long(vm, value);
  uint16_t headerWord = readAllocationHeaderWord_long(p);
  TeTypeCode typeCode = vm_getTypeCodeFromHeaderWord(headerWord);

  return typeCode;
}

#if MVM_SUPPORT_FLOAT
int32_t mvm_float64ToInt32(MVM_FLOAT64 value) {
  CODE_COVERAGE(486); // Hit
  if (MVM_FLOAT_IS_FINITE(value)) {
    CODE_COVERAGE(487); // Hit
    return (int32_t)value;
  } else {
    CODE_COVERAGE(488); // Hit
    return 0;
  }
}

Value mvm_newNumber(VM* vm, MVM_FLOAT64 value) {
  CODE_COVERAGE(28); // Hit
  if (MVM_FLOAT_IS_NAN(value)) {
    CODE_COVERAGE(298); // Hit
    return VM_VALUE_NAN;
  } else {
    CODE_COVERAGE(517); // Hit
  }

  if (MVM_FLOAT_IS_NEG_ZERO(value)) {
    CODE_COVERAGE(299); // Hit
    return VM_VALUE_NEG_ZERO;
  } else {
    CODE_COVERAGE(518); // Hit
  }

  // Doubles are very expensive to compute, so at every opportunity, we'll check
  // if we can coerce back to an integer
  int32_t valueAsInt = mvm_float64ToInt32(value);
  if (value == (MVM_FLOAT64)valueAsInt) {
    CODE_COVERAGE(300); // Hit
    return mvm_newInt32(vm, valueAsInt);
  } else {
    CODE_COVERAGE(301); // Hit
  }

  MVM_FLOAT64* pResult = GC_ALLOCATE_TYPE(vm, MVM_FLOAT64, TC_REF_FLOAT64);
  *pResult = value;

  return ShortPtr_encode(vm, pResult);
}
#endif // MVM_SUPPORT_FLOAT

Value mvm_newInt32(VM* vm, int32_t value) {
  CODE_COVERAGE(29); // Hit
  if ((value >= VM_MIN_INT14) && (value <= VM_MAX_INT14)) {
    CODE_COVERAGE(302); // Hit
    return VirtualInt14_encode(vm, value);
  } else {
    CODE_COVERAGE(303); // Hit
  }

  // Int32

  int32_t* pResult = GC_ALLOCATE_TYPE(vm, int32_t, TC_REF_INT32);
  *pResult = value;

  return ShortPtr_encode(vm, pResult);
}

bool mvm_toBool(VM* vm, Value value) {
  CODE_COVERAGE(30); // Hit

  TeTypeCode type = deepTypeOf(vm, value);
  switch (type) {
    case TC_VAL_INT14: {
      CODE_COVERAGE(304); // Hit
      return value != VirtualInt14_encode(vm, 0);
    }
    case TC_REF_INT32: {
      CODE_COVERAGE_UNTESTED(305); // Not hit
      // Int32 can't be zero, otherwise it would be encoded as an int14
      VM_ASSERT(vm, vm_readInt32(vm, type, value) != 0);
      return false;
    }
    case TC_REF_FLOAT64: {
      CODE_COVERAGE_UNTESTED(306); // Not hit
      #if MVM_SUPPORT_FLOAT
        // Double can't be zero, otherwise it would be encoded as an int14
        VM_ASSERT(vm, mvm_toFloat64(vm, value) != 0);
      #endif
      return false;
    }
    case TC_REF_INTERNED_STRING:
    case TC_REF_STRING: {
      CODE_COVERAGE(307); // Hit
      return vm_stringSizeUtf8(vm, value) != 0;
    }
    case TC_REF_PROPERTY_LIST: {
      CODE_COVERAGE(308); // Hit
      return true;
    }
    case TC_REF_CLOSURE: {
      CODE_COVERAGE_UNTESTED(372); // Not hit
      return true;
    }
    case TC_REF_ARRAY: {
      CODE_COVERAGE(309); // Hit
      return true;
    }
    case TC_REF_FUNCTION: {
      CODE_COVERAGE_UNTESTED(311); // Not hit
      return true;
    }
    case TC_REF_HOST_FUNC: {
      CODE_COVERAGE_UNTESTED(312); // Not hit
      return true;
    }
    case TC_REF_UINT8_ARRAY: {
      CODE_COVERAGE_UNTESTED(313); // Not hit
      return true;
    }
    case TC_REF_SYMBOL: {
      CODE_COVERAGE_UNTESTED(314); // Not hit
      return true;
    }
    case TC_REF_CLASS: {
      CODE_COVERAGE(604); // Hit
      return true;
    }
    case TC_REF_VIRTUAL: {
      CODE_COVERAGE_UNTESTED(609); // Not hit
      VM_RESERVED(vm);
      return MVM_E_FATAL_ERROR_MUST_KILL_VM;

    }
    case TC_REF_RESERVED_1: {
      CODE_COVERAGE_UNTESTED(610); // Not hit
      VM_RESERVED(vm);
      return MVM_E_FATAL_ERROR_MUST_KILL_VM;

    }
    case TC_VAL_UNDEFINED: {
      CODE_COVERAGE(315); // Hit
      return false;
    }
    case TC_VAL_NULL: {
      CODE_COVERAGE(316); // Hit
      return false;
    }
    case TC_VAL_TRUE: {
      CODE_COVERAGE(317); // Hit
      return true;
    }
    case TC_VAL_FALSE: {
      CODE_COVERAGE(318); // Hit
      return false;
    }
    case TC_VAL_NAN: {
      CODE_COVERAGE_UNTESTED(319); // Not hit
      return false;
    }
    case TC_VAL_NEG_ZERO: {
      CODE_COVERAGE_UNTESTED(320); // Not hit
      return false;
    }
    case TC_VAL_DELETED: {
      CODE_COVERAGE_UNTESTED(321); // Not hit
      return false;
    }
    case TC_VAL_STR_LENGTH: {
      CODE_COVERAGE_UNTESTED(268); // Not hit
      return true;
    }
    case TC_VAL_STR_PROTO: {
      CODE_COVERAGE_UNTESTED(269); // Not hit
      return true;
    }
    case TC_VAL_NO_OP_FUNC: {
      CODE_COVERAGE_UNTESTED(655); // Not hit
      return true;
    }
    default: return VM_UNEXPECTED_INTERNAL_ERROR(vm);
  }
}

static bool vm_isString(VM* vm, Value value) {
  CODE_COVERAGE(31); // Hit
  return mvm_typeOf(vm, value) == VM_T_STRING;
}

/** Reads a numeric value that is a subset of a 32-bit integer */
static int32_t vm_readInt32(VM* vm, TeTypeCode type, Value value) {
  CODE_COVERAGE(33); // Hit
  if (type == TC_VAL_INT14) {
    CODE_COVERAGE(330); // Hit
    return VirtualInt14_decode(vm, value);
  } else if (type == TC_REF_INT32) {
    CODE_COVERAGE(331); // Hit
    LongPtr target = DynamicPtr_decode_long(vm, value);
    int32_t result = (int32_t)LongPtr_read4(target);
    return result;
  } else {
    return VM_UNEXPECTED_INTERNAL_ERROR(vm);
  }
}

static inline uint16_t readAllocationHeaderWord_long(LongPtr pAllocation) {
  CODE_COVERAGE(519); // Hit
  return LongPtr_read2_aligned(LongPtr_add(pAllocation, -2));
}

static inline uint16_t readAllocationHeaderWord(void* pAllocation) {
  CODE_COVERAGE(520); // Hit
  return ((uint16_t*)pAllocation)[-1];
}

static inline mvm_TfHostFunction* vm_getResolvedImports(VM* vm) {
  CODE_COVERAGE(40); // Hit
  return (mvm_TfHostFunction*)(vm + 1); // Starts right after the header
}

static inline mvm_HostFunctionID vm_getHostFunctionId(VM* vm, uint16_t hostFunctionIndex) {
  LongPtr lpImportTable = getBytecodeSection(vm, BCS_IMPORT_TABLE, NULL);
  LongPtr lpImportTableEntry = LongPtr_add(lpImportTable, hostFunctionIndex * sizeof (vm_TsImportTableEntry));
  return LongPtr_read2_aligned(lpImportTableEntry);
}

mvm_TeType mvm_typeOf(VM* vm, Value value) {
  TeTypeCode tc = deepTypeOf(vm, value);
  VM_ASSERT(vm, tc < sizeof typeByTC);
  TABLE_COVERAGE(tc, TC_END, 42); // Hit 17/27
  return (mvm_TeType)typeByTC[tc];
}

LongPtr vm_toStringUtf8_long(VM* vm, Value value, size_t* out_sizeBytes) {
  CODE_COVERAGE(43); // Hit
  VM_ASSERT_NOT_USING_CACHED_REGISTERS(vm);

  value = vm_convertToString(vm, value);

  TeTypeCode typeCode = deepTypeOf(vm, value);

  if (typeCode == TC_VAL_STR_PROTO) {
    CODE_COVERAGE_UNTESTED(521); // Not hit
    *out_sizeBytes = sizeof PROTO_STR - 1;
    return LongPtr_new((void*)&PROTO_STR);
  } else {
    CODE_COVERAGE(522); // Hit
  }

  if (typeCode == TC_VAL_STR_LENGTH) {
    CODE_COVERAGE_UNTESTED(523); // Not hit
    *out_sizeBytes = sizeof LENGTH_STR - 1;
    return LongPtr_new((void*)&LENGTH_STR);
  } else {
    CODE_COVERAGE(524); // Hit
  }

  VM_ASSERT(vm, (typeCode == TC_REF_STRING) || (typeCode == TC_REF_INTERNED_STRING));

  LongPtr lpTarget = DynamicPtr_decode_long(vm, value);
  uint16_t headerWord = readAllocationHeaderWord_long(lpTarget);
  uint16_t sourceSize = vm_getAllocationSizeExcludingHeaderFromHeaderWord(headerWord);

  if (out_sizeBytes) {
    CODE_COVERAGE(349); // Hit
    *out_sizeBytes = sourceSize - 1; // Without the extra safety null-terminator
  } else {
    CODE_COVERAGE_UNTESTED(350); // Not hit
  }

  return lpTarget;
}

/**
 * Gets a pointer to the string bytes of the string represented by `value`.
 *
 * `value` must be a string
 *
 * Warning: the result is a native pointer and becomes invalid if a GC
 * collection occurs.
 */
LongPtr vm_getStringData(VM* vm, Value value) {
  CODE_COVERAGE(228); // Hit
  TeTypeCode typeCode = deepTypeOf(vm, value);
  switch (typeCode) {
    case TC_VAL_STR_PROTO:
      CODE_COVERAGE_UNTESTED(229); // Not hit
      return LongPtr_new((void*)&PROTO_STR);
    case TC_VAL_STR_LENGTH:
      CODE_COVERAGE(512); // Hit
      return LongPtr_new((void*)&LENGTH_STR);
    case TC_REF_STRING:
    case TC_REF_INTERNED_STRING:
      return DynamicPtr_decode_long(vm, value);
    default:
      VM_ASSERT_UNREACHABLE(vm);
      return LongPtr_new(0);
  }
}

const char* mvm_toStringUtf8(VM* vm, Value value, size_t* out_sizeBytes) {
  CODE_COVERAGE(623); // Hit
  VM_ASSERT_NOT_USING_CACHED_REGISTERS(vm);
  /*
   * Note: I previously had this function returning a long pointer, but this
   * tripped someone up because they passed the result directly to printf, which
   * on MSP430 apparently doesn't support arbitrary long pointers (data20
   * pointers). Now I just copy it locally.
   */

  size_t size; // Size excluding a null terminator
  LongPtr lpTarget = vm_toStringUtf8_long(vm, value, &size);
  if (out_sizeBytes)
    *out_sizeBytes = size;

  void* pTarget = LongPtr_truncate(vm, lpTarget);
  // Is the string in RAM? (i.e. the truncated pointer is the same as the full pointer)
  if (LongPtr_new(pTarget) == lpTarget) {
    CODE_COVERAGE(624); // Hit
    return (const char*)pTarget;
  } else {
    CODE_COVERAGE_UNTESTED(625); // Not hit
    // Allocate a new string in local memory (with additional null terminator)
    vm_allocString(vm, size, &pTarget);
    memcpy_long(pTarget, lpTarget, size);

    return (const char*)pTarget;
  }
}

size_t mvm_stringSizeUtf8(mvm_VM* vm, mvm_Value value) {
  CODE_COVERAGE_UNTESTED(620); // Hit
  VM_ASSERT_NOT_USING_CACHED_REGISTERS(vm);
  size_t size;
  vm_toStringUtf8_long(vm, value, &size);
  return size;
}

Value mvm_newBoolean(bool source) {
  CODE_COVERAGE(44); // Hit
  return source ? VM_VALUE_TRUE : VM_VALUE_FALSE;
}

Value vm_allocString(VM* vm, size_t sizeBytes, void** out_pData) {
  CODE_COVERAGE(45); // Hit
  VM_ASSERT_NOT_USING_CACHED_REGISTERS(vm);
  if (sizeBytes < 3) {
    TABLE_COVERAGE(sizeBytes, 3, 525); // Hit 2/3
  }

  // Note: allocating 1 extra byte for the extra null terminator
  char* pData = mvm_allocate(vm, (uint16_t)sizeBytes + 1, TC_REF_STRING);
  *out_pData = pData;
  // Null terminator
  pData[sizeBytes] = '\0';
  return ShortPtr_encode(vm, pData);
}

// New string from null-terminated
static Value vm_newStringFromCStrNT(VM* vm, const char* s) {
  size_t len = strlen(s);
  return mvm_newString(vm, s, len);
}

Value mvm_newString(VM* vm, const char* sourceUtf8, size_t sizeBytes) {
  CODE_COVERAGE(46); // Hit
  VM_ASSERT_NOT_USING_CACHED_REGISTERS(vm);
  void* data;
  Value value = vm_allocString(vm, sizeBytes, &data);
  memcpy(data, sourceUtf8, sizeBytes);
  return value;
}

static Value getBuiltin(VM* vm, mvm_TeBuiltins builtinID) {
  CODE_COVERAGE(526); // Hit
  LongPtr lpBuiltins = getBytecodeSection(vm, BCS_BUILTINS, NULL);
  LongPtr lpBuiltin = LongPtr_add(lpBuiltins, (int16_t)(builtinID * sizeof (Value)));
  Value value = LongPtr_read2_aligned(lpBuiltin);

  // Check if the builtin accesses a RAM value via a handle
  Value* target = vm_getHandleTargetOrNull(vm, value);
  if (target) {
    CODE_COVERAGE(212); // Hit
    return *target;
  } else {
    CODE_COVERAGE(213); // Hit
    return value;
  }
}

/**
 * If the value is a handle, this returns a pointer to the global variable
 * referenced by the handle. Otherwise, this returns NULL.
 *
 * See also vm_resolveIndirections if you're only reading the value.
 */
static Value* vm_getHandleTargetOrNull(VM* vm, Value value) {
  CODE_COVERAGE(527); // Hit

  // Check low bits
  if (!Value_isBytecodeMappedPtrOrWellKnown(value)) {
    CODE_COVERAGE(528); // Hit
    return NULL;
  }

  if (value < VM_VALUE_WELLKNOWN_END) {
    CODE_COVERAGE(529); // Hit
    return NULL;
  }

  // See if it points earlier than the globals section, then it's pointing to a
  // ROM allocation, which is not a handle.
  uint16_t globalsOffset = getSectionOffset(vm->lpBytecode, BCS_GLOBALS);
  if (value < globalsOffset) {
    CODE_COVERAGE(530); // Hit
    VM_ASSERT(vm, value >= getSectionOffset(vm->lpBytecode, BCS_ROM));
    return NULL;
  }

  CODE_COVERAGE(531); // Hit

  // The globals section should be the last addressable section in the ROM, so a
  // pointer should not pointer later than the end of the globals section.
  VM_ASSERT(vm, value < getSectionOffset(vm->lpBytecode, vm_sectionAfter(vm, BCS_GLOBALS)));

  uint16_t globalIndex = (value - globalsOffset) / 2;
  return &vm->globals[globalIndex];
}

/**
 * If `value` points to a handle, this returns the value in the handle.
 *
 * See also `vm_getHandleTargetOrNull` if you want to write to the value.
 */
static Value vm_resolveIndirections(VM* vm, Value value) {
  CODE_COVERAGE(729); // Hit
  Value* target = vm_getHandleTargetOrNull(vm, value);
  if (!target) {
    CODE_COVERAGE(730); // Hit
    return value;
  }
  CODE_COVERAGE(731); // Hit
  return *target;
}


/**
 * Assigns to the slot pointed to by lpTarget
 *
 * If lpTarget points to a handle, then the corresponding global variable is
 * mutated. Otherwise, the target is directly mutated.
 *
 * This is used to synthesize mutation of slots in ROM, such as exports,
 * builtins, and properties of ROM objects. Such logically-mutable slots *must*
 * hold a value that is a BytecodeMappedPtr to a global variable that holds the
 * mutable reference.
 *
 * The function works transparently on RAM or ROM slots.
 */
// TODO: probably SetProperty should use this, so it works on ROM-allocated
// objects/arrays. Probably a good candidate for TDD.
static void setSlot_long(VM* vm, LongPtr lpSlot, Value value) {
  CODE_COVERAGE(532); // Hit
  Value slotContents = LongPtr_read2_aligned(lpSlot);
  // Work out if the target slot is actually a handle.
  Value* handleTarget = vm_getHandleTargetOrNull(vm, slotContents);
  if (handleTarget) {
    CODE_COVERAGE(533); // Hit
    // Set the corresponding global variable
    *handleTarget = value;
    return;
  } else {
    CODE_COVERAGE_UNTESTED(534); // Not hit
  }
  // Otherwise, for the mutation must be valid, the slot must be in RAM.

  // We never mutate through a long pointer, because anything mutable must be in
  // RAM and anything in RAM must be addressable by a short pointer
  Value* pSlot = LongPtr_truncate(vm, lpSlot);

  // Check the truncation hasn't lost anything. If this fails, the slot could be
  // in ROM. If this passes, the slot
  VM_ASSERT(vm, LongPtr_new(pSlot) == lpSlot);

  // The compiler must never produce bytecode that is able to attempt to write
  // to the bytecode image itself, but just to catch mistakes, here's an
  // assertion to make sure it doesn't write to bytecode. In a properly working
  // system (compiler + engine), this assertion isn't needed
  VM_ASSERT(vm, (lpSlot < vm->lpBytecode) ||
    (lpSlot >= LongPtr_add(vm->lpBytecode, getBytecodeSize(vm))));

  *pSlot = value;
}

static void setBuiltin(VM* vm, mvm_TeBuiltins builtinID, Value value) {
  CODE_COVERAGE(535); // Hit
  LongPtr lpBuiltins = getBytecodeSection(vm, BCS_BUILTINS, NULL);
  LongPtr lpBuiltin = LongPtr_add(lpBuiltins, (int16_t)(builtinID * sizeof (Value)));
  setSlot_long(vm, lpBuiltin, value);
}

// Warning: this function trashes the word at pObjectValue, which happens when
// traversing the prototype chain.
//
// Warning: this function will convert the value at pPropertyName to an interned
// string if it isn't already.
//
// Note: out_propertyValue is allowed point to the same address as pObjectValue
MVM_HIDDEN TeError getProperty(VM* vm, Value* pObjectValue, Value* pPropertyName, Value* out_propertyValue) {
  CODE_COVERAGE(48); // Hit

  mvm_TeError err;
  LongPtr lpArr;
  LongPtr lpClass;
  uint16_t length;
  TeTypeCode type;
  Value objectValue;
  Value propertyName;

  // This function may trigger a GC cycle because it may add a cell to the string intern table
  VM_ASSERT(vm, !vm->stack || !vm->stack->reg.usingCachedRegisters);

  // Note: toPropertyName can trigger a GC cycle
  err = toPropertyName(vm, pPropertyName);
  if (err != MVM_E_SUCCESS) return err;

SUB_GET_PROPERTY:

  propertyName = *pPropertyName;
  objectValue = *pObjectValue;
  type = deepTypeOf(vm, objectValue);
  switch (type) {
    case TC_REF_UINT8_ARRAY: {
      CODE_COVERAGE(339); // Hit
      lpArr = DynamicPtr_decode_long(vm, objectValue);
      uint16_t header = readAllocationHeaderWord_long(lpArr);
      length = vm_getAllocationSizeExcludingHeaderFromHeaderWord(header);
      if (propertyName == VM_VALUE_STR_LENGTH) {
        CODE_COVERAGE(340); // Hit
        VM_EXEC_SAFE_MODE(*pObjectValue = VM_VALUE_NULL);
        *out_propertyValue = VirtualInt14_encode(vm, length);
        return MVM_E_SUCCESS;
      } else {
        CODE_COVERAGE(341); // Hit
      }

      if (!Value_isVirtualInt14(propertyName)) {
        CODE_COVERAGE_ERROR_PATH(342); // Not hit
        return MVM_E_INVALID_ARRAY_INDEX;
      }
      int16_t index = VirtualInt14_decode(vm, propertyName);

      if ((index < 0) || (index >= length)) {
        CODE_COVERAGE(343); // Not hit
        *out_propertyValue = VM_VALUE_UNDEFINED;
        return MVM_E_SUCCESS;
      }

      uint8_t byteValue = LongPtr_read1(LongPtr_add(lpArr, (uint16_t)index));
      VM_EXEC_SAFE_MODE(*pObjectValue = VM_VALUE_NULL);
      *out_propertyValue = VirtualInt14_encode(vm, byteValue);
      return MVM_E_SUCCESS;
    }

    case TC_REF_PROPERTY_LIST: {
      CODE_COVERAGE(359); // Hit

      LongPtr lpPropertyList = DynamicPtr_decode_long(vm, objectValue);
      DynamicPtr dpProto = READ_FIELD_2(lpPropertyList, TsPropertyList, dpProto);

      if (propertyName == VM_VALUE_STR_PROTO) {
        CODE_COVERAGE_UNIMPLEMENTED(326); // Hit
        *out_propertyValue = dpProto;
        return MVM_E_SUCCESS;
      }

      while (lpPropertyList) {
        uint16_t headerWord = readAllocationHeaderWord_long(lpPropertyList);
        uint16_t size = vm_getAllocationSizeExcludingHeaderFromHeaderWord(headerWord);
        uint16_t propCount = (size - sizeof (TsPropertyList)) / 4;

        LongPtr p = LongPtr_add(lpPropertyList, sizeof (TsPropertyList));
        while (propCount--) {
          Value key = LongPtr_read2_aligned(p);
          p = LongPtr_add(p, 2);
          Value value = LongPtr_read2_aligned(p);
          p = LongPtr_add(p, 2);

          if (key == propertyName) {
            CODE_COVERAGE(361); // Hit
            VM_EXEC_SAFE_MODE(*pObjectValue = VM_VALUE_NULL);
            *out_propertyValue = value;
            return MVM_E_SUCCESS;
          } else {
            CODE_COVERAGE(362); // Hit
          }
        }

        DynamicPtr dpNext = READ_FIELD_2(lpPropertyList, TsPropertyList, dpNext);
         // Move to next group, if there is one
        if (dpNext != VM_VALUE_NULL) {
          CODE_COVERAGE(536); // Hit
          lpPropertyList = DynamicPtr_decode_long(vm, dpNext);
        } else { // Otherwise try read from the prototype
          CODE_COVERAGE(537); // Hit
          lpPropertyList = DynamicPtr_decode_long(vm, dpProto);
          if (lpPropertyList) {
            CODE_COVERAGE(538); // Hit
            dpProto = READ_FIELD_2(lpPropertyList, TsPropertyList, dpProto);
          } else {
            CODE_COVERAGE(539); // Hit
          }
        }
      }

      VM_EXEC_SAFE_MODE(*pObjectValue = VM_VALUE_NULL);
      *out_propertyValue = VM_VALUE_UNDEFINED;
      return MVM_E_SUCCESS;
    }

    case TC_REF_ARRAY: {
      CODE_COVERAGE(363); // Hit

      lpArr = DynamicPtr_decode_long(vm, objectValue);
      Value viLength = READ_FIELD_2(lpArr, TsArray, viLength);
      length = VirtualInt14_decode(vm, viLength);

      // Drill in to fixed-length array inside the array
      DynamicPtr dpData = READ_FIELD_2(lpArr, TsArray, dpData);
      lpArr = DynamicPtr_decode_long(vm, dpData);

      goto SUB_GET_PROP_FIXED_LENGTH_ARRAY;
    }

    case TC_REF_FIXED_LENGTH_ARRAY: {
      CODE_COVERAGE(286); // Hit

      lpArr = DynamicPtr_decode_long(vm, objectValue);

      uint16_t header = readAllocationHeaderWord_long(lpArr);
      uint16_t size = vm_getAllocationSizeExcludingHeaderFromHeaderWord(header);
      length = size >> 1;

      goto SUB_GET_PROP_FIXED_LENGTH_ARRAY;
    }

    case TC_REF_CLASS: {
      CODE_COVERAGE(615); // Hit
      lpClass = DynamicPtr_decode_long(vm, objectValue);
      // Delegate to the `staticProps` of the class
      *pObjectValue = READ_FIELD_2(lpClass, TsClass, staticProps);
      goto SUB_GET_PROPERTY;
    }

    default: return vm_newError(vm, MVM_E_TYPE_ERROR);
  }

SUB_GET_PROP_FIXED_LENGTH_ARRAY:
  CODE_COVERAGE(323); // Hit

  if (propertyName == VM_VALUE_STR_LENGTH) {
    CODE_COVERAGE(274); // Hit
    VM_EXEC_SAFE_MODE(*pObjectValue = VM_VALUE_NULL);
    *out_propertyValue = VirtualInt14_encode(vm, length);
    return MVM_E_SUCCESS;
  } else if (propertyName == VM_VALUE_STR_PROTO) {
    CODE_COVERAGE(275); // Hit
    VM_EXEC_SAFE_MODE(*pObjectValue = VM_VALUE_NULL);
    *out_propertyValue = getBuiltin(vm, BIN_ARRAY_PROTO);
    return MVM_E_SUCCESS;
  } else {
    CODE_COVERAGE(276); // Hit
  }

  // Array index
  if (Value_isVirtualInt14(propertyName)) {
    CODE_COVERAGE(277); // Hit
    int16_t index = VirtualInt14_decode(vm, propertyName);
    if (index < 0) {
      CODE_COVERAGE_ERROR_PATH(144); // Not hit
      return vm_newError(vm, MVM_E_INVALID_ARRAY_INDEX);
    }

    if ((uint16_t)index >= length) {
      CODE_COVERAGE(283); // Hit
      VM_EXEC_SAFE_MODE(*pObjectValue = VM_VALUE_NULL);
      *out_propertyValue = VM_VALUE_UNDEFINED;
      return MVM_E_SUCCESS;
    } else {
      CODE_COVERAGE(328); // Hit
    }
    // We've already checked if the value exceeds the length, so lpData
    // cannot be null and the capacity must be at least as large as the
    // length of the array.
    VM_ASSERT(vm, lpArr);
    VM_ASSERT(vm, length * 2 <= vm_getAllocationSizeExcludingHeaderFromHeaderWord(readAllocationHeaderWord_long(lpArr)));
    Value value = LongPtr_read2_aligned(LongPtr_add(lpArr, (uint16_t)index * 2));
    if (value == VM_VALUE_DELETED) {
      CODE_COVERAGE(329); // Hit
      value = VM_VALUE_UNDEFINED;
    } else {
      CODE_COVERAGE(364); // Hit
    }
    VM_EXEC_SAFE_MODE(*pObjectValue = VM_VALUE_NULL);
    *out_propertyValue = value;
    return MVM_E_SUCCESS;
  }
  CODE_COVERAGE(278); // Hit

  *pObjectValue = getBuiltin(vm, BIN_ARRAY_PROTO);
  if (*pObjectValue != VM_VALUE_NULL) {
    CODE_COVERAGE(396); // Hit
    goto SUB_GET_PROPERTY;
  } else {
    CODE_COVERAGE_UNTESTED(397); // Not hit
    VM_EXEC_SAFE_MODE(*pObjectValue = VM_VALUE_NULL);
    *out_propertyValue = VM_VALUE_UNDEFINED;
    return MVM_E_SUCCESS;
  }
}

// Note: the array is passed by pointer (pvArr) because this function can
// trigger a GC cycle, not because `*pvArr` is mutated by this function.
static void growArray(VM* vm, Value* pvArr, uint16_t newLength, uint16_t newCapacity) {
  CODE_COVERAGE(293); // Hit
  VM_ASSERT_NOT_USING_CACHED_REGISTERS(vm);

  VM_ASSERT(vm, newCapacity >= newLength);
  if (newCapacity > MAX_ALLOCATION_SIZE / 2) {
    CODE_COVERAGE_ERROR_PATH(540); // Not hit
    MVM_FATAL_ERROR(vm, MVM_E_ARRAY_TOO_LONG);
  }
  VM_ASSERT(vm, newCapacity != 0);

  uint16_t* pNewData = mvm_allocate(vm, newCapacity * 2, TC_REF_FIXED_LENGTH_ARRAY);
  // Copy values from the old array. Note that the above allocation can trigger
  // a GC collection which moves the array, so we need to decode the value again
  TsArray* arr = DynamicPtr_decode_native(vm, *pvArr);
  DynamicPtr dpOldData = arr->dpData;
  uint16_t oldCapacity = 0;
  if (dpOldData != VM_VALUE_NULL) {
    CODE_COVERAGE(294); // Hit
    LongPtr lpOldData = DynamicPtr_decode_long(vm, dpOldData);

    uint16_t oldDataHeader = readAllocationHeaderWord_long(lpOldData);
    uint16_t oldSize = vm_getAllocationSizeExcludingHeaderFromHeaderWord(oldDataHeader);
    VM_ASSERT(vm, (oldSize & 1) == 0);
    oldCapacity = oldSize / 2;

    memcpy_long(pNewData, lpOldData, oldSize);
  } else {
    CODE_COVERAGE(310); // Hit
  }
  CODE_COVERAGE(325); // Hit
  VM_ASSERT(vm, newCapacity >= oldCapacity);
  // Fill in the rest of the memory as holes
  uint16_t* p = &pNewData[oldCapacity];
  uint16_t* end = &pNewData[newCapacity];
  while (p != end) {
    *p++ = VM_VALUE_DELETED;
  }
  arr->dpData = ShortPtr_encode(vm, pNewData);
  arr->viLength = VirtualInt14_encode(vm, newLength);
}

/**
 * Gets an array of the keys of an object. Does not enumerate internal
 * properties, which are identified as any property with a key that is a
 * negative int14.
 */
MVM_HIDDEN TeError vm_objectKeys(VM* vm, Value* inout_slot) {
  CODE_COVERAGE(636); // Hit
  Value obj;
  LongPtr lpClass;

SUB_OBJECT_KEYS:
  obj = *inout_slot;

  TeTypeCode tc = deepTypeOf(vm, obj);
  if (tc == TC_REF_CLASS) {
    CODE_COVERAGE_UNTESTED(637); // Not hit
    lpClass = DynamicPtr_decode_long(vm, obj);
    // Delegate to the `staticProps` of the class
    *inout_slot = READ_FIELD_2(lpClass, TsClass, staticProps);
    goto SUB_OBJECT_KEYS;
  }
  CODE_COVERAGE(638); // Hit

  if (tc != TC_REF_PROPERTY_LIST) {
    CODE_COVERAGE_ERROR_PATH(639); // Not hit
    return MVM_E_OBJECT_KEYS_ON_NON_OBJECT;
  }

  // Count the number of properties (first add up the sizes)

  uint16_t propsSize = 0;
  Value propList = obj;
  // Note: the GC packs an object into a single allocation, so this should
  // frequently be O(1) and only loop once
  do {
    LongPtr lpPropList = DynamicPtr_decode_long(vm, propList);
    uint16_t segmentSize = vm_getAllocationSize_long(lpPropList) - sizeof(TsPropertyList);

    // Skip internal properties
    LongPtr lpProp = LongPtr_add(lpPropList, sizeof(TsPropertyList));
    while (segmentSize) {
      Value propKey = LongPtr_read2_aligned(lpProp);
      // Internal slots are always the first slots, so when we find the first non-internal slot then we've reached the end of the internal slots
      if ((propKey & 0x8003) != 0x8003) break;
      VM_ASSERT(vm, segmentSize >= 4); // Internal slots must always come in pairs
      segmentSize -= 4;
      lpProp = LongPtr_add(lpProp, 4);
    }

    propsSize += segmentSize;
    propList = LongPtr_read2_aligned(lpPropList) /* dpNext */;
    TABLE_COVERAGE(propList != VM_VALUE_NULL ? 1 : 0, 2, 640); // Hit 2/2
  } while (propList != VM_VALUE_NULL);

  // Each prop is 4 bytes, and each entry in the key array is 2 bytes
  uint16_t arrSize = propsSize >> 1;

  // If the array is empty, an empty allocation is illegal because allocations
  // need to be big enough to hold the tombstone. A 1-byte allocation will be
  // rounded down when asking the size, but rounded up in the allocation unit.
  if (!arrSize) {
    CODE_COVERAGE(641); // Hit
    arrSize = 1;
  } else {
    CODE_COVERAGE(691); // Hit
  }

  // Allocate the new array.
  Value* pArr = mvm_allocate(vm, arrSize, TC_REF_FIXED_LENGTH_ARRAY);
  obj = *inout_slot; // Invalidated by potential GC collection

  // Populate the array

  propList = obj;
  *inout_slot = ShortPtr_encode(vm, pArr);
  Value* p = pArr;
  do {
    LongPtr lpPropList = DynamicPtr_decode_long(vm, propList);
    propList = LongPtr_read2_aligned(lpPropList) /* dpNext */;

    uint16_t propsSize = vm_getAllocationSize_long(lpPropList) - sizeof(TsPropertyList);
    LongPtr lpProp = LongPtr_add(lpPropList, sizeof(TsPropertyList));
    TABLE_COVERAGE(propsSize != 0 ? 1 : 0, 2, 642); // Hit 2/2
    while (propsSize) {
      Value value = LongPtr_read2_aligned(lpProp);
      // Skip internal properties, which are negative int14. A negative int14
      // will have the low 2 bits set to say that it's an int14 and teh high bit
      // set to say that it's negative.
      if ((value & 0x8003) != 0x8003) {
        CODE_COVERAGE(692); // Hit
        *p = value;
        p++; // Move to next entry in array
      } else {
        CODE_COVERAGE(693); // Hit
      }
      // Each property cell is 4 bytes
      lpProp /* prop */ = LongPtr_add(lpProp /* prop */, 4);
      propsSize -= 4;
    }
    TABLE_COVERAGE(propList != VM_VALUE_NULL ? 1 : 0, 2, 643); // Hit 2/2
  } while (propList != VM_VALUE_NULL);

  VM_ASSERT(vm, (p - pArr) * 4 == propsSize);

  return MVM_E_SUCCESS;
}

/**
 * Note: the operands are passed by pointer because they can move during
 * setProperty if a GC cycle is triggered (the pointers should point to
 * GC-reachable slots).
 *
 * Warning: `pObject` is trashed. In particular, when setting a property on a
 * class, the `pObject` slot is reused for the `staticProps` of the class.
 *
 * Warning: this function will convert the value at pPropertyName to an interned
 * string if it isn't already.
 */
MVM_HIDDEN TeError setProperty(VM* vm, Value* pObject, Value* pPropertyName, Value* pPropertyValue) {
  CODE_COVERAGE(49); // Hit
  VM_ASSERT_NOT_USING_CACHED_REGISTERS(vm);

  mvm_TeError err;
  LongPtr lpClass;
  TeTypeCode type;

  // This function may trigger a GC cycle because it may add a cell to the string intern table
  VM_ASSERT(vm, !vm->stack || !vm->stack->reg.usingCachedRegisters);

  err = toPropertyName(vm, pPropertyName);
  if (err != MVM_E_SUCCESS) return err;

  MVM_LOCAL(Value, vObjectValue, 0);
  MVM_LOCAL(Value, vPropertyName, *pPropertyName);
  MVM_LOCAL(Value, vPropertyValue, *pPropertyValue);

SUB_SET_PROPERTY:

  MVM_SET_LOCAL(vObjectValue, *pObject);
  type = deepTypeOf(vm, MVM_GET_LOCAL(vObjectValue));
  switch (type) {
    case TC_REF_UINT8_ARRAY: {
      CODE_COVERAGE(594); // Hit
      // It's not valid for the optimizer to move a buffer into ROM if it's
      // ever written to, so it must be in RAM.
      VM_ASSERT(vm, Value_isShortPtr(MVM_GET_LOCAL(vObjectValue)));
      uint8_t* p = ShortPtr_decode(vm, MVM_GET_LOCAL(vObjectValue));
      uint16_t header = readAllocationHeaderWord(p);
      uint16_t length = vm_getAllocationSizeExcludingHeaderFromHeaderWord(header);

      if (!Value_isVirtualInt14(MVM_GET_LOCAL(vPropertyName))) {
        CODE_COVERAGE_ERROR_PATH(595); // Not hit
        return MVM_E_INVALID_ARRAY_INDEX;
      }
      int16_t index = VirtualInt14_decode(vm, MVM_GET_LOCAL(vPropertyName));
      if ((index < 0) || (index >= length)) {
        CODE_COVERAGE_ERROR_PATH(612); // Not hit
        return MVM_E_INVALID_ARRAY_INDEX;
      }

      Value byteValue = MVM_GET_LOCAL(vPropertyValue);
      if (!Value_isVirtualUInt8(byteValue)) {
        // For performance reasons, Microvium does not automatically coerce
        // values to bytes.
        CODE_COVERAGE_ERROR_PATH(613); // Not hit
        return MVM_E_CAN_ONLY_ASSIGN_BYTES_TO_UINT8_ARRAY;
      }

      p[index] = (uint8_t)VirtualInt14_decode(vm, byteValue);
      VM_EXEC_SAFE_MODE(*pObject = VM_VALUE_NULL);
      return MVM_E_SUCCESS;
    }

    case TC_REF_PROPERTY_LIST: {
      CODE_COVERAGE(366); // Hit
      if (MVM_GET_LOCAL(vPropertyName) == VM_VALUE_STR_PROTO) {
        CODE_COVERAGE_UNIMPLEMENTED(327); // Not hit
        VM_NOT_IMPLEMENTED(vm);
        return MVM_E_FATAL_ERROR_MUST_KILL_VM;
      } else {
        CODE_COVERAGE(541); // Hit
      }

      // Note: while objects in general can be in ROM, objects which are
      // writable must always be in RAM.

      MVM_LOCAL(TsPropertyList*, pPropertyList, DynamicPtr_decode_native(vm, MVM_GET_LOCAL(vObjectValue)));

      while (true) {
        CODE_COVERAGE(367); // Hit
        uint16_t headerWord = readAllocationHeaderWord(MVM_GET_LOCAL(pPropertyList));
        uint16_t size = vm_getAllocationSizeExcludingHeaderFromHeaderWord(headerWord);
        uint16_t propCount = (size - sizeof (TsPropertyList)) / 4;

        uint16_t* p = (uint16_t*)(MVM_GET_LOCAL(pPropertyList) + 1);
        while (propCount--) {
          Value key = *p++;

          // We can do direct comparison because the strings have been interned,
          // and numbers are represented in a normalized way.
          if (key == MVM_GET_LOCAL(vPropertyName)) {
            CODE_COVERAGE(368); // Hit
            *p = MVM_GET_LOCAL(vPropertyValue);
            VM_EXEC_SAFE_MODE(*pObject = VM_VALUE_NULL);
            return MVM_E_SUCCESS;
          } else {
            // Skip to next property
            p++;
            CODE_COVERAGE(369); // Hit
          }
        }

        DynamicPtr dpNext = MVM_GET_LOCAL(pPropertyList)->dpNext;
        // Move to next group, if there is one
        if (dpNext != VM_VALUE_NULL) {
          CODE_COVERAGE(542); // Hit
          MVM_SET_LOCAL(pPropertyList, DynamicPtr_decode_native(vm, dpNext));
        } else {
          CODE_COVERAGE(543); // Hit
          break;
        }
      }

      // If we reach the end, then this is a new property. We add new properties
      // by just appending a new TsPropertyList onto the linked list. The GC
      // will compact these into the head later.

      TsPropertyCell* pNewCell = GC_ALLOCATE_TYPE(vm, TsPropertyCell, TC_REF_PROPERTY_LIST);

      // GC collection invalidates the following values so we need to refresh
      // them from the stack slots.
      MVM_SET_LOCAL(vPropertyName, *pPropertyName);
      MVM_SET_LOCAL(vPropertyValue, *pPropertyValue);
      MVM_SET_LOCAL(pPropertyList, DynamicPtr_decode_native(vm, *pObject));

      /*
      Note: This is a bit of a pain. When we allocate the new cell, it may or
      may not trigger a GC collection cycle. If it does, then the object may be
      moved AND COMPACTED, so the linked list chain of properties is different
      to before (or may not be different, if there was no GC cycle), so we need
      to re-iterate the linked list to find the last node, where we append the
      property.
      */
      while (true) {
        DynamicPtr dpNext = MVM_GET_LOCAL(pPropertyList)->dpNext;
        if (dpNext != VM_VALUE_NULL) {
          MVM_SET_LOCAL(pPropertyList, DynamicPtr_decode_native(vm, dpNext));
        } else {
          break;
        }
      }

      ShortPtr spNewCell = ShortPtr_encode(vm, pNewCell);
      pNewCell->base.dpNext = VM_VALUE_NULL;
      pNewCell->base.dpProto = VM_VALUE_NULL; // Not used because this is a child cell, but still needs a value because the GC sees it.
      pNewCell->key = MVM_GET_LOCAL(vPropertyName);
      pNewCell->value = MVM_GET_LOCAL(vPropertyValue);

      // Attach to linked list. This needs to be a long-pointer write because we
      // don't know if the original property list was in data memory.
      //
      // Note: `pPropertyList` currently points to the last property list in
      // the chain.
      MVM_GET_LOCAL(pPropertyList)->dpNext = spNewCell;
      VM_EXEC_SAFE_MODE(*pObject = VM_VALUE_NULL);
      return MVM_E_SUCCESS;
    }
    case TC_REF_ARRAY: {
      CODE_COVERAGE(370); // Hit

      // Note: while objects in general can be in ROM, objects which are
      // writable must always be in RAM.

      MVM_LOCAL(TsArray*, arr, DynamicPtr_decode_native(vm, MVM_GET_LOCAL(vObjectValue)));
      VirtualInt14 viLength = MVM_GET_LOCAL(arr)->viLength;
      VM_ASSERT(vm, Value_isVirtualInt14(viLength));
      uint16_t oldLength = VirtualInt14_decode(vm, viLength);
      MVM_LOCAL(DynamicPtr, dpData, MVM_GET_LOCAL(arr)->dpData);
      MVM_LOCAL(uint16_t*, pData, NULL);
      uint16_t oldCapacity = 0;
      if (MVM_GET_LOCAL(dpData) != VM_VALUE_NULL) {
        CODE_COVERAGE(544); // Hit
        VM_ASSERT(vm, Value_isShortPtr(MVM_GET_LOCAL(dpData)));
        MVM_SET_LOCAL(pData, DynamicPtr_decode_native(vm, MVM_GET_LOCAL(dpData)));
        uint16_t dataSize = vm_getAllocationSize(MVM_GET_LOCAL(pData));
        oldCapacity = dataSize / 2;
      } else {
        CODE_COVERAGE(545); // Hit
      }

      // If the property name is "length" then we'll be changing the length
      if (MVM_GET_LOCAL(vPropertyName) == VM_VALUE_STR_LENGTH) {
        CODE_COVERAGE(282); // Hit

        if (!Value_isVirtualInt14(MVM_GET_LOCAL(vPropertyValue)))
          MVM_FATAL_ERROR(vm, MVM_E_TYPE_ERROR);
        uint16_t newLength = VirtualInt14_decode(vm, MVM_GET_LOCAL(vPropertyValue));

        if (newLength < oldLength) { // Making array smaller
          CODE_COVERAGE(176); // Hit
          // pData will not be null because oldLength must be more than 1 for it to get here
          VM_ASSERT(vm, MVM_GET_LOCAL(pData));
          // Wipe array items that aren't reachable
          uint16_t count = oldLength - newLength;
          uint16_t* p = &MVM_GET_LOCAL(pData)[newLength];
          while (count--)
            *p++ = VM_VALUE_DELETED;

          MVM_GET_LOCAL(arr)->viLength = VirtualInt14_encode(vm, newLength);
          VM_EXEC_SAFE_MODE(*pObject = VM_VALUE_NULL);
          return MVM_E_SUCCESS;
        } else if (newLength == oldLength) {
          CODE_COVERAGE_UNTESTED(546); // Not hit
          /* Do nothing */
        } else if (newLength <= oldCapacity) { // Array is getting bigger, but still less than capacity
          CODE_COVERAGE(287); // Hit

          // We can just overwrite the length field. Note that the newly
          // uncovered memory is already filled with VM_VALUE_DELETED
          MVM_GET_LOCAL(arr)->viLength = VirtualInt14_encode(vm, newLength);
          VM_EXEC_SAFE_MODE(*pObject = VM_VALUE_NULL);
          return MVM_E_SUCCESS;
        } else { // Make array bigger
          CODE_COVERAGE(288); // Hit
          // I'll assume that direct assignments to the length mean that people
          // know exactly how big the array should be, so we don't add any
          // extra capacity
          uint16_t newCapacity = newLength;
          growArray(vm, &*pObject, newLength, newCapacity);
          VM_EXEC_SAFE_MODE(*pObject = VM_VALUE_NULL);
          return MVM_E_SUCCESS;
        }
      } else if (MVM_GET_LOCAL(vPropertyName) == VM_VALUE_STR_PROTO) { // Writing to the __proto__ property
        CODE_COVERAGE_UNTESTED(289); // Not hit
        // We could make this read/write in future
        return vm_newError(vm, MVM_E_PROTO_IS_READONLY);
      } else if (Value_isVirtualInt14(MVM_GET_LOCAL(vPropertyName))) { // Array index
        CODE_COVERAGE(285); // Hit
        int16_t index = VirtualInt14_decode(vm, MVM_GET_LOCAL(vPropertyName) );
        if (index < 0) {
          CODE_COVERAGE_ERROR_PATH(24); // Not hit
          return vm_newError(vm, MVM_E_INVALID_ARRAY_INDEX);
        }

        // Need to expand the array?
        if ((uint16_t)index >= oldLength) {
          CODE_COVERAGE(290); // Hit
          uint16_t newLength = (uint16_t)index + 1;
          if ((uint16_t)index < oldCapacity) {
            CODE_COVERAGE(291); // Hit
            // The length changes to include the value. The extra slots are
            // already filled in with holes from the original allocation.
            MVM_GET_LOCAL(arr)->viLength = VirtualInt14_encode(vm, newLength);
          } else {
            CODE_COVERAGE(292); // Hit
            // We expand the capacity more aggressively here because this is the
            // path used when we push into arrays or just assign values to an
            // array in a loop.
            uint16_t newCapacity = oldCapacity * 2;
            if (newCapacity < VM_ARRAY_INITIAL_CAPACITY) newCapacity = VM_ARRAY_INITIAL_CAPACITY;
            if (newCapacity < newLength) newCapacity = newLength;
            growArray(vm, &*pObject, newLength, newCapacity);
            MVM_SET_LOCAL(vPropertyValue, *pPropertyValue); // Value could have changed due to GC collection
            MVM_SET_LOCAL(vObjectValue, *pObject); // Value could have changed due to GC collection
            MVM_SET_LOCAL(arr, DynamicPtr_decode_native(vm, MVM_GET_LOCAL(vObjectValue))); // Value could have changed due to GC collection
          }
        } // End of array expansion

        // By this point, the array should have expanded as necessary
        MVM_SET_LOCAL(dpData, MVM_GET_LOCAL(arr)->dpData);
        VM_ASSERT(vm, MVM_GET_LOCAL(dpData) != VM_VALUE_NULL);
        VM_ASSERT(vm, Value_isShortPtr(MVM_GET_LOCAL(dpData)));
        MVM_SET_LOCAL(pData, DynamicPtr_decode_native(vm, MVM_GET_LOCAL(dpData)));
        VM_ASSERT(vm, !!MVM_GET_LOCAL(pData));

        // Write the item to memory
        MVM_GET_LOCAL(pData)[(uint16_t)index] = MVM_GET_LOCAL(vPropertyValue);

        VM_EXEC_SAFE_MODE(*pObject = VM_VALUE_NULL);
        return MVM_E_SUCCESS;
      }

      // Else not a valid array index
      CODE_COVERAGE_ERROR_PATH(140); // Not hit
      return vm_newError(vm, MVM_E_INVALID_ARRAY_INDEX);
    }

    case TC_REF_CLASS: {
      CODE_COVERAGE(630); // Hit
      lpClass = DynamicPtr_decode_long(vm, MVM_GET_LOCAL(vObjectValue));
      // Delegate to the `staticProps` of the class
      *pObject = READ_FIELD_2(lpClass, TsClass, staticProps);
      goto SUB_SET_PROPERTY;
    }

    default: return vm_newError(vm, MVM_E_TYPE_ERROR);
  }
}

/** Converts the argument to either an TC_VAL_INT14 or a TC_REF_INTERNED_STRING, or gives an error */
static TeError toPropertyName(VM* vm, Value* value) {
  CODE_COVERAGE(50); // Hit

  // This function may trigger a GC cycle because it may add a cell to the string intern table
  VM_ASSERT(vm, !vm->stack || !vm->stack->reg.usingCachedRegisters);
  VM_POTENTIAL_GC_POINT(vm);

  // Property names in microvium are either integer indexes or non-integer interned strings
  TeTypeCode type = deepTypeOf(vm, *value);
  switch (type) {
    // These are already valid property names
    case TC_VAL_INT14: {
      CODE_COVERAGE(279); // Hit
      if (VirtualInt14_decode(vm, *value) < 0) {
        CODE_COVERAGE_UNTESTED(280); // Not hit
        return vm_newError(vm, MVM_E_RANGE_ERROR);
      }
      CODE_COVERAGE(281); // Hit
      return MVM_E_SUCCESS;
    }
    case TC_REF_INTERNED_STRING: {
      CODE_COVERAGE(373); // Hit
      return MVM_E_SUCCESS;
    }

    case TC_REF_INT32: {
      CODE_COVERAGE_ERROR_PATH(374); // Not hit
      // 32-bit numbers are out of the range of supported array indexes
      return vm_newError(vm, MVM_E_RANGE_ERROR);
    }

    case TC_REF_STRING: {
      CODE_COVERAGE(375); // Hit

      // Note: In Microvium at the moment, it's illegal to use an integer-valued
      // string as a property name. If the string is in bytecode, it will only
      // have the type TC_REF_STRING if it's a number and is illegal.
      if (!Value_isShortPtr(*value)) {
        return vm_newError(vm, MVM_E_TYPE_ERROR);
      }

      if (vm_ramStringIsNonNegativeInteger(vm, *value)) {
        CODE_COVERAGE_ERROR_PATH(378); // Not hit
        return vm_newError(vm, MVM_E_TYPE_ERROR);
      } else {
        CODE_COVERAGE(379); // Hit
      }

      // Strings need to be converted to interned strings in order to be valid
      // property names. This is because properties are searched by reference
      // equality.
      toInternedString(vm, value);
      return MVM_E_SUCCESS;
    }

    case TC_VAL_STR_LENGTH: {
      CODE_COVERAGE(272); // Hit
      return MVM_E_SUCCESS;
    }

    case TC_VAL_STR_PROTO: {
      CODE_COVERAGE(273); // Hit
      return MVM_E_SUCCESS;
    }
    default: {
      CODE_COVERAGE_ERROR_PATH(380); // Not hit
      return vm_newError(vm, MVM_E_TYPE_ERROR);
    }
  }
}

// Converts a TC_REF_STRING to a TC_REF_INTERNED_STRING
// TODO: Test cases for this function
static void toInternedString(VM* vm, Value* pValue) {
  CODE_COVERAGE(51); // Hit
  Value value = *pValue;
  VM_ASSERT(vm, deepTypeOf(vm, value) == TC_REF_STRING);

  // This function may trigger a GC cycle because it may add a cell to the intern table
  VM_ASSERT(vm, !vm->stack || !vm->stack->reg.usingCachedRegisters);

  // TC_REF_STRING values are always in GC memory. If they were in flash, they'd
  // already be TC_REF_INTERNED_STRING.
  char* pStr1 = DynamicPtr_decode_native(vm, value);
  uint16_t str1Size = vm_getAllocationSize(pStr1);

  LongPtr lpStr1 = LongPtr_new(pStr1);
  // Note: the sizes here include the null terminator
  if ((str1Size == sizeof PROTO_STR) && (memcmp_long(lpStr1, LongPtr_new((void*)&PROTO_STR), sizeof PROTO_STR) == 0)) {
    CODE_COVERAGE_UNTESTED(547); // Not hit
    *pValue = VM_VALUE_STR_PROTO;
  } else if ((str1Size == sizeof LENGTH_STR) && (memcmp_long(lpStr1, LongPtr_new((void*)&LENGTH_STR), sizeof LENGTH_STR) == 0)) {
    CODE_COVERAGE(548); // Hit
    *pValue = VM_VALUE_STR_LENGTH;
  } else {
    CODE_COVERAGE(549); // Hit
  }

  LongPtr lpBytecode = vm->lpBytecode;

  // We start by searching the string table for interned strings that are baked
  // into the ROM. These are stored alphabetically, so we can perform a binary
  // search.

  uint16_t stringTableOffset = getSectionOffset(vm->lpBytecode, BCS_STRING_TABLE);
  uint16_t stringTableSize = getSectionOffset(vm->lpBytecode, vm_sectionAfter(vm, BCS_STRING_TABLE)) - stringTableOffset;
  int strCount = stringTableSize / sizeof (Value);

  int first = 0;
  int last = strCount - 1;

  while (first <= last) {
    CODE_COVERAGE(381); // Hit
    int middle = (first + last) / 2;
    uint16_t str2Offset = stringTableOffset + middle * 2;
    Value vStr2 = LongPtr_read2_aligned(LongPtr_add(lpBytecode, str2Offset));
    LongPtr lpStr2 = DynamicPtr_decode_long(vm, vStr2);
    uint16_t header = readAllocationHeaderWord_long(lpStr2);
    VM_ASSERT(vm, vm_getTypeCodeFromHeaderWord(header) == TC_REF_INTERNED_STRING);
    uint16_t str2Size = vm_getAllocationSizeExcludingHeaderFromHeaderWord(header);
    int compareSize = str1Size < str2Size ? str1Size : str2Size;
    int c = memcmp_long(lpStr1, lpStr2, compareSize);

    // If they compare equal for the range that they have in common, we check the length
    if (c == 0) {
      CODE_COVERAGE(382); // Hit
      if (str1Size < str2Size) {
        CODE_COVERAGE_UNTESTED(383); // Not hit
        c = -1;
      } else if (str1Size > str2Size) {
        CODE_COVERAGE_UNTESTED(384); // Not hit
        c = 1;
      } else {
        CODE_COVERAGE(385); // Hit
        // Exact match
        *pValue = vStr2;
        return;
      }
    }

    // c is > 0 if the string we're searching for comes after the middle point
    if (c > 0) {
      CODE_COVERAGE(386); // Hit
      first = middle + 1;
    } else {
      CODE_COVERAGE(387); // Hit
      last = middle - 1;
    }
  }

  // At this point, we haven't found the interned string in the bytecode. We
  // need to check in RAM. Now we're comparing an in-RAM string against other
  // in-RAM strings. We're looking for an exact match, not performing a binary
  // search with inequality comparison, since the linked list of interned
  // strings in RAM is not sorted.
  Value vInternedStrings = getBuiltin(vm, BIN_INTERNED_STRINGS);
  Value spCell = vInternedStrings;
  while (spCell != VM_VALUE_UNDEFINED) {
    CODE_COVERAGE(388); // Hit
    VM_ASSERT(vm, Value_isShortPtr(spCell));
    TsInternedStringCell* pCell = ShortPtr_decode(vm, spCell);
    Value vStr2 = pCell->str;
    char* pStr2 = ShortPtr_decode(vm, vStr2);
    uint16_t str2Header = readAllocationHeaderWord(pStr2);
    uint16_t str2Size = vm_getAllocationSizeExcludingHeaderFromHeaderWord(str2Header);

    // The sizes have to match for the strings to be equal
    if (str2Size == str1Size) {
      CODE_COVERAGE(389); // Hit
      // Note: we use memcmp instead of strcmp because strings are allowed to
      // have embedded null terminators.
      int c = memcmp(pStr1, pStr2, str1Size);
      // Equal?
      if (c == 0) {
        CODE_COVERAGE(390); // Hit
        *pValue = vStr2;
        return;
      } else {
        CODE_COVERAGE(391); // Hit
      }
    } else {
      CODE_COVERAGE(550); // Hit
    }
    spCell = pCell->spNext;
    TABLE_COVERAGE(spCell ? 1 : 0, 2, 551); // Hit 1/2
  }

  CODE_COVERAGE(616); // Hit

  // If we get here, it means there was no matching interned string already
  // existing in ROM or RAM. We upgrade the current string to a
  // TC_REF_INTERNED_STRING, since we now know it doesn't conflict with any existing
  // existing interned strings.
  setHeaderWord(vm, pStr1, TC_REF_INTERNED_STRING, str1Size);

  // Add the string to the linked list of interned strings
  TsInternedStringCell* pCell = GC_ALLOCATE_TYPE(vm, TsInternedStringCell, TC_REF_FIXED_LENGTH_ARRAY);
  value = *pValue; // Invalidated by potential GC collection
  vInternedStrings = getBuiltin(vm, BIN_INTERNED_STRINGS);  // Invalidated by potential GC collection
  // Push onto linked list2
  pCell->spNext = vInternedStrings;
  pCell->str = value;
  setBuiltin(vm, BIN_INTERNED_STRINGS, ShortPtr_encode(vm, pCell));
}

static int memcmp_long(LongPtr p1, LongPtr p2, size_t size) {
  CODE_COVERAGE(471); // Hit
  return MVM_LONG_MEM_CMP(p1, p2, size);
}

static void memcpy_long(void* target, LongPtr source, size_t size) {
  CODE_COVERAGE(9); // Hit
  MVM_LONG_MEM_CPY(target, source, size);
}

/** Size of string excluding bonus null terminator */
static uint16_t vm_stringSizeUtf8(VM* vm, Value value) {
  CODE_COVERAGE(53); // Hit
  TeTypeCode typeCode = deepTypeOf(vm, value);
  switch (typeCode) {
    case TC_REF_STRING:
    case TC_REF_INTERNED_STRING: {
      LongPtr lpStr = DynamicPtr_decode_long(vm, value);
      uint16_t headerWord = readAllocationHeaderWord_long(lpStr);
      // Less 1 because of the bonus null terminator
      return vm_getAllocationSizeExcludingHeaderFromHeaderWord(headerWord) - 1;
    }
    case TC_VAL_STR_PROTO: {
      CODE_COVERAGE_UNTESTED(552); // Not hit
      return sizeof PROTO_STR - 1;
    }
    case TC_VAL_STR_LENGTH: {
      CODE_COVERAGE(608); // Hit
      return sizeof LENGTH_STR - 1;
    }
    default:
      VM_ASSERT_UNREACHABLE(vm);
      return 0;
  }
}

/**
 * Checks if a string contains only decimal digits (and is not empty). May only
 * be called on TC_REF_STRING and only those in GC memory.
 */
static bool vm_ramStringIsNonNegativeInteger(VM* vm, Value str) {
  CODE_COVERAGE(55); // Hit
  VM_ASSERT(vm, deepTypeOf(vm, str) == TC_REF_STRING);

  char* pStr = ShortPtr_decode(vm, str);

  // Length excluding bonus null terminator
  uint16_t len = vm_getAllocationSize(pStr) - 1;
  char* p = pStr;
  if (!len) {
    CODE_COVERAGE_UNTESTED(554); // Not hit
    return false;
  } else {
    CODE_COVERAGE(555); // Hit
  }
  while (len--) {
    CODE_COVERAGE(398); // Hit
    if (!isdigit(*p++)) {
      CODE_COVERAGE(399); // Hit
      return false;
    } else {
      CODE_COVERAGE_UNTESTED(400); // Not hit
    }
  }
  return true;
}

// Convert a string to an integer
TeError strToInt32(mvm_VM* vm, mvm_Value value, int32_t* out_result) {
  CODE_COVERAGE(404); // Not hit

  TeTypeCode type = deepTypeOf(vm, value);
  VM_ASSERT(vm, type == TC_REF_STRING || type == TC_REF_INTERNED_STRING);

  bool isFloat = false;

  // Note: this function is implemented to use long pointers to access ROM
  // memory. This is because the string may be in ROM and we don't want to copy
  // the string to RAM. Copying to RAM involves allocating the available memory,
  // which requires that the VM register cache be in a flushed state, which they
  // aren't necessarily at this point in the code.

  LongPtr start = DynamicPtr_decode_long(vm, value);
  LongPtr s = start;
  uint16_t size = vm_getAllocationSize_long(s);
  uint16_t len = size - 1; // Excluding null terminator

  // Skip leading whitespace
  while (isspace(LongPtr_read1(s))) {
    s = LongPtr_add(s, 1);
  }

  int sign = (LongPtr_read1(s) == '-') ? -1 : 1;
  if (LongPtr_read1(s) == '+' || LongPtr_read1(s) == '-') {
    s = LongPtr_add(s, 1);
  }

  // Find end of digits
  int32_t n = 0;
  while (isdigit(LongPtr_read1(s))) {
    int32_t n2 = n * 10 + (LongPtr_read1(s) - '0');
    s = LongPtr_add(s, 1);
    // Overflow Int32
    if (n2 < n) isFloat = true;
    n = n2;
  }

  // Decimal point
  if ((LongPtr_read1(s) == ',') || (LongPtr_read1(s) == '.')) {
    CODE_COVERAGE(739); // Not hit
    isFloat = true;
    s = LongPtr_add(s, 1);
  }

  // Digits after decimal point
  while (isdigit(LongPtr_read1(s))) s = LongPtr_add(s, 1);

  // Skip trailing whitespace
  while (isspace(LongPtr_read1(s))) s = LongPtr_add(s, 1);

  // Check if we reached the end of the string. If we haven't reached the end of
  // the string then there is a non-digit character in the string.
  if (LongPtr_sub(s, start) != len) {
    CODE_COVERAGE(740); // Not hit
    return MVM_E_NAN;
  }

  // This function cannot handle floating point numbers
  if (isFloat) {
    CODE_COVERAGE_UNTESTED(741); // Not hit
    return MVM_E_FLOAT64;
  }

  CODE_COVERAGE(656); // Hit

  *out_result = sign * n;

  return MVM_E_SUCCESS;
}

TeError toInt32Internal(mvm_VM* vm, mvm_Value value, int32_t* out_result) {
  CODE_COVERAGE(56); // Hit
  // TODO: when the type codes are more stable, we should convert these to a table.
  *out_result = 0;
  TeTypeCode type = deepTypeOf(vm, value);
  MVM_SWITCH(type, TC_END - 1) {
    MVM_CASE(TC_VAL_INT14):
    MVM_CASE(TC_REF_INT32): {
      CODE_COVERAGE(401); // Hit
      *out_result = vm_readInt32(vm, type, value);
      return MVM_E_SUCCESS;
    }
    MVM_CASE(TC_REF_FLOAT64): {
      CODE_COVERAGE(402); // Hit
      return MVM_E_FLOAT64;
    }
    MVM_CASE(TC_REF_STRING):
    MVM_CASE(TC_REF_INTERNED_STRING): {
      CODE_COVERAGE(403); // Not hit
      return strToInt32(vm, value, out_result);
    }
    MVM_CASE(TC_VAL_STR_LENGTH): {
      CODE_COVERAGE(270); // Not hit
      return MVM_E_NAN;
    }
    MVM_CASE(TC_VAL_STR_PROTO): {
      CODE_COVERAGE(271); // Not hit
      return MVM_E_NAN;
    }
    MVM_CASE(TC_REF_PROPERTY_LIST): {
      CODE_COVERAGE(405); // Hit
      return MVM_E_NAN;
    }
    MVM_CASE(TC_REF_ARRAY): {
      CODE_COVERAGE_UNTESTED(406); // Not hit
      return MVM_E_NAN;
    }
    MVM_CASE(TC_REF_FUNCTION): {
      CODE_COVERAGE(408); // Hit
      return MVM_E_NAN;
    }
    MVM_CASE(TC_REF_HOST_FUNC): {
      CODE_COVERAGE_UNTESTED(409); // Not hit
      return MVM_E_NAN;
    }
    MVM_CASE(TC_REF_CLOSURE): {
      CODE_COVERAGE_UNTESTED(410); // Not hit
      return MVM_E_NAN;
    }
    MVM_CASE(TC_REF_UINT8_ARRAY): {
      CODE_COVERAGE_UNTESTED(411); // Not hit
      return MVM_E_NAN;
    }
    MVM_CASE(TC_REF_VIRTUAL): {
      CODE_COVERAGE_UNTESTED(632); // Not hit
      VM_RESERVED(vm);
      return MVM_E_FATAL_ERROR_MUST_KILL_VM;
    }
    MVM_CASE(TC_REF_CLASS): {
      CODE_COVERAGE(633); // Hit
      return MVM_E_NAN;
    }
    MVM_CASE(TC_REF_SYMBOL): {
      CODE_COVERAGE_UNTESTED(412); // Not hit
      return MVM_E_NAN;
    }
    MVM_CASE(TC_VAL_UNDEFINED): {
      CODE_COVERAGE(413); // Hit
      return MVM_E_NAN;
    }
    MVM_CASE(TC_VAL_NULL): {
      CODE_COVERAGE(414); // Hit
      break;
    }
    MVM_CASE(TC_VAL_TRUE): {
      CODE_COVERAGE_UNTESTED(415); // Not hit
      *out_result = 1; break;
    }
    MVM_CASE(TC_VAL_FALSE): {
      CODE_COVERAGE_UNTESTED(416); // Not hit
      break;
    }
    MVM_CASE(TC_VAL_NAN): {
      CODE_COVERAGE(417); // Hit
      return MVM_E_NAN;
    }
    MVM_CASE(TC_VAL_NEG_ZERO): {
      CODE_COVERAGE(418); // Hit
      return MVM_E_NEG_ZERO;
    }
    MVM_CASE(TC_VAL_DELETED): {
      CODE_COVERAGE_UNTESTED(419); // Not hit
      return MVM_E_NAN;
    }
    MVM_CASE(TC_VAL_NO_OP_FUNC): {
      CODE_COVERAGE(742); // Not hit
      return MVM_E_NAN;
    }
    default:
      VM_ASSERT_UNREACHABLE(vm);
  }
  return MVM_E_SUCCESS;
}

int32_t mvm_toInt32(mvm_VM* vm, mvm_Value value) {
  CODE_COVERAGE(57); // Hit
  int32_t result;
  TeError err = toInt32Internal(vm, value, &result);
  if (err == MVM_E_SUCCESS) {
    CODE_COVERAGE(420); // Hit
    return result;
  } else if (err == MVM_E_NAN) {
    CODE_COVERAGE(421); // Hit
    return 0;
  } else if (err == MVM_E_NEG_ZERO) {
    CODE_COVERAGE_UNTESTED(422); // Not hit
    return 0;
  } else {
    CODE_COVERAGE_UNTESTED(423); // Not hit
  }

  VM_ASSERT(vm, deepTypeOf(vm, value) == TC_REF_FLOAT64);
  #if MVM_SUPPORT_FLOAT
    return (int32_t)mvm_toFloat64(vm, value);
  #else // !MVM_SUPPORT_FLOAT
    // If things were compiled correctly, there shouldn't be any floats in the
    // system at all
    return 0;
  #endif
}

#if MVM_SUPPORT_FLOAT
MVM_FLOAT64 mvm_toFloat64(mvm_VM* vm, mvm_Value value) {
  CODE_COVERAGE(58); // Hit
  int32_t result;
  TeError err = toInt32Internal(vm, value, &result);
  if (err == MVM_E_SUCCESS) {
    CODE_COVERAGE(424); // Hit
    return result;
  } else if (err == MVM_E_NAN) {
    CODE_COVERAGE(425); // Hit
    return MVM_FLOAT64_NAN;
  } else if (err == MVM_E_NEG_ZERO) {
    CODE_COVERAGE(426); // Hit
    return MVM_FLOAT_NEG_ZERO;
  } else {
    CODE_COVERAGE(427); // Hit
  }

  VM_ASSERT(vm, deepTypeOf(vm, value) == TC_REF_FLOAT64);
  LongPtr lpFloat = DynamicPtr_decode_long(vm, value);
  MVM_FLOAT64 f;
  memcpy_long(&f, lpFloat, sizeof f);
  return f;
}
#endif // MVM_SUPPORT_FLOAT

// See implementation of mvm_equal for the meaning of each
typedef enum TeEqualityAlgorithm {
  EA_NONE,
  EA_COMPARE_PTR_VALUE_AND_TYPE,
  EA_COMPARE_NON_PTR_TYPE,
  EA_COMPARE_REFERENCE,
  EA_NOT_EQUAL,
  EA_COMPARE_STRING,
} TeEqualityAlgorithm;

static const TeEqualityAlgorithm equalityAlgorithmByTypeCode[TC_END] = {
  EA_NONE,                       // TC_REF_TOMBSTONE          = 0x0
  EA_COMPARE_PTR_VALUE_AND_TYPE, // TC_REF_INT32              = 0x1
  EA_COMPARE_PTR_VALUE_AND_TYPE, // TC_REF_FLOAT64            = 0x2
  EA_COMPARE_STRING,             // TC_REF_STRING             = 0x3
  EA_COMPARE_STRING,             // TC_REF_INTERNED_STRING    = 0x4
  EA_COMPARE_REFERENCE,          // TC_REF_FUNCTION           = 0x5
  EA_COMPARE_PTR_VALUE_AND_TYPE, // TC_REF_HOST_FUNC          = 0x6
  EA_COMPARE_PTR_VALUE_AND_TYPE, // TC_REF_BIG_INT            = 0x7
  EA_COMPARE_REFERENCE,          // TC_REF_SYMBOL             = 0x8
  EA_NONE,                       // TC_REF_CLASS              = 0x9
  EA_NONE,                       // TC_REF_VIRTUAL            = 0xA
  EA_NONE,                       // TC_REF_RESERVED_1         = 0xB
  EA_COMPARE_REFERENCE,          // TC_REF_PROPERTY_LIST      = 0xC
  EA_COMPARE_REFERENCE,          // TC_REF_ARRAY              = 0xD
  EA_COMPARE_REFERENCE,          // TC_REF_FIXED_LENGTH_ARRAY = 0xE
  EA_COMPARE_REFERENCE,          // TC_REF_CLOSURE            = 0xF
  EA_COMPARE_NON_PTR_TYPE,       // TC_VAL_INT14              = 0x10
  EA_COMPARE_NON_PTR_TYPE,       // TC_VAL_UNDEFINED          = 0x11
  EA_COMPARE_NON_PTR_TYPE,       // TC_VAL_NULL               = 0x12
  EA_COMPARE_NON_PTR_TYPE,       // TC_VAL_TRUE               = 0x13
  EA_COMPARE_NON_PTR_TYPE,       // TC_VAL_FALSE              = 0x14
  EA_NOT_EQUAL,                  // TC_VAL_NAN                = 0x15
  EA_COMPARE_NON_PTR_TYPE,       // TC_VAL_NEG_ZERO           = 0x16
  EA_NONE,                       // TC_VAL_DELETED            = 0x17
  EA_COMPARE_STRING,             // TC_VAL_STR_LENGTH         = 0x18
  EA_COMPARE_STRING,             // TC_VAL_STR_PROTO          = 0x19
  EA_COMPARE_NON_PTR_TYPE,       // TC_VAL_NO_OP_FUNC         = 0x1A
};

bool mvm_equal(mvm_VM* vm, mvm_Value a, mvm_Value b) {
  CODE_COVERAGE(462); // Hit
  VM_ASSERT_NOT_USING_CACHED_REGISTERS(vm);

  TeTypeCode aType = deepTypeOf(vm, a);
  TeTypeCode bType = deepTypeOf(vm, b);
  TeEqualityAlgorithm algorithmA = equalityAlgorithmByTypeCode[aType];
  TeEqualityAlgorithm algorithmB = equalityAlgorithmByTypeCode[bType];

  TABLE_COVERAGE(algorithmA, 6, 556); // Hit 4/6
  TABLE_COVERAGE(algorithmB, 6, 557); // Hit 4/6
  TABLE_COVERAGE(aType, TC_END, 558); // Hit 7/27
  TABLE_COVERAGE(bType, TC_END, 559); // Hit 9/27

  // If the values aren't even in the same class of comparison, they're not
  // equal. In particular, strings will not be equal to non-strings.
  if (algorithmA != algorithmB) {
    CODE_COVERAGE(560); // Hit
    return false;
  } else {
    CODE_COVERAGE(561); // Hit
  }

  if (algorithmA == EA_NOT_EQUAL) {
    CODE_COVERAGE(562); // Hit
    return false; // E.g. comparing NaN
  } else {
    CODE_COVERAGE(563); // Hit
  }

  if (a == b) {
    CODE_COVERAGE(564); // Hit
    return true;
  } else {
    CODE_COVERAGE(565); // Hit
  }

  switch (algorithmA) {
    case EA_COMPARE_REFERENCE: {
      // Reference equality comparison assumes that two values with different
      // locations in memory must be different values, since their identity is
      // their address. Since we've already checked `a == b`, this must be false.
      return false;
    }
    case EA_COMPARE_NON_PTR_TYPE: {
      // Non-pointer types are those like Int14 and the well-known values
      // (except NaN). These can just be compared with `a == b`, which we've
      // already done.
      return false;
    }

    case EA_COMPARE_STRING: {
      // Strings are a pain to compare because there are edge cases like the
      // fact that the string "length" _may_ be represented by
      // VM_VALUE_STR_LENGTH rather than a pointer to a string (or it may be a
      // TC_REF_STRING). To keep the code concise, I'm fetching a pointer to the
      // string data itself and then comparing that. This is the only equality
      // algorithm that doesn't check the type. It makes use of the check for
      // `algorithmA != algorithmB` from earlier and the fact that only strings
      // compare with this algorithm, which means we won't get to this point
      // unless both `a` and `b` are strings.
      if (a == b) {
        CODE_COVERAGE_UNTESTED(566); // Not hit
        return true;
      } else {
        CODE_COVERAGE(567); // Hit
      }
      size_t sizeA;
      size_t sizeB;
      LongPtr lpStrA = vm_toStringUtf8_long(vm, a, &sizeA);
      LongPtr lpStrB = vm_toStringUtf8_long(vm, b, &sizeB);
      bool result = (sizeA == sizeB) && (memcmp_long(lpStrA, lpStrB, (uint16_t)sizeA) == 0);
      TABLE_COVERAGE(result ? 1 : 0, 2, 568); // Hit 2/2
      return result;
    }

    /*
    Compares two values that are both pointer values that point to non-reference
    types (e.g. int32). These will be equal if the value pointed to has the same
    type, the same size, and the raw data pointed to is the same.
    */
    case EA_COMPARE_PTR_VALUE_AND_TYPE: {
      CODE_COVERAGE_UNTESTED(475); // Not hit

      if (a == b) {
        CODE_COVERAGE_UNTESTED(569); // Not hit
        return true;
      } else {
        CODE_COVERAGE_UNTESTED(570); // Not hit
      }
      if (aType != bType) {
        CODE_COVERAGE_UNTESTED(571); // Not hit
        return false;
      } else {
        CODE_COVERAGE_UNTESTED(572); // Not hit
        }

      LongPtr lpA = DynamicPtr_decode_long(vm, a);
      LongPtr lpB = DynamicPtr_decode_long(vm, b);
      uint16_t aHeaderWord = readAllocationHeaderWord_long(lpA);
      uint16_t bHeaderWord = readAllocationHeaderWord_long(lpB);
      // If the header words are different, the sizes or types are different
      if (aHeaderWord != bHeaderWord) {
        CODE_COVERAGE_UNTESTED(476); // Not hit
        return false;
      } else {
        CODE_COVERAGE_UNTESTED(477); // Not hit
      }
      uint16_t size = vm_getAllocationSizeExcludingHeaderFromHeaderWord(aHeaderWord);
      if (memcmp_long(lpA, lpB, size) == 0) {
        CODE_COVERAGE_UNTESTED(481); // Not hit
        return true;
      } else {
        CODE_COVERAGE_UNTESTED(482); // Not hit
        return false;
      }
    }

    default: {
      VM_ASSERT_UNREACHABLE(vm);
      return false;
    }
  }
}

bool mvm_isNaN(mvm_Value value) {
  CODE_COVERAGE_UNTESTED(573); // Not hit
  return value == VM_VALUE_NAN;
}

#if MVM_INCLUDE_SNAPSHOT_CAPABILITY

// Called during snapshotting to convert native pointers to their position-independent form
static void serializePtr(VM* vm, Value* pv) {
  CODE_COVERAGE(576); // Hit
  Value v = *pv;
  if (!Value_isShortPtr(v)) {
    CODE_COVERAGE(577); // Hit
    return;
  } else {
    CODE_COVERAGE(578); // Hit
  }
  void* p = ShortPtr_decode(vm, v);

  // Pointers are encoded as an offset in the heap
  uint16_t offsetInHeap = pointerOffsetInHeap(vm, vm->pLastBucket, p);

  // The lowest bit must be zero so that this is tagged as a "ShortPtr".
  VM_ASSERT(vm, (offsetInHeap & 1) == 0);

  *pv = offsetInHeap;
}

// The opposite of `loadPointers`
static void serializePointers(VM* vm, mvm_TsBytecodeHeader* bc) {
  CODE_COVERAGE(579); // Hit
  // CAREFUL! This function mutates `bc`, not `vm`.

  uint16_t n;
  uint16_t* p;

  uint16_t heapOffset = bc->sectionOffsets[BCS_HEAP];
  uint16_t heapSize = bc->bytecodeSize - heapOffset;

  uint16_t* pGlobals = (uint16_t*)((uint8_t*)bc + bc->sectionOffsets[BCS_GLOBALS]);
  uint16_t* heapMemory = (uint16_t*)((uint8_t*)bc + heapOffset);

  // Roots in global variables
  uint16_t globalsSize = bc->sectionOffsets[BCS_GLOBALS + 1] - bc->sectionOffsets[BCS_GLOBALS];
  p = pGlobals;
  n = globalsSize / 2;
  TABLE_COVERAGE(n ? 1 : 0, 2, 580); // Hit 1/2
  while (n--) {
    serializePtr(vm, p++);
  }

  // Pointers in heap memory
  p = heapMemory;
  uint16_t* heapEnd = (uint16_t*)((uint8_t*)heapMemory + heapSize);
  while (p < heapEnd) {
    CODE_COVERAGE(581); // Hit
    uint16_t header = *p++;
    uint16_t size = vm_getAllocationSizeExcludingHeaderFromHeaderWord(header);
    int words = size / 2; // Note: round **down** to nearest word
    uint16_t* next = p + (size + 1) / 2; // Note: round **up** to nearest word
    TABLE_COVERAGE(next == p + words ? 1 : 0, 2, 694); // Hit 2/2
    TeTypeCode tc = vm_getTypeCodeFromHeaderWord(header);

    if (tc < TC_REF_DIVIDER_CONTAINER_TYPES) { // Non-container types
      CODE_COVERAGE(582); // Hit
      p = next;
      continue;
    } else {
      // Else, container types
      CODE_COVERAGE(583); // Hit
    }

    while (words--) {
      if (Value_isShortPtr(*p))
        serializePtr(vm, p);
      p++;
    }
    p = next;
  }
}

void* mvm_createSnapshot(mvm_VM* vm, size_t* out_size) {
  CODE_COVERAGE(503); // Hit
  if (out_size)
    *out_size = 0;

  uint16_t heapOffset = getSectionOffset(vm->lpBytecode, BCS_HEAP);
  uint16_t heapSize = getHeapSize(vm);

  // This assumes that the heap is the last section in the bytecode. Since the
  // heap is the only part of the bytecode image that changes size, we can just
  // calculate the new bytecode size as follows
  VM_ASSERT(vm, BCS_HEAP == BCS_SECTION_COUNT - 1);
  uint32_t bytecodeSize = (uint32_t)heapOffset + heapSize;

  if (bytecodeSize > 0xFFFF) {
    CODE_COVERAGE_ERROR_PATH(584); // Not hit
    MVM_FATAL_ERROR(vm, MVM_E_SNAPSHOT_TOO_LARGE);
  } else {
    CODE_COVERAGE(585); // Hit
  }

  mvm_TsBytecodeHeader* pNewBytecode = vm_malloc(vm, bytecodeSize);
  if (!pNewBytecode) return NULL;

  // The globals and heap are the last parts of the image because they're the
  // only mutable sections
  VM_ASSERT(vm, BCS_GLOBALS == BCS_SECTION_COUNT - 2);
  uint16_t sizeOfConstantPart = getSectionOffset(vm->lpBytecode, BCS_GLOBALS);

  // The first part of the snapshot doesn't change between executions (except
  // some header fields, which we'll update later).
  memcpy_long(pNewBytecode, vm->lpBytecode, sizeOfConstantPart);

  // Snapshot the globals memory
  uint16_t sizeOfGlobals = getSectionSize(vm, BCS_GLOBALS);
  memcpy((uint8_t*)pNewBytecode + pNewBytecode->sectionOffsets[BCS_GLOBALS], vm->globals, sizeOfGlobals);

  // Snapshot heap memory

  TsBucket* pBucket = vm->pLastBucket;
  // Start at the end of the heap and work backwards, because buckets are linked
  // in reverse order. (Edit: actually, they're also linked forwards now, but I
  // might retract that at some point so I'll leave this with the backwards
  // iteration).
  uint8_t* pHeapStart = (uint8_t*)pNewBytecode + pNewBytecode->sectionOffsets[BCS_HEAP];
  uint8_t* pTarget = pHeapStart + heapSize;
  uint16_t cursor = heapSize;
  TABLE_COVERAGE(pBucket ? 1 : 0, 2, 586); // Hit 2/2
  while (pBucket) {
    CODE_COVERAGE(504); // Hit
    uint16_t offsetStart = pBucket->offsetStart;
    uint16_t bucketSize = cursor - offsetStart;
    uint8_t* pBucketData = getBucketDataBegin(pBucket);

    pTarget -= bucketSize;
    memcpy(pTarget, pBucketData, bucketSize);

    cursor = offsetStart;
    pBucket = pBucket->prev;
  }

  // Update header fields
  pNewBytecode->bytecodeSize = bytecodeSize;

  // Convert pointers-to-RAM into their corresponding serialized form
  serializePointers(vm, pNewBytecode);

  uint16_t crcStartOffset = OFFSETOF(mvm_TsBytecodeHeader, crc) + sizeof pNewBytecode->crc;
  uint16_t crcSize = bytecodeSize - crcStartOffset;
  void* pCrcStart = (uint8_t*)pNewBytecode + crcStartOffset;
  pNewBytecode->crc = MVM_CALC_CRC16_CCITT(pCrcStart, crcSize);

  if (out_size) {
    CODE_COVERAGE(587); // Hit
    *out_size = bytecodeSize;
  }
  return (void*)pNewBytecode;
}
#endif // MVM_INCLUDE_SNAPSHOT_CAPABILITY

#if MVM_INCLUDE_DEBUG_CAPABILITY

void mvm_dbg_setBreakpoint(VM* vm, int bytecodeAddress) {
  CODE_COVERAGE_UNTESTED(588); // Not hit

  // These checks on the bytecode address are assertions rather than user faults
  // because the address is probably not manually computed by a user, it's
  // derived from some kind of debug symbol file. In a production environment,
  // setting a breakpoint on an address that's never executed (e.g. because it's
  // not executable) is not a VM failure.
  VM_ASSERT(vm, (bytecodeAddress == - 1) || (bytecodeAddress >= getSectionOffset(vm->lpBytecode, BCS_ROM)));
  VM_ASSERT(vm, (bytecodeAddress == -1) || (bytecodeAddress < getSectionOffset(vm->lpBytecode, vm_sectionAfter(vm, BCS_ROM))));

  mvm_dbg_removeBreakpoint(vm, bytecodeAddress);
  TsBreakpoint* breakpoint = vm_malloc(vm, sizeof (TsBreakpoint));
  if (!breakpoint) {
    MVM_FATAL_ERROR(vm, MVM_E_MALLOC_FAIL);
    return;
  }
  breakpoint->bytecodeAddress = bytecodeAddress;
  // Add to linked-list
  breakpoint->next = vm->pBreakpoints;
  vm->pBreakpoints = breakpoint;
}

void mvm_dbg_removeBreakpoint(VM* vm, uint16_t bytecodeAddress) {
  CODE_COVERAGE_UNTESTED(589); // Not hit

  TsBreakpoint** ppBreakpoint = &vm->pBreakpoints;
  TsBreakpoint* pBreakpoint = *ppBreakpoint;
  while (pBreakpoint) {
    if (pBreakpoint->bytecodeAddress == bytecodeAddress) {
      CODE_COVERAGE_UNTESTED(590); // Not hit
      // Remove from linked list
      *ppBreakpoint = pBreakpoint->next;
      vm_free(vm, pBreakpoint);
      pBreakpoint = *ppBreakpoint;
    } else {
      CODE_COVERAGE_UNTESTED(591); // Not hit
      ppBreakpoint = &pBreakpoint->next;
      pBreakpoint = *ppBreakpoint;
    }
  }
}

void mvm_dbg_setBreakpointCallback(mvm_VM* vm, mvm_TfBreakpointCallback cb) {
  CODE_COVERAGE_UNTESTED(592); // Not hit
  // It doesn't strictly need to be null, but is probably a mistake if it's not.
  VM_ASSERT(vm, vm->breakpointCallback == NULL);
  vm->breakpointCallback = cb;
}

#endif // MVM_INCLUDE_DEBUG_CAPABILITY

/**
 * Test out the LONG_PTR macros provided in the port file. lpBytecode should
 * point to actual bytecode, whereas pHeader should point to a local copy that's
 * been validated.
 */
static TeError vm_validatePortFileMacros(MVM_LONG_PTR_TYPE lpBytecode, mvm_TsBytecodeHeader* pHeader, void* context) {
  uint32_t x1 = 0x12345678;
  uint32_t x2 = 0x12345678;
  uint32_t x3 = 0x87654321;
  uint32_t x4 = 0x99999999;
  uint32_t* px1 = &x1;
  uint32_t* px2 = &x2;
  uint32_t* px3 = &x3;
  uint32_t* px4 = &x4;
  MVM_LONG_PTR_TYPE lpx1 = MVM_LONG_PTR_NEW(px1);
  MVM_LONG_PTR_TYPE lpx2 = MVM_LONG_PTR_NEW(px2);
  MVM_LONG_PTR_TYPE lpx3 = MVM_LONG_PTR_NEW(px3);
  MVM_LONG_PTR_TYPE lpx4 = MVM_LONG_PTR_NEW(px4);

  if (!((MVM_LONG_PTR_TRUNCATE(lpx1)) == px1)) goto SUB_FAIL;
  if (!((MVM_READ_LONG_PTR_1(lpx1)) == 0x78)) goto SUB_FAIL;
  if (!((MVM_READ_LONG_PTR_2(lpx1)) == 0x5678)) goto SUB_FAIL;
  if (!((MVM_READ_LONG_PTR_1((MVM_LONG_PTR_ADD(lpx1, 1)))) == 0x56)) goto SUB_FAIL;
  if (!((MVM_LONG_PTR_SUB((MVM_LONG_PTR_ADD(lpx1, 3)), lpx1)) == 3)) goto SUB_FAIL;
  if (!((MVM_LONG_PTR_SUB(lpx1, (MVM_LONG_PTR_ADD(lpx1, 3)))) == -3)) goto SUB_FAIL;
  if (!((MVM_LONG_MEM_CMP(lpx1, lpx2, 4)) == 0)) goto SUB_FAIL;
  if (!((MVM_LONG_MEM_CMP(lpx1, lpx3, 4)) > 0)) goto SUB_FAIL;
  if (!((MVM_LONG_MEM_CMP(lpx1, lpx4, 4)) < 0)) goto SUB_FAIL;

  MVM_LONG_MEM_CPY(px4, lpx3, 4);
  if (!(x4 == 0x87654321)) goto SUB_FAIL;
  x4 = 0x99999999;

  // The above tests were testing the case of using a long pointer to point to
  // local RAM. We need to also test that everything works when point to the
  // actual bytecode. lpBytecode and pHeader should point to data of the same
  // value but in different address spaces (ROM and RAM respectively).

  if (!((MVM_READ_LONG_PTR_1(lpBytecode)) == pHeader->bytecodeVersion)) goto SUB_FAIL;
  if (!((MVM_READ_LONG_PTR_2(lpBytecode)) == *((uint16_t*)pHeader))) goto SUB_FAIL;
  if (!((MVM_READ_LONG_PTR_1((MVM_LONG_PTR_ADD(lpBytecode, 2)))) == pHeader->requiredEngineVersion)) goto SUB_FAIL;
  if (!((MVM_LONG_PTR_SUB((MVM_LONG_PTR_ADD(lpBytecode, 3)), lpBytecode)) == 3)) goto SUB_FAIL;
  if (!((MVM_LONG_PTR_SUB(lpBytecode, (MVM_LONG_PTR_ADD(lpBytecode, 3)))) == -3)) goto SUB_FAIL;
  if (!((MVM_LONG_MEM_CMP(lpBytecode, (MVM_LONG_PTR_NEW(pHeader)), 8)) == 0)) goto SUB_FAIL;

  if (MVM_NATIVE_POINTER_IS_16_BIT && (sizeof(void*) != 2)) return MVM_E_EXPECTED_POINTER_SIZE_TO_BE_16_BIT;
  if ((!MVM_NATIVE_POINTER_IS_16_BIT) && (sizeof(void*) == 2)) return MVM_E_EXPECTED_POINTER_SIZE_NOT_TO_BE_16_BIT;

  #if MVM_USE_SINGLE_RAM_PAGE
    void* ptr = MVM_CONTEXTUAL_MALLOC(2, context);
    MVM_CONTEXTUAL_FREE(ptr, context);
    if ((intptr_t)ptr - (intptr_t)MVM_RAM_PAGE_ADDR > 0xffff) return MVM_E_MALLOC_NOT_WITHIN_RAM_PAGE;
  #endif // MVM_USE_SINGLE_RAM_PAGE

  return MVM_E_SUCCESS;

SUB_FAIL:
  return MVM_E_PORT_FILE_MACRO_TEST_FAILURE;
}

uint16_t mvm_getCurrentAddress(VM* vm) {
  vm_TsStack* stack = vm->stack;
  if (!stack) return 0; // Not currently running
  LongPtr lpProgramCounter = stack->reg.lpProgramCounter;
  LongPtr lpBytecode = vm->lpBytecode;
  uint16_t address = (uint16_t)MVM_LONG_PTR_SUB(lpProgramCounter, lpBytecode);
  return address;
}

// Clone a fixed length array or other container type
static Value vm_cloneContainer(VM* vm, Value* pArr) {
  VM_ASSERT_NOT_USING_CACHED_REGISTERS(vm);

  LongPtr* lpSource = DynamicPtr_decode_long(vm, *pArr);
  uint16_t headerWord = readAllocationHeaderWord_long(lpSource);
  uint16_t size = vm_getAllocationSizeExcludingHeaderFromHeaderWord(headerWord);
  uint16_t* newArray = mvm_allocate(vm, size, vm_getTypeCodeFromHeaderWord(headerWord));

  // May have moved during allocation
  lpSource = DynamicPtr_decode_long(vm, *pArr);

  uint16_t* pTarget = newArray;
  while (size) {
    *pTarget++ = LongPtr_read2_aligned(lpSource);
    lpSource = LongPtr_add(lpSource, 2);
    size -= 2;
  }

  return ShortPtr_encode(vm, newArray);
}

static Value vm_safePop(VM* vm, Value* pStackPointerAfterDecr) {
  // This is only called in the run-loop, so the registers should be cached
  VM_ASSERT(vm, vm->stack->reg.usingCachedRegisters);
  if (pStackPointerAfterDecr < getBottomOfStack(vm->stack)) {
    MVM_FATAL_ERROR(vm, MVM_E_ASSERTION_FAILED);
  }
  return *pStackPointerAfterDecr;
}

static inline void mvm_checkValueAccess(VM* vm, uint8_t potentialCycleNumber) {
  VM_ASSERT(vm, vm->gc_potentialCycleNumber == potentialCycleNumber);
}

static TeError vm_newError(VM* vm, TeError err) {
  #if MVM_ALL_ERRORS_FATAL
  MVM_FATAL_ERROR(vm, err);
  #endif
  return err;
}

static void* vm_malloc(VM* vm, size_t size) {
  void* result = MVM_CONTEXTUAL_MALLOC(size, vm->context);

  #if MVM_SAFE_MODE && MVM_USE_SINGLE_RAM_PAGE
    // See comment on MVM_RAM_PAGE_ADDR in microvium_port_example.h
    VM_ASSERT(vm, (intptr_t)result - (intptr_t)MVM_RAM_PAGE_ADDR <= 0xFFFF);
  #endif
  return result;
}

// This is because we get an unused warning on the `context` variable if the
// MVM_CONTEXTUAL_FREE macro doesn't actually use the context.
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif
// Note: mvm_free frees the VM, while vm_free is the counterpart to vm_malloc
static void vm_free(VM* vm, void* ptr) {
  // Capture the context before freeing the ptr, since the pointer could be the vm
  void* context = vm->context;

  #if MVM_SAFE_MODE && MVM_USE_SINGLE_RAM_PAGE
    // See comment on MVM_RAM_PAGE_ADDR in microvium_port_example.h
    VM_ASSERT(vm, !ptr || ((intptr_t)ptr - (intptr_t)MVM_RAM_PAGE_ADDR <= 0xFFFF));
  #endif

  MVM_CONTEXTUAL_FREE(ptr, context);
}
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

static mvm_TeError vm_uint8ArrayNew(VM* vm, Value* slot) {
  CODE_COVERAGE(344); // Hit

  uint16_t size = *slot;
  if (!Value_isVirtualUInt12(size)) {
    CODE_COVERAGE_ERROR_PATH(345); // Not hit
    return vm_newError(vm, MVM_E_INVALID_UINT8_ARRAY_LENGTH);
  }
  size = VirtualInt14_decode(vm, size);

  uint8_t* p = mvm_allocate(vm, size, TC_REF_UINT8_ARRAY);
  *slot = ShortPtr_encode(vm, p);
  memset(p, 0, size);

  return MVM_E_SUCCESS;
}

mvm_Value mvm_uint8ArrayFromBytes(mvm_VM* vm, const uint8_t* data, size_t sizeBytes) {
  CODE_COVERAGE(346); // Hit
  if (sizeBytes >= (MAX_ALLOCATION_SIZE + 1)) {
    MVM_FATAL_ERROR(vm, MVM_E_ALLOCATION_TOO_LARGE);
    return VM_VALUE_UNDEFINED;
  }
  // Note: mvm_allocate will also check the size
  uint8_t* p = mvm_allocate(vm, (uint16_t)sizeBytes, TC_REF_UINT8_ARRAY);
  Value result = ShortPtr_encode(vm, p);
  memcpy(p, data, sizeBytes);
  return result;
}

mvm_TeError mvm_uint8ArrayToBytes(mvm_VM* vm, mvm_Value uint8ArrayValue, uint8_t** out_data, size_t* out_size) {
  CODE_COVERAGE(348); // Hit

  // Note: while it makes sense to allow Uint8Arrays in general to live in ROM,
  // I think we can require that those that hit the FFI boundary are never
  // optimized into ROM. For efficiency and because I imagine that it's a very
  // limited use case to have constant data accessed through this API.

  if (!Value_isShortPtr(uint8ArrayValue)) {
    CODE_COVERAGE_ERROR_PATH(574); // Not hit
    return vm_newError(vm, MVM_E_TYPE_ERROR);
  }

  void* p = ShortPtr_decode(vm, uint8ArrayValue);
  uint16_t headerWord = readAllocationHeaderWord(p);
  TeTypeCode typeCode = vm_getTypeCodeFromHeaderWord(headerWord);
  if (typeCode != TC_REF_UINT8_ARRAY) {
    CODE_COVERAGE_ERROR_PATH(575); // Not hit
    return vm_newError(vm, MVM_E_TYPE_ERROR);
  }

  *out_size = (size_t)vm_getAllocationSizeExcludingHeaderFromHeaderWord(headerWord);
  *out_data = p;
  return MVM_E_SUCCESS;
}

/**
 * The internal version of asyncStart.
 *
 * Differs in the following ways:
 *
 * - `vm_asyncStartUnsafe` doesn't add an additional wrapper to the callback.
 * - `vm_asyncStartUnsafe` may return a promise value, whereas `mvm_asyncStart`
 *   will wrap that promise in a callback.
 */
static mvm_Value vm_asyncStartUnsafe(mvm_VM* vm, mvm_Value* out_result) {
  CODE_COVERAGE(743); // Not hit
  VM_ASSERT_NOT_USING_CACHED_REGISTERS(vm);

  #if MVM_SAFE_MODE
    if (!vm || !vm->stack) MVM_FATAL_ERROR(vm, MVM_E_REQUIRES_ACTIVE_VM);
  #endif
  vm_TsRegisters* reg = &vm->stack->reg;


  Value cpsCallback = reg->cpsCallback;
  // Mark that the callback has been "consumed". This is not strictly
  // necessary but adds a layer of safety because it could indicate a mistake
  // if `mvm_asyncStart` is called multiple times (especially since the
  // callback should only be called exactly once).
  reg->cpsCallback = VM_VALUE_DELETED;

  mvm_TeType tc = mvm_typeOf(vm, cpsCallback);

  // The callback is a continuation function (optimized hot-path)
  if (tc == VM_T_FUNCTION) {
    CODE_COVERAGE(744); // Not hit
    // Else, the callback will be a function. This path indicates the situation
    // where the caller supports CPS and has given the callee the callback via
    // the `cpsCallback` register.
    VM_ASSERT(vm, mvm_typeOf(vm, cpsCallback) == VM_T_FUNCTION);
    // The synchronous result (the promise) is elided because the caller
    // communicated that they support CPS
    *out_result = VM_VALUE_DELETED;

    return cpsCallback;
  }

  // Void call - no callback so we return `VM_VALUE_NO_OP_FUNC`
  if (reg->argCountAndFlags & AF_VOID_CALLED) {
    // The callback is undefined if the caller is void-calling
    VM_ASSERT(vm, cpsCallback == VM_VALUE_UNDEFINED);

    // This path indicates the situation where the caller is a void call and
    // does not need the promise result.
    CODE_COVERAGE(658); // Hit

    // This is not strictly necessary because the synchronous result is not used
    // in a void call, but it's consistent.
    *out_result = VM_VALUE_DELETED;

    // In this situation, there's nothing actually waiting to be called back
    // (the JS code is not awaiting the result of the host call), but we return
    // a dummy function so that the API is consistent.
    return VM_VALUE_NO_OP_FUNC;
  }

  if (cpsCallback == VM_VALUE_DELETED) {
    // This path indicates the situation where the callback for the current
    // activation record is no longer accessible, either because of a nested
    // function call or because the host already called `mvm_asyncStart`.
    CODE_COVERAGE_ERROR_PATH(745); // Not hit
    MVM_FATAL_ERROR(vm, MVM_E_ASYNC_START_ERROR);
    return 0;
  }

  // Otherwise, the caller does not support CPS (the caller is not a void call
  // and not an await-call) and so is expecting a promise result. We need to
  // instantiate a promise and then create a closure callback that resolves the
  // promise.
  CODE_COVERAGE(746); // Not hit

  VM_ASSERT(vm, cpsCallback == VM_VALUE_UNDEFINED);
  Value promiseProto = getBuiltin(vm, BIN_PROMISE_PROTOTYPE);
  if (promiseProto == VM_VALUE_UNDEFINED) {
    MVM_FATAL_ERROR(vm, MVM_E_ASYNC_WITHOUT_AWAIT);
  }
  VM_ASSERT(vm, deepTypeOf(vm, promiseProto) == TC_REF_PROPERTY_LIST);
  Value promise = vm_objectCreate(vm, promiseProto, 2);
  Value* pPromise = (Value*)ShortPtr_decode(vm, promise);
  // Internal slots
  pPromise[VM_OIS_PROMISE_STATUS] = VM_PROMISE_STATUS_PENDING;
  pPromise[VM_OIS_PROMISE_OUT] = VM_VALUE_UNDEFINED; // No subscribers yet

  // Note: both the synchronous result and callback are represented by the
  // promise in this scenario. As the "callback", the promise essentially
  // represents the group of subscribers to invoke.
  *out_result = promise; // The promise to put in var[0] to return to the caller of the async function
  return promise; // The promise to put in slot[1] of the closure to invoke when the async operation completes
}

/**
 * Create a new object with the given prototype and number of internal slots.
 *
 * internalSlotCount must be a multiple of 2 because it's overloading the
 * key/value pairs of the object.
 *
 * @warning It does NOT initialize the internal slots.
 */
static Value vm_objectCreate(VM* vm, Value prototype, int internalSlotCount) {
  VM_ASSERT(vm, (internalSlotCount % 2) == 0);
  size_t size = sizeof(TsPropertyList) + internalSlotCount * sizeof(Value);
  vm_push(vm, prototype); // GC reachable
  TsPropertyList* pObject = mvm_allocate(vm, (uint16_t)size, TC_REF_PROPERTY_LIST);
  pObject->dpProto = vm_pop(vm); // prototype
  pObject->dpNext = VM_VALUE_NULL;

  return ShortPtr_encode(vm, pObject);
}

// Same as vm_asyncStartUnsafe but adds an additional wrapper closure
mvm_Value mvm_asyncStart(mvm_VM* vm, mvm_Value* out_result) {
  mvm_Value callbackOrPromise = vm_asyncStartUnsafe(vm, out_result);

  if (callbackOrPromise == VM_VALUE_NO_OP_FUNC) {
    CODE_COVERAGE(702); // Hit
    return VM_VALUE_NO_OP_FUNC;
  }

  mvm_Value asyncHostCallback = getBuiltin(vm, BIN_ASYNC_HOST_CALLBACK);
  CODE_COVERAGE(695); // Hit
  if (asyncHostCallback == VM_VALUE_UNDEFINED) {
    CODE_COVERAGE_UNTESTED(703); // Not hit

    // If the builtin is missing, it means the compiler found no await points
    // in the program and assumed that the async machinery was not needed.
    MVM_FATAL_ERROR(vm, MVM_E_ASYNC_WITHOUT_AWAIT);
    return VM_VALUE_NO_OP_FUNC;
  }

  CODE_COVERAGE(704); // Hit

  // Anchor on stack
  vm_push(vm, callbackOrPromise);

  uint16_t* pClosure = mvm_allocate(vm, 4, TC_REF_CLOSURE);
  pClosure[0] = asyncHostCallback;
  pClosure[1] = vm_pop(vm); // callbackOrPromise
  mvm_Value closureValue = ShortPtr_encode(vm, pClosure);

  return closureValue;
}

static mvm_Value* vm_push(mvm_VM* vm, mvm_Value value) {
  VM_ASSERT_NOT_USING_CACHED_REGISTERS(vm);
  VM_ASSERT(vm, vm && vm->stack);
  vm_TsRegisters* reg = &vm->stack->reg;
  VM_ASSERT(vm, reg->pStackPointer < getTopOfStackSpace(vm->stack));
  mvm_Value* result = reg->pStackPointer++;
  *result = value;
  return result;
}

static mvm_Value vm_pop(mvm_VM* vm) {
  VM_ASSERT_NOT_USING_CACHED_REGISTERS(vm);
  VM_ASSERT(vm, vm && vm->stack);
  vm_TsRegisters* reg = &vm->stack->reg;
  VM_ASSERT(vm, reg->pStackPointer > getBottomOfStack(vm->stack));
  return *--reg->pStackPointer;
}

/**
 * Enqueue the given job to the job queue (for the moment there is only one job
 * queue, for executing async callbacks). The job must be of type TC_REF_CLOSURE
 */
static void vm_enqueueJob(VM* vm, Value jobClosure) {
  Value* firstNode;
  Value firstNodeRef;

  CODE_COVERAGE(672); // Hit

  // The job queue exists in the ephemeral registers. There is no way to enqueue
  // job while the VM is idle (no stack). But obviously you can call a VM
  // function that triggers a job to be enqueued
  VM_ASSERT(vm, vm->stack);
  VM_ASSERT_NOT_USING_CACHED_REGISTERS(vm);
  vm_TsRegisters* reg = &vm->stack->reg;
  Value jobQueue = reg->jobQueue;

  // Note: jobs are always closures
  VM_ASSERT(vm, deepTypeOf(vm, jobClosure) == TC_REF_CLOSURE);

  TeTypeCode type = deepTypeOf(vm, jobQueue);

  // Hot path (I think)
  if (type == TC_VAL_UNDEFINED) {
    CODE_COVERAGE(673); // Hit
    // No jobs yet - the new job is the only job
    reg->jobQueue = jobClosure;
    return;
  }

  vm_push(vm, jobClosure); // GC-reachable

  // Note: jobs are always closures
  if (type == TC_REF_CLOSURE) {
    CODE_COVERAGE(674); // Hit

    // There is already one job. We need to promote the queue to a linked list
    // (cycle). Each element in the linked cycle is a triple with [prev, job,
    // next]. Here there is only one node in the cycle, so the next and prev are
    // itself.
    firstNode = (Value*)mvm_allocate(vm, 2 * 3, TC_REF_FIXED_LENGTH_ARRAY);
    firstNodeRef = ShortPtr_encode(vm, firstNode);
    firstNode[0] = firstNodeRef; // prev
    firstNode[1] = reg->jobQueue; // job
    firstNode[2] = firstNodeRef; // next
    reg->jobQueue = firstNodeRef;
    VM_EXEC_SAFE_MODE(jobQueue = VM_VALUE_DELETED); // Invalidated
    VM_EXEC_SAFE_MODE(type = 0); // Invalidated
    /* no return */
  } else {
    CODE_COVERAGE(675); // Hit
  }

  // If it's not undefined or a closure, it must be a linked list (linked cycle)
  // of jobs.
  VM_ASSERT(vm, deepTypeOf(vm, reg->jobQueue) == TC_REF_FIXED_LENGTH_ARRAY);

  // Create a new node in the linked cycle
  Value* newNode = mvm_allocate(vm, 2 * 3, TC_REF_FIXED_LENGTH_ARRAY);
  VM_EXEC_SAFE_MODE(firstNodeRef = VM_VALUE_DELETED); // Invalidated
  VM_EXEC_SAFE_MODE(firstNode = 0); // Invalidated
  VM_EXEC_SAFE_MODE(jobClosure = VM_VALUE_DELETED); // Invalidated

  // Note: the job queue is always in RAM.
  firstNodeRef = reg->jobQueue;
  firstNode = ShortPtr_decode(vm, firstNodeRef);

  // We insert the new job at the "end" of the list. Since the list is actually
  // a cycle, this means inserting it before the first node. This is the main
  // reason we store this as a cycle rather than a flat list -- it gives us
  // access to the last node of the list without using another register.
  Value lastNodeRef = firstNode[0] /* prev */;
  Value* lastNode = ShortPtr_decode(vm, lastNodeRef);

  Value newNodeRef = ShortPtr_encode(vm, newNode);
  newNode[0] = lastNodeRef;  // prev
  newNode[1] = vm_pop(vm) /* jobClosure */; // job
  newNode[2] = firstNodeRef; // next
  lastNode[2] = newNodeRef;  // last.next
  firstNode[0] = newNodeRef; // first.prev
}

/**
 * Dequeues the first job from the job queue and returns it.
 *
 * WARNING: the result is not otherwise GC reachable, so don't run a GC cycle
 * until it's anchored to the reachability graph.
 *
 * WARNING: this should only be called if there is an actual job (i.e. the queue
 * register is not VM_VALUE_UNDEFINED). This function doesn't handle that case
 * because it's expected to be the hot case.
 */
static Value vm_dequeueJob(VM* vm) {
  CODE_COVERAGE(676); // Hit

  // The job queue exists in the ephemeral registers. There is no way to enqueue
  // job while the VM is idle (no stack). But obviously you can call a VM
  // function that triggers a job to be enqueued
  VM_ASSERT(vm, vm->stack);
  VM_ASSERT_NOT_USING_CACHED_REGISTERS(vm);

  vm_TsRegisters* reg = &vm->stack->reg;
  Value jobQueue = reg->jobQueue;

  // Caller should check if there isn't a job first (hot path)
  VM_ASSERT(vm, reg->jobQueue != VM_VALUE_UNDEFINED);

  TeTypeCode tc = deepTypeOf(vm, jobQueue);

  // Note: jobs are only closures (not other callable types)
  if (tc == TC_REF_CLOSURE) {
    CODE_COVERAGE(677); // Hit
    reg->jobQueue = VM_VALUE_UNDEFINED;
    return jobQueue;
  }

  // Otherwise the queue is a linked cycle (see vm_enqueueJob). Each node in the
  // cycle is a triple of [prev, job, next]
  VM_ASSERT(vm, tc == TC_REF_FIXED_LENGTH_ARRAY);
  Value* first = ShortPtr_decode(vm, jobQueue);

  // First job in the queue
  Value result = first[1] /* job */;

  // Cycle of 1? Then this dequeue empties the queue
  if (ShortPtr_decode(vm, first[0] /* prev */) == first) {
    CODE_COVERAGE(678); // Hit
    VM_ASSERT(vm, first[0] == jobQueue);
    reg->jobQueue = VM_VALUE_UNDEFINED; // Job queue is empty
    VM_ASSERT(vm, deepTypeOf(vm, first[1]) == TC_REF_CLOSURE);
    return result;
  } else {
    CODE_COVERAGE(679); // Hit
    // Warning: `second` might be the same as `last` if there are only 2 cells in the cycle
    Value* last = ShortPtr_decode(vm, first[0]);
    Value* second = ShortPtr_decode(vm, first[2]);
    last[2] /* next */ = first[2] /* next */;
    second[0] /* prev */ = first[0] /* prev */;
    reg->jobQueue = first[2];
    return result;
  }
}

/**
 * Checks the Microvium heap for corruption.
 *
 * This function walks the heap and checks that all RAM pointer values point to
 * legitimate allocations.
 */
#if MVM_DEBUG_UTILS
void mvm_checkHeap(mvm_VM* vm) {
  // Allocation map is 1 bit per word. The bit is set if the word is the
  // beginning of an allocation.
  uint16_t heapSize = getHeapSize(vm);
  uint8_t* allocationMap = (uint8_t*)malloc((heapSize + 15) / 16);
  memset(allocationMap, 0, (heapSize + 15) / 16);
  #define setAllocationMapBit(address) (allocationMap[(address) / 16] |=  (1 << ((address) % 16 / 2)))
  #define getAllocationMapBit(address) ((allocationMap[(address) / 16] & (1 << ((address) % 16 / 2))) != 0)

  TsBucket* firstBucket = vm->pLastBucket;
  while (firstBucket && firstBucket->prev) {
    firstBucket = firstBucket->prev;
  }

  // 1st pass to find allocations
  TsBucket* bucket = firstBucket;
  while (bucket) {
    uint8_t* bucketBegin = (uint8_t*)getBucketDataBegin(bucket);
    Value* p = (Value*)bucketBegin;
    Value* bucketEnd = bucket->pEndOfUsedSpace;
    uint16_t offsetStart = bucket->offsetStart;
    while (p < bucketEnd) {
      uint16_t header = *p++;
      uint16_t offset = (uint16_t)((uint8_t*)p - bucketBegin) + offsetStart;
      setAllocationMapBit(offset);
      uint16_t size = vm_getAllocationSizeExcludingHeaderFromHeaderWord(header);
      Value* next = p + ((size + 1) / 2);
      if (next > bucketEnd) {
        MVM_FATAL_ERROR(vm, MVM_E_HEAP_CORRUPT);
        goto cleanup;
      }
      p = next;
    }
    bucket = bucket->next;
  }

  // 2st pass to check pointers
  bucket = firstBucket;
  while (bucket) {
    uint8_t* bucketBegin = (uint8_t*)getBucketDataBegin(bucket);
    Value* p = (Value*)bucketBegin;
    Value* bucketEnd = bucket->pEndOfUsedSpace;
    while (p < bucketEnd) {
      uint16_t header = *p++;
      Value* pAlloc = p;
      uint16_t size = vm_getAllocationSizeExcludingHeaderFromHeaderWord(header);
      TeTypeCode tc = vm_getTypeCodeFromHeaderWord(header);
      Value* next = pAlloc + ((size + 1) / 2); // Round up
      if (next > bucketEnd) {
        MVM_FATAL_ERROR(vm, MVM_E_HEAP_CORRUPT);
      }
      if (tc >= TC_REF_DIVIDER_CONTAINER_TYPES) {
        uint16_t words = size / 2; // Round down
        for (uint16_t slotNumber = 0; slotNumber < words; slotNumber++) {
          Value value = pAlloc[slotNumber];
          if (Value_isShortPtr(value)) {
            Value* target = ShortPtr_decode(vm, value);
            uint16_t offset = pointerOffsetInHeap(vm, vm->pLastBucket, target);
            if (!getAllocationMapBit(offset)) {
              MVM_FATAL_ERROR(vm, MVM_E_HEAP_CORRUPT);
              goto cleanup;
            }
          }
        }
        p = next;
      }
      p = next;
    }
    bucket = bucket->next;
  }

cleanup:
  free(allocationMap);
}
#endif // MVM_DEBUG_UTILS

/**
 * If the value is a RAM pointer, validates that it points to a valid heap
 * allocation.
 *
 * WARNING: Very expensive, since it walks the whole heap to find the
 * allocation.
 */
#if MVM_DEBUG_UTILS
void mvm_checkValue(mvm_VM* vm, mvm_Value value) {
  if (!Value_isShortPtr(value)) {
    return;
  }

  TsBucket* firstBucket = vm->pLastBucket;
  while (firstBucket && firstBucket->prev) {
    firstBucket = firstBucket->prev;
  }

  TsBucket* bucket = firstBucket;
  while (bucket) {
    uint8_t* bucketBegin = (uint8_t*)getBucketDataBegin(bucket);
    Value* p = (Value*)bucketBegin;
    Value* bucketEnd = bucket->pEndOfUsedSpace;
    while (p < bucketEnd) {
      uint16_t header = *p++;
      if (ShortPtr_encode(vm, p) == value) {
        return; // Found
      }
      uint16_t size = vm_getAllocationSizeExcludingHeaderFromHeaderWord(header);
      Value* next = p + ((size + 1) / 2);
      if (next > bucketEnd) {
        MVM_FATAL_ERROR(vm, MVM_E_HEAP_CORRUPT);
        return;
      }
      p = next;
    }
    bucket = bucket->next;
  }

  // Not found. Therefore corrupt.
  MVM_FATAL_ERROR(vm, MVM_E_HEAP_CORRUPT);
}
#endif // MVM_DEBUG_UTILS

/**
 * Parses the call stack and returns a pointer to the set of frame.
 *
 * The first returned frame is the active frame, and the rest proceed in
 * descending order.
 *
 * If the registers are currently cached, the first frame will be rubbish.
 *
 * The memory is malloced but re-used on each call to `mvm_readCallStack`,
 * so the caller should not free it.
 *
 * A zero programCounter indicates the end of the call stack, which can be
 * useful for using this in debug watch. Also the `out_frameCount` is optional.
 *
 * If there are no frames, returns NULL.
 */
#if MVM_DEBUG_UTILS
mvm_TsCallStackFrame* mvm_readCallStack(VM* vm, int* out_frameCount) {
  static mvm_TsCallStackFrame* frames = 0;
  static int allocatedFramesCount = 0;

  vm_TsStack* stack = vm->stack;
  if (!stack) {
    if (out_frameCount) {
      *out_frameCount = 0;
    }
    return NULL;
  }
  vm_TsRegisters* reg = &stack->reg;
  uint16_t* beginningOfStack = getBottomOfStack(stack);

  // Count number of required frames
  int frameCount = 1;
  uint16_t* pFrameBase = reg->pFrameBase;
  while (pFrameBase > beginningOfStack) {
    frameCount++;
    pFrameBase -= 4;
    pFrameBase = pFrameBase - (*pFrameBase) / 2;
  }

  if (out_frameCount) {
    *out_frameCount = frameCount;
  }

  // Allocate enough space. One extra as a kind of "null terminator" for debug purposes
  if (allocatedFramesCount < frameCount + 1) {
    free(frames);
    allocatedFramesCount = frameCount + 1;
    frames = (mvm_TsCallStackFrame*)malloc(sizeof(mvm_TsCallStackFrame) * allocatedFramesCount);
  }

  memset(frames, 0, sizeof(mvm_TsCallStackFrame) * allocatedFramesCount);

  mvm_TsCallStackFrame* pFrame = frames;
  VM_ASSERT(vm, pFrame >= frames);

  Value* pStackPointer = reg->pStackPointer;
  uint16_t programCounter = (uint16_t)LongPtr_sub(reg->lpProgramCounter, vm->lpBytecode);
  Value* pArgs = reg->pArgs;
  uint16_t argCountAndFlags = reg->argCountAndFlags;
  uint16_t argCount = (argCountAndFlags & AF_ARG_COUNT_MASK);
  Value closure = reg->closure;

  pFrameBase = reg->pFrameBase;
  while (true) {
    VM_ASSERT(vm, pFrame >= frames);
    VM_ASSERT(vm, pFrame < frames + frameCount);

    pFrame->programCounter = programCounter;
    pFrame->frameBase = pFrameBase;
    pFrame->frameDepth = (int)(pStackPointer - pFrameBase);
    pFrame->argCount = argCount;
    pFrame->args = pArgs;
    pFrame->closure = closure == VM_VALUE_UNDEFINED ? NULL : ShortPtr_decode(vm, closure);
    pFrame->closureSlotCount = closure == VM_VALUE_UNDEFINED ? 0 : vm_getAllocationSize(ShortPtr_decode(vm, closure)) / 2;

    pFrame++;

    // Unwind frame
    VM_ASSERT(vm, VM_FRAME_BOUNDARY_VERSION == 2);
    pStackPointer = pFrameBase;
    if (pStackPointer <= beginningOfStack) {
      break;
    }

    programCounter = *(--pStackPointer);
    argCountAndFlags = *(--pStackPointer);
    argCount = (argCountAndFlags & AF_ARG_COUNT_MASK);
    closure = *(--pStackPointer);
    pStackPointer--;
    pFrameBase = (uint16_t*)((uint8_t*)pStackPointer - *pStackPointer);
    pArgs = pFrameBase - VM_FRAME_BOUNDARY_SAVE_SIZE_WORDS - argCount;

  }

  VM_ASSERT(vm, pFrameBase == beginningOfStack);
  VM_ASSERT(vm, pFrame == frames + frameCount);

  return frames;
}
#endif // MVM_DEBUG_UTILS

/**
 * Counts the number of allocations in the heap
 *
 * WARNING: Expensive, since it walks the whole heap.
 */
#if MVM_DEBUG_UTILS
int mvm_readHeapCount(VM* vm) {
  TsBucket* firstBucket = vm->pLastBucket;
  while (firstBucket && firstBucket->prev) {
    firstBucket = firstBucket->prev;
  }

  TsBucket* bucket = firstBucket;
  int count = 0;
  while (bucket) {
    uint8_t* bucketBegin = (uint8_t*)getBucketDataBegin(bucket);
    Value* p = (Value*)bucketBegin;
    Value* bucketEnd = bucket->pEndOfUsedSpace;
    while (p < bucketEnd) {
      uint16_t header = *p++;
      uint16_t size = vm_getAllocationSizeExcludingHeaderFromHeaderWord(header);
      count++;

      Value* next = p + ((size + 1) / 2);
      if (next > bucketEnd) {
        MVM_FATAL_ERROR(vm, MVM_E_HEAP_CORRUPT);
        return 0;
      }
      p = next;
    }
    bucket = bucket->next;
  }

  return count;
}
#endif // MVM_DEBUG_UTILS

/**
 * Parse the whole heap. Will return a pointer to the heap allocation info
 * array, which is malloced and re-used on each call to `mvm_readHeap`, so the
 * caller should not free it.
 *
 * The `out_count` is optional. The number of returned heap allocations is N+1
 * with the last one being a sentinel with all fields zeroed.
 */
#if MVM_DEBUG_UTILS
mvm_TsHeapAllocationInfo* mvm_readHeap(VM* vm, int* out_count) {
  static mvm_TsHeapAllocationInfo* heap = 0;
  static int allocatedHeapCount = 0;

  if (out_count) {
    *out_count = -1; // Initial value. Will be overridden.
  }

  if (heap) {
    free(heap);
    heap = 0;
    allocatedHeapCount = 0;
  }

  int count = mvm_readHeapCount(vm);
  allocatedHeapCount = count + 1;
  heap = (mvm_TsHeapAllocationInfo*)malloc(sizeof(mvm_TsHeapAllocationInfo) * allocatedHeapCount);
  if (!heap) return heap;
  memset(heap, 0, sizeof(mvm_TsHeapAllocationInfo) * allocatedHeapCount);

  TsBucket* firstBucket = vm->pLastBucket;
  while (firstBucket && firstBucket->prev) {
    firstBucket = firstBucket->prev;
  }

  TsBucket* bucket = firstBucket;
  int i = 0;
  while (bucket) {
    uint8_t* bucketBegin = (uint8_t*)getBucketDataBegin(bucket);
    Value* p = (Value*)bucketBegin;
    Value* bucketEnd = bucket->pEndOfUsedSpace;
    uint16_t offsetStart = bucket->offsetStart;
    while (p < bucketEnd) {
      uint16_t header = *p++;
      uint16_t offset = (uint16_t)((uint8_t*)p - bucketBegin) + offsetStart;
      uint16_t size = vm_getAllocationSizeExcludingHeaderFromHeaderWord(header);
      TeTypeCode tc = vm_getTypeCodeFromHeaderWord(header);

      VM_ASSERT(vm, i < allocatedHeapCount - 1);
      heap[i++] = (mvm_TsHeapAllocationInfo){
        .a = (uint16_t)((intptr_t)p),
        .t = tc,
        .s = size,
        .offset = offset,
        .address = p,
      };

      Value* next = p + ((size + 1) / 2);
      if (next > bucketEnd) {
        MVM_FATAL_ERROR(vm, MVM_E_HEAP_CORRUPT);
        return NULL;
      }
      p = next;
    }
    bucket = bucket->next;
  }

  if (out_count) {
    *out_count = count;
  }

  return heap;
}
#endif // MVM_DEBUG_UTILS

#ifdef MVM_GAS_COUNTER
void mvm_stopAfterNInstructions(mvm_VM* vm, int32_t n) {
  vm->stopAfterNInstructions = n;
}

int32_t mvm_getInstructionCountRemaining(mvm_VM* vm) {
  return vm->stopAfterNInstructions;
}
#endif // MVM_GAS_COUNTER

/**
 * Subscribe a callback to a promise.
 */
MVM_HIDDEN void mvm_subscribeToPromise(VM* vm, Value vPromise, Value vCallback) {
  CODE_COVERAGE_UNTESTED(747); // Not hit
  VM_ASSERT_NOT_USING_CACHED_REGISTERS(vm);

  VM_ASSERT(vm, deepTypeOf(vm, vPromise) == TC_REF_PROPERTY_LIST);
  VM_ASSERT(vm, mvm_typeOf(vm, vCallback) == VM_T_FUNCTION);

  Value* pPromise = ShortPtr_decode(vm, vPromise);
  // Check that the promise is a promise
  VM_ASSERT(vm, pPromise[VM_OIS_PROTO] == getBuiltin(vm, BIN_PROMISE_PROTOTYPE));

  vm_TePromiseStatus promiseStatus = pPromise[VM_OIS_PROMISE_STATUS];

  if (promiseStatus == VM_PROMISE_STATUS_PENDING) {
    CODE_COVERAGE(707); // Hit

    // Subscribe to the promise
    Value vSubscribers = pPromise[VM_OIS_PROMISE_OUT];
    if (vSubscribers == VM_VALUE_UNDEFINED) {
      CODE_COVERAGE(715); // Hit
      // No subscribers yet (hot path)
      pPromise[VM_OIS_PROMISE_OUT] = vCallback;
    } else {
      CODE_COVERAGE(716); // Hit

      // Warning: the stack work here is a bit awkward but required because when
      // we call `vm_newArray` or `vm_arrayPush` it may trigger a garbage
      // collection which may move the promise object or the array itself. So we
      // need these to be GC-reachable in slots that have stable addresses (i.e.
      // stack slots).

      Value* pvCallback = vm_push(vm, vCallback);
      Value* pvPromise = vm_push(vm, vPromise);
      Value* pvSubscribers = vm_push(vm, vSubscribers);

      TeTypeCode tc = deepTypeOf(vm, vSubscribers);
      if (tc == TC_REF_CLOSURE) { // Single subscriber. Upgrade to array
        CODE_COVERAGE(717); // Hit
        Value vNewArray = vm_newArray(vm, 2); // capacity = 2; [old subscriber, new subscriber]
        Value* pvNewArray = vm_push(vm, vNewArray);
        // Put the single subscriber into the array
        vm_arrayPush(vm, pvNewArray, pvSubscribers);
        vNewArray = vm_pop(vm);
        pPromise = ShortPtr_decode(vm, *pvPromise); // May have moved
        pPromise[VM_OIS_PROMISE_OUT] = vNewArray;
        *pvSubscribers = vNewArray;
      } else { // Already an array -- nothing to do
        CODE_COVERAGE(718); // Hit
        VM_ASSERT(vm, tc == TC_REF_ARRAY);
      }
      vm_arrayPush(vm, pvSubscribers, pvCallback);
      vm_pop(vm); // vSubscribers
      vm_pop(vm); // vPromise
      vm_pop(vm); // vCallback
    }
  } else { // Resolved or rejected
    CODE_COVERAGE(708); // Hit
    VM_ASSERT(vm, (promiseStatus == VM_PROMISE_STATUS_RESOLVED) || (promiseStatus == VM_PROMISE_STATUS_REJECTED));
    TABLE_COVERAGE(promiseStatus == VM_PROMISE_STATUS_RESOLVED ? 1 : 0, 2, 709); // Hit 2/2

    Value resultOrError = pPromise[VM_OIS_PROMISE_OUT];
    Value isSuccess = (promiseStatus == VM_PROMISE_STATUS_RESOLVED) ? VM_VALUE_TRUE : VM_VALUE_FALSE;

    // Immediately schedule this async function to resume on the job queue
    vm_scheduleContinuation(vm, vCallback, isSuccess, resultOrError);
  }

  // The whole function is a potential GC point because it may allocate a new
  // array for the subscription.
  VM_POTENTIAL_GC_POINT(vm);
}