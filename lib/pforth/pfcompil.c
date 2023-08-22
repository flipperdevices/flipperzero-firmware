/* @(#) pfcompil.c 98/01/26 1.5 */
/***************************************************************
** Compiler for PForth based on 'C'
**
** These routines could be left out of an execute only version.
**
** Author: Phil Burk
** Copyright 1994 3DO, Phil Burk, Larry Polansky, David Rosenboom
**
** Permission to use, copy, modify, and/or distribute this
** software for any purpose with or without fee is hereby granted.
**
** THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
** WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
** WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL
** THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
** CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING
** FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
** CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
** OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
**
****************************************************************
** 941004 PLB Extracted IO calls from pforth_main.c
** 950320 RDG Added underflow checking for FP stack
***************************************************************/

#include "pf_all.h"
#include "pfcompil.h"

#define ABORT_RETURN_CODE   (10)
#define UINT32_MASK  ((sizeof(ucell_t)-1))

/***************************************************************/
/************** Static Prototypes ******************************/
/***************************************************************/

static void  ffStringColon( const ForthStringPtr FName );
static cell_t CheckRedefinition( const ForthStringPtr FName );
static void  ffUnSmudge( void );
static cell_t FindAndCompile( const char *theWord );
static cell_t ffCheckDicRoom( void );

#ifndef PF_NO_INIT
    static void CreateDeferredC( ExecToken DefaultXT, const char *CName );
#endif

cell_t NotCompiled( const char *FunctionName )
{
    MSG("Function ");
    MSG(FunctionName);
    MSG(" not compiled in this version of PForth.\n");
    return -1;
}

#ifndef PF_NO_SHELL
/***************************************************************
** Create an entry in the Dictionary for the given ExecutionToken.
** FName is name in Forth format.
*/
void CreateDicEntry( ExecToken XT, const ForthStringPtr FName, ucell_t Flags )
{
    cfNameLinks *cfnl;

    cfnl = (cfNameLinks *) gCurrentDictionary->dic_HeaderPtr;

/* Set link to previous header, if any. */
    if( gVarContext )
    {
        WRITE_CELL_DIC( &cfnl->cfnl_PreviousName, ABS_TO_NAMEREL( gVarContext ) );
    }
    else
    {
        cfnl->cfnl_PreviousName = 0;
    }

/* Put Execution token in header. */
    WRITE_CELL_DIC( &cfnl->cfnl_ExecToken, XT );

/* Advance Header Dictionary Pointer */
    gCurrentDictionary->dic_HeaderPtr += sizeof(cfNameLinks);

/* Laydown name. */
    gVarContext = gCurrentDictionary->dic_HeaderPtr;
    pfCopyMemory( (uint8_t *) gCurrentDictionary->dic_HeaderPtr, FName, (*FName)+1 );
    gCurrentDictionary->dic_HeaderPtr += (*FName)+1;

/* Set flags. */
    *(char*)gVarContext |= (char) Flags;

/* Align to quad byte boundaries with zeroes. */
    while( gCurrentDictionary->dic_HeaderPtr & UINT32_MASK )
    {
        *(char*)(gCurrentDictionary->dic_HeaderPtr++) = 0;
    }
}

/***************************************************************
** Convert name then create dictionary entry.
*/
void CreateDicEntryC( ExecToken XT, const char *CName, ucell_t Flags )
{
    ForthString FName[40];
    CStringToForth( FName, CName, sizeof(FName) );
    CreateDicEntry( XT, FName, Flags );
}

/***************************************************************
** Convert absolute namefield address to previous absolute name
** field address or NULL.
*/
const ForthString *NameToPrevious( const ForthString *NFA )
{
    cell_t RelNamePtr;
    const cfNameLinks *cfnl;

/* DBUG(("\nNameToPrevious: NFA = 0x%x\n", (cell_t) NFA)); */
    cfnl = (const cfNameLinks *) ( ((const char *) NFA) - sizeof(cfNameLinks) );

    RelNamePtr = READ_CELL_DIC((const cell_t *) (&cfnl->cfnl_PreviousName));
/* DBUG(("\nNameToPrevious: RelNamePtr = 0x%x\n", (cell_t) RelNamePtr )); */
    if( RelNamePtr )
    {
        return ( (ForthString *) NAMEREL_TO_ABS( RelNamePtr ) );
    }
    else
    {
        return NULL;
    }
}
/***************************************************************
** Convert NFA to ExecToken.
*/
ExecToken NameToToken( const ForthString *NFA )
{
    const cfNameLinks *cfnl;

/* Convert absolute namefield address to absolute link field address. */
    cfnl = (const cfNameLinks *) ( ((const char *) NFA) - sizeof(cfNameLinks) );

    return READ_CELL_DIC((const cell_t *) (&cfnl->cfnl_ExecToken));
}

/***************************************************************
** Find XTs needed by compiler.
*/
cell_t FindSpecialXTs( void )
{
    if( ffFindC( "(QUIT)", &gQuitP_XT ) == 0) goto nofind;
    if( ffFindC( "NUMBER?", &gNumberQ_XT ) == 0) goto nofind;
    if( ffFindC( "ACCEPT", &gAcceptP_XT ) == 0) goto nofind;
DBUG(("gNumberQ_XT = 0x%x\n", (unsigned int)gNumberQ_XT ));
    return 0;

nofind:
    ERR("FindSpecialXTs failed!\n");
    return -1;
}

/***************************************************************
** Build a dictionary from scratch.
*/
#ifndef PF_NO_INIT
PForthDictionary pfBuildDictionary( cell_t HeaderSize, cell_t CodeSize )
{
    pfDictionary_t *dic;

    dic = pfCreateDictionary( HeaderSize, CodeSize );
    if( !dic ) goto nomem;

    pfDebugMessage("pfBuildDictionary: Start adding dictionary entries.\n");

    gCurrentDictionary = dic;
    gNumPrimitives = NUM_PRIMITIVES;

    CreateDicEntryC( ID_EXIT, "EXIT", 0 );
    pfDebugMessage("pfBuildDictionary: added EXIT\n");
    CreateDicEntryC( ID_1MINUS, "1-", 0 );
    pfDebugMessage("pfBuildDictionary: added 1-\n");
    CreateDicEntryC( ID_1PLUS, "1+", 0 );
    CreateDicEntryC( ID_2_R_FETCH, "2R@", 0 );
    CreateDicEntryC( ID_2_R_FROM, "2R>", 0 );
    CreateDicEntryC( ID_2_TO_R, "2>R", 0 );
    CreateDicEntryC( ID_2DUP, "2DUP", 0 );
    CreateDicEntryC( ID_2LITERAL, "2LITERAL", FLAG_IMMEDIATE );
    CreateDicEntryC( ID_2LITERAL_P, "(2LITERAL)", 0 );
    CreateDicEntryC( ID_2MINUS, "2-", 0 );
    CreateDicEntryC( ID_2PLUS, "2+", 0 );
    CreateDicEntryC( ID_2OVER, "2OVER", 0 );
    CreateDicEntryC( ID_2SWAP, "2SWAP", 0 );
    CreateDicEntryC( ID_ACCEPT_P, "(ACCEPT)", 0 );
    CreateDeferredC( ID_ACCEPT_P, "ACCEPT" );
    CreateDicEntryC( ID_ALITERAL, "ALITERAL", FLAG_IMMEDIATE );
    CreateDicEntryC( ID_ALITERAL_P, "(ALITERAL)", 0 );
    CreateDicEntryC( ID_ALLOCATE, "ALLOCATE", 0 );
    pfDebugMessage("pfBuildDictionary: added ALLOCATE\n");
    CreateDicEntryC( ID_ARSHIFT, "ARSHIFT", 0 );
    CreateDicEntryC( ID_AND, "AND", 0 );
    CreateDicEntryC( ID_BAIL, "BAIL", 0 );
    CreateDicEntryC( ID_BRANCH, "BRANCH", 0 );
    CreateDicEntryC( ID_BODY_OFFSET, "BODY_OFFSET", 0 );
    CreateDicEntryC( ID_BYE, "BYE", 0 );
    CreateDicEntryC( ID_CATCH, "CATCH", 0 );
    CreateDicEntryC( ID_CELL, "CELL", 0 );
    CreateDicEntryC( ID_CELLS, "CELLS", 0 );
    CreateDicEntryC( ID_CFETCH, "C@", 0 );
    CreateDicEntryC( ID_CMOVE, "CMOVE", 0 );
    CreateDicEntryC( ID_CMOVE_UP, "CMOVE>", 0 );
    CreateDicEntryC( ID_COLON, ":", 0 );
    CreateDicEntryC( ID_COLON_P, "(:)", 0 );
    CreateDicEntryC( ID_COMPARE, "COMPARE", 0 );
    CreateDicEntryC( ID_COMP_EQUAL, "=", 0 );
    CreateDicEntryC( ID_COMP_NOT_EQUAL, "<>", 0 );
    CreateDicEntryC( ID_COMP_GREATERTHAN, ">", 0 );
    CreateDicEntryC( ID_COMP_U_GREATERTHAN, "U>", 0 );
    pfDebugMessage("pfBuildDictionary: added U>\n");
    CreateDicEntryC( ID_COMP_LESSTHAN, "<", 0 );
    CreateDicEntryC( ID_COMP_U_LESSTHAN, "U<", 0 );
    CreateDicEntryC( ID_COMP_ZERO_EQUAL, "0=", 0 );
    CreateDicEntryC( ID_COMP_ZERO_NOT_EQUAL, "0<>", 0 );
    CreateDicEntryC( ID_COMP_ZERO_GREATERTHAN, "0>", 0 );
    CreateDicEntryC( ID_COMP_ZERO_LESSTHAN, "0<", 0 );
    CreateDicEntryC( ID_CR, "CR", 0 );
    CreateDicEntryC( ID_CREATE, "CREATE", 0 );
    CreateDicEntryC( ID_CREATE_P, "(CREATE)", 0 );
    CreateDicEntryC( ID_D_PLUS, "D+", 0 );
    CreateDicEntryC( ID_D_MINUS, "D-", 0 );
    CreateDicEntryC( ID_D_UMSMOD, "UM/MOD", 0 );
    CreateDicEntryC( ID_D_MUSMOD, "MU/MOD", 0 );
    CreateDicEntryC( ID_D_MTIMES, "M*", 0 );
    pfDebugMessage("pfBuildDictionary: added M*\n");
    CreateDicEntryC( ID_D_UMTIMES, "UM*", 0 );
    CreateDicEntryC( ID_DEFER, "DEFER", 0 );
    CreateDicEntryC( ID_CSTORE, "C!", 0 );
    CreateDicEntryC( ID_DEPTH, "DEPTH",  0 );
    pfDebugMessage("pfBuildDictionary: added DEPTH\n");
    CreateDicEntryC( ID_DIVIDE, "/", 0 );
    CreateDicEntryC( ID_DOT, ".",  0 );
    CreateDicEntryC( ID_DOTS, ".S",  0 );
    pfDebugMessage("pfBuildDictionary: added .S\n");
    CreateDicEntryC( ID_DO_P, "(DO)", 0 );
    CreateDicEntryC( ID_DROP, "DROP", 0 );
    CreateDicEntryC( ID_DUMP, "DUMP", 0 );
    CreateDicEntryC( ID_DUP, "DUP",  0 );
    CreateDicEntryC( ID_EMIT_P, "(EMIT)",  0 );
    pfDebugMessage("pfBuildDictionary: added (EMIT)\n");
    CreateDeferredC( ID_EMIT_P, "EMIT");
    pfDebugMessage("pfBuildDictionary: added EMIT\n");
    CreateDicEntryC( ID_EOL, "EOL",  0 );
    CreateDicEntryC( ID_ERRORQ_P, "(?ERROR)",  0 );
    CreateDicEntryC( ID_ERRORQ_P, "?ERROR",  0 );
    CreateDicEntryC( ID_EXECUTE, "EXECUTE",  0 );
    CreateDicEntryC( ID_FETCH, "@",  0 );
    CreateDicEntryC( ID_FILL, "FILL", 0 );
    CreateDicEntryC( ID_FIND, "FIND",  0 );
    CreateDicEntryC( ID_FILE_CREATE, "CREATE-FILE",  0 );
    CreateDicEntryC( ID_FILE_DELETE, "DELETE-FILE",  0 );
    CreateDicEntryC( ID_FILE_OPEN, "OPEN-FILE",  0 );
    CreateDicEntryC( ID_FILE_CLOSE, "CLOSE-FILE",  0 );
    CreateDicEntryC( ID_FILE_READ, "READ-FILE",  0 );
    CreateDicEntryC( ID_FILE_SIZE, "FILE-SIZE",  0 );
    CreateDicEntryC( ID_FILE_WRITE, "WRITE-FILE",  0 );
    CreateDicEntryC( ID_FILE_POSITION, "FILE-POSITION",  0 );
    CreateDicEntryC( ID_FILE_REPOSITION, "REPOSITION-FILE",  0 );
    CreateDicEntryC( ID_FILE_FLUSH, "FLUSH-FILE",  0 );
    CreateDicEntryC( ID_FILE_RENAME, "(RENAME-FILE)",  0 );
    CreateDicEntryC( ID_FILE_RESIZE, "(RESIZE-FILE)",  0 );
    CreateDicEntryC( ID_FILE_RO, "R/O",  0 );
    CreateDicEntryC( ID_FILE_RW, "R/W",  0 );
    CreateDicEntryC( ID_FILE_WO, "W/O",  0 );
    CreateDicEntryC( ID_FILE_BIN, "BIN",  0 );
    CreateDicEntryC( ID_FINDNFA, "FINDNFA",  0 );
    CreateDicEntryC( ID_FLUSHEMIT, "FLUSHEMIT",  0 );
    CreateDicEntryC( ID_FREE, "FREE",  0 );
#include "pfcompfp.h"
    CreateDicEntryC( ID_HERE, "HERE",  0 );
    CreateDicEntryC( ID_NUMBERQ_P, "(SNUMBER?)",  0 );
    CreateDicEntryC( ID_I, "I",  0 );
    CreateDicEntryC( ID_INTERPRET, "INTERPRET", 0 );
    CreateDicEntryC( ID_J, "J",  0 );
    CreateDicEntryC( ID_INCLUDE_FILE, "INCLUDE-FILE",  0 );
    CreateDicEntryC( ID_KEY, "KEY",  0 );
    CreateDicEntryC( ID_LEAVE_P, "(LEAVE)", 0 );
    CreateDicEntryC( ID_LITERAL, "LITERAL", FLAG_IMMEDIATE );
    CreateDicEntryC( ID_LITERAL_P, "(LITERAL)", 0 );
    CreateDicEntryC( ID_LOADSYS, "LOADSYS", 0 );
    CreateDicEntryC( ID_LOCAL_COMPILER, "LOCAL-COMPILER", 0 );
    CreateDicEntryC( ID_LOCAL_ENTRY, "(LOCAL.ENTRY)", 0 );
    CreateDicEntryC( ID_LOCAL_EXIT, "(LOCAL.EXIT)", 0 );
    CreateDicEntryC( ID_LOCAL_FETCH, "(LOCAL@)", 0 );
    CreateDicEntryC( ID_LOCAL_FETCH_1, "(1_LOCAL@)", 0 );
    CreateDicEntryC( ID_LOCAL_FETCH_2, "(2_LOCAL@)", 0 );
    CreateDicEntryC( ID_LOCAL_FETCH_3, "(3_LOCAL@)", 0 );
    CreateDicEntryC( ID_LOCAL_FETCH_4, "(4_LOCAL@)", 0 );
    CreateDicEntryC( ID_LOCAL_FETCH_5, "(5_LOCAL@)", 0 );
    CreateDicEntryC( ID_LOCAL_FETCH_6, "(6_LOCAL@)", 0 );
    CreateDicEntryC( ID_LOCAL_FETCH_7, "(7_LOCAL@)", 0 );
    CreateDicEntryC( ID_LOCAL_FETCH_8, "(8_LOCAL@)", 0 );
    CreateDicEntryC( ID_LOCAL_STORE, "(LOCAL!)", 0 );
    CreateDicEntryC( ID_LOCAL_STORE_1, "(1_LOCAL!)", 0 );
    CreateDicEntryC( ID_LOCAL_STORE_2, "(2_LOCAL!)", 0 );
    CreateDicEntryC( ID_LOCAL_STORE_3, "(3_LOCAL!)", 0 );
    CreateDicEntryC( ID_LOCAL_STORE_4, "(4_LOCAL!)", 0 );
    CreateDicEntryC( ID_LOCAL_STORE_5, "(5_LOCAL!)", 0 );
    CreateDicEntryC( ID_LOCAL_STORE_6, "(6_LOCAL!)", 0 );
    CreateDicEntryC( ID_LOCAL_STORE_7, "(7_LOCAL!)", 0 );
    CreateDicEntryC( ID_LOCAL_STORE_8, "(8_LOCAL!)", 0 );
    CreateDicEntryC( ID_LOCAL_PLUSSTORE, "(LOCAL+!)", 0 );
    CreateDicEntryC( ID_LOOP_P, "(LOOP)", 0 );
    CreateDicEntryC( ID_LSHIFT, "LSHIFT", 0 );
    CreateDicEntryC( ID_MAX, "MAX", 0 );
    CreateDicEntryC( ID_MIN, "MIN", 0 );
    CreateDicEntryC( ID_MINUS, "-", 0 );
    CreateDicEntryC( ID_NAME_TO_TOKEN, "NAME>", 0 );
    CreateDicEntryC( ID_NAME_TO_PREVIOUS, "PREVNAME", 0 );
    CreateDicEntryC( ID_NOOP, "NOOP", 0 );
    CreateDeferredC( ID_NUMBERQ_P, "NUMBER?" );
    CreateDicEntryC( ID_OR, "OR", 0 );
    CreateDicEntryC( ID_OVER, "OVER", 0 );
    pfDebugMessage("pfBuildDictionary: added OVER\n");
    CreateDicEntryC( ID_PICK, "PICK",  0 );
    CreateDicEntryC( ID_PLUS, "+",  0 );
    CreateDicEntryC( ID_PLUSLOOP_P, "(+LOOP)", 0 );
    CreateDicEntryC( ID_PLUS_STORE, "+!",  0 );
    CreateDicEntryC( ID_QUIT_P, "(QUIT)",  0 );
    CreateDeferredC( ID_QUIT_P, "QUIT" );
    CreateDicEntryC( ID_QDO_P, "(?DO)", 0 );
    CreateDicEntryC( ID_QDUP, "?DUP",  0 );
    CreateDicEntryC( ID_QTERMINAL, "?TERMINAL",  0 );
    CreateDicEntryC( ID_QTERMINAL, "KEY?",  0 );
    CreateDicEntryC( ID_REFILL, "REFILL",  0 );
    CreateDicEntryC( ID_RESIZE, "RESIZE",  0 );
    CreateDicEntryC( ID_ROLL, "ROLL",  0 );
    CreateDicEntryC( ID_ROT, "ROT",  0 );
    CreateDicEntryC( ID_RSHIFT, "RSHIFT",  0 );
    CreateDicEntryC( ID_R_DROP, "RDROP",  0 );
    CreateDicEntryC( ID_R_FETCH, "R@",  0 );
    CreateDicEntryC( ID_R_FROM, "R>",  0 );
    CreateDicEntryC( ID_RP_FETCH, "RP@",  0 );
    CreateDicEntryC( ID_RP_STORE, "RP!",  0 );
    CreateDicEntryC( ID_SEMICOLON, ";",  FLAG_IMMEDIATE );
    CreateDicEntryC( ID_SP_FETCH, "SP@",  0 );
    CreateDicEntryC( ID_SP_STORE, "SP!",  0 );
    CreateDicEntryC( ID_STORE, "!",  0 );
    CreateDicEntryC( ID_SAVE_FORTH_P, "(SAVE-FORTH)",  0 );
    CreateDicEntryC( ID_SCAN, "SCAN",  0 );
    CreateDicEntryC( ID_SKIP, "SKIP",  0 );
    CreateDicEntryC( ID_SLEEP_P, "(SLEEP)", 0 );
    CreateDicEntryC( ID_SOURCE, "SOURCE",  0 );
    CreateDicEntryC( ID_SOURCE_SET, "SET-SOURCE",  0 );
    CreateDicEntryC( ID_SOURCE_ID, "SOURCE-ID",  0 );
    CreateDicEntryC( ID_SOURCE_ID_PUSH, "PUSH-SOURCE-ID",  0 );
    CreateDicEntryC( ID_SOURCE_ID_POP, "POP-SOURCE-ID",  0 );
    CreateDicEntryC( ID_SOURCE_LINE_NUMBER_FETCH, "SOURCE-LINE-NUMBER@",  0 );
    CreateDicEntryC( ID_SOURCE_LINE_NUMBER_STORE, "SOURCE-LINE-NUMBER!",  0 );
    CreateDicEntryC( ID_SWAP, "SWAP",  0 );
    CreateDicEntryC( ID_TEST1, "TEST1",  0 );
    CreateDicEntryC( ID_TEST2, "TEST2",  0 );
    CreateDicEntryC( ID_TICK, "'", 0 );
    CreateDicEntryC( ID_TIMES, "*", 0 );
    CreateDicEntryC( ID_THROW, "THROW", 0 );
    CreateDicEntryC( ID_TO_R, ">R", 0 );
    CreateDicEntryC( ID_TYPE, "TYPE", 0 );
    CreateDicEntryC( ID_VAR_BASE, "BASE", 0 );
    CreateDicEntryC( ID_VAR_BYE_CODE, "BYE-CODE", 0 );
    CreateDicEntryC( ID_VAR_CODE_BASE, "CODE-BASE", 0 );
    CreateDicEntryC( ID_VAR_CODE_LIMIT, "CODE-LIMIT", 0 );
    CreateDicEntryC( ID_VAR_CONTEXT, "CONTEXT", 0 );
    CreateDicEntryC( ID_VAR_DP, "DP", 0 );
    CreateDicEntryC( ID_VAR_ECHO, "ECHO", 0 );
    CreateDicEntryC( ID_VAR_HEADERS_PTR, "HEADERS-PTR", 0 );
    CreateDicEntryC( ID_VAR_HEADERS_BASE, "HEADERS-BASE", 0 );
    CreateDicEntryC( ID_VAR_HEADERS_LIMIT, "HEADERS-LIMIT", 0 );
    CreateDicEntryC( ID_VAR_NUM_TIB, "#TIB", 0 );
    CreateDicEntryC( ID_VAR_RETURN_CODE, "RETURN-CODE", 0 );
    CreateDicEntryC( ID_VAR_TRACE_FLAGS, "TRACE-FLAGS", 0 );
    CreateDicEntryC( ID_VAR_TRACE_LEVEL, "TRACE-LEVEL", 0 );
    CreateDicEntryC( ID_VAR_TRACE_STACK, "TRACE-STACK", 0 );
    CreateDicEntryC( ID_VAR_OUT, "OUT", 0 );
    CreateDicEntryC( ID_VAR_STATE, "STATE", 0 );
    CreateDicEntryC( ID_VAR_TO_IN, ">IN", 0 );
    CreateDicEntryC( ID_VERSION_CODE, "VERSION_CODE", 0 );
    CreateDicEntryC( ID_WORD, "WORD", 0 );
    CreateDicEntryC( ID_WORD_FETCH, "W@", 0 );
    CreateDicEntryC( ID_WORD_STORE, "W!", 0 );
    CreateDicEntryC( ID_XOR, "XOR", 0 );
    CreateDicEntryC( ID_ZERO_BRANCH, "0BRANCH", 0 );

    pfDebugMessage("pfBuildDictionary: FindSpecialXTs\n");
    if( FindSpecialXTs() < 0 ) goto error;

    if( CompileCustomFunctions() < 0 ) goto error; /* Call custom 'C' call builder. */

#ifdef PF_DEBUG
    DumpMemory( dic->dic_HeaderBase, 256 );
    DumpMemory( dic->dic_CodeBase, 256 );
#endif

    pfDebugMessage("pfBuildDictionary: Finished adding dictionary entries.\n");
    return (PForthDictionary) dic;

error:
    pfDebugMessage("pfBuildDictionary: Error adding dictionary entries.\n");
    pfDeleteDictionary( dic );
    return NULL;

nomem:
    return NULL;
}
#endif /* !PF_NO_INIT */

/*
** ( xt -- nfa 1 , x 0 , find NFA in dictionary from XT )
** 1 for IMMEDIATE values
*/
cell_t ffTokenToName( ExecToken XT, const ForthString **NFAPtr )
{
    const ForthString *NameField;
    cell_t Searching = TRUE;
    cell_t Result = 0;
    ExecToken TempXT;

    NameField = (ForthString *) gVarContext;
DBUGX(("\ffCodeToName: gVarContext = 0x%x\n", gVarContext));

    do
    {
        TempXT = NameToToken( NameField );

        if( TempXT == XT )
        {
DBUGX(("ffCodeToName: NFA = 0x%x\n", NameField));
            *NFAPtr = NameField ;
            Result = 1;
            Searching = FALSE;
        }
        else
        {
            NameField = NameToPrevious( NameField );
            if( NameField == NULL )
            {
                *NFAPtr = 0;
                Searching = FALSE;
            }
        }
    } while ( Searching);

    return Result;
}

/*
** ( $name -- $addr 0 | nfa -1 | nfa 1 , find NFA in dictionary )
** 1 for IMMEDIATE values
*/
cell_t ffFindNFA( const ForthString *WordName, const ForthString **NFAPtr )
{
    const ForthString *WordChar;
    uint8_t WordLen;
    const char *NameField, *NameChar;
    int8_t NameLen;
    cell_t Searching = TRUE;
    cell_t Result = 0;

    WordLen = (uint8_t) ((ucell_t)*WordName & 0x1F);
    WordChar = WordName+1;

    NameField = (ForthString *) gVarContext;
DBUG(("\nffFindNFA: WordLen = %d, WordName = %*s\n", WordLen, WordLen, WordChar ));
DBUG(("\nffFindNFA: gVarContext = 0x%x\n", gVarContext));
    do
    {
        NameLen = (uint8_t) ((ucell_t)(*NameField) & MASK_NAME_SIZE);
        NameChar = NameField+1;
/* DBUG(("   %c\n", (*NameField & FLAG_SMUDGE) ? 'S' : 'V' )); */
        if( ((*NameField & FLAG_SMUDGE) == 0) &&
            (NameLen == WordLen) &&
            ffCompareTextCaseN( NameChar, WordChar, WordLen ) ) /* FIXME - slow */
        {
DBUG(("ffFindNFA: found it at NFA = 0x%x\n", NameField));
            *NFAPtr = NameField ;
            Result = ((*NameField) & FLAG_IMMEDIATE) ? 1 : -1;
            Searching = FALSE;
        }
        else
        {
            NameField = NameToPrevious( NameField );
            if( NameField == NULL )
            {
                *NFAPtr = WordName;
                Searching = FALSE;
            }
        }
    } while ( Searching);
DBUG(("ffFindNFA: returns 0x%x\n", Result));
    return Result;
}


/***************************************************************
** ( $name -- $name 0 | xt -1 | xt 1 )
** 1 for IMMEDIATE values
*/
cell_t ffFind( const ForthString *WordName, ExecToken *pXT )
{
    const ForthString *NFA;
    cell_t Result;

    Result = ffFindNFA( WordName, &NFA );
DBUG(("ffFind: %8s at 0x%x\n", WordName+1, NFA)); /* WARNING, not NUL terminated. %Q */
    if( Result )
    {
        *pXT = NameToToken( NFA );
    }
    else
    {
        *pXT = (ExecToken) WordName;
    }

    return Result;
}

/****************************************************************
** Find name when passed 'C' string.
*/
cell_t ffFindC( const char *WordName, ExecToken *pXT )
{
DBUG(("ffFindC: %s\n", WordName ));
    CStringToForth( gScratch, WordName, sizeof(gScratch) );
    return ffFind( gScratch, pXT );
}


/***********************************************************/
/********* Compiling New Words *****************************/
/***********************************************************/
#define DIC_SAFETY_MARGIN  (400)

/*************************************************************
**  Check for dictionary overflow.
*/
static cell_t ffCheckDicRoom( void )
{
    cell_t RoomLeft;
    RoomLeft = (char *)gCurrentDictionary->dic_HeaderLimit -
           (char *)gCurrentDictionary->dic_HeaderPtr;
    if( RoomLeft < DIC_SAFETY_MARGIN )
    {
        pfReportError("ffCheckDicRoom", PF_ERR_HEADER_ROOM);
        return PF_ERR_HEADER_ROOM;
    }

    RoomLeft = (char *)gCurrentDictionary->dic_CodeLimit -
               (char *)gCurrentDictionary->dic_CodePtr.Byte;
    if( RoomLeft < DIC_SAFETY_MARGIN )
    {
        pfReportError("ffCheckDicRoom", PF_ERR_CODE_ROOM);
        return PF_ERR_CODE_ROOM;
    }
    return 0;
}

/*************************************************************
**  Create a dictionary entry given a string name.
*/
void ffCreateSecondaryHeader( const ForthStringPtr FName)
{
    pfDebugMessage("ffCreateSecondaryHeader()\n");
/* Check for dictionary overflow. */
    if( ffCheckDicRoom() ) return;

    pfDebugMessage("ffCreateSecondaryHeader: CheckRedefinition()\n");
    CheckRedefinition( FName );
/* Align CODE_HERE */
    CODE_HERE = (cell_t *)( (((ucell_t)CODE_HERE) + UINT32_MASK) & ~UINT32_MASK);
    CreateDicEntry( (ExecToken) ABS_TO_CODEREL(CODE_HERE), FName, FLAG_SMUDGE );
}

/*************************************************************
** Begin compiling a secondary word.
*/
static void ffStringColon( const ForthStringPtr FName)
{
    ffCreateSecondaryHeader( FName );
    gVarState = 1;
}

/*************************************************************
** Read the next ExecToken from the Source and create a word.
*/
void ffColon( void )
{
    char *FName;

    gDepthAtColon = DATA_STACK_DEPTH;

    FName = ffWord( BLANK );
    if( *FName > 0 )
    {
        ffStringColon( FName );
    }
}

/*************************************************************
** Check to see if name is already in dictionary.
*/
static cell_t CheckRedefinition( const ForthStringPtr FName )
{
    cell_t flag;
    ExecToken XT;

    flag = ffFind( FName, &XT);
    if ( flag && !gVarQuiet)
    {
        ioType( FName+1, (cell_t) *FName );
        MSG( " redefined.\n" ); /* FIXME - allow user to run off this warning. */
    }
    return flag;
}

void ffStringCreate( char *FName)
{
    ffCreateSecondaryHeader( FName );

    CODE_COMMA( ID_CREATE_P );
    CODE_COMMA( ID_EXIT );
    ffFinishSecondary();

}

/* Read the next ExecToken from the Source and create a word. */
void ffCreate( void )
{
    char *FName;

    FName = ffWord( BLANK );
    if( *FName > 0 )
    {
        ffStringCreate( FName );
    }
}

void ffStringDefer( const ForthStringPtr FName, ExecToken DefaultXT )
{
    pfDebugMessage("ffStringDefer()\n");
    ffCreateSecondaryHeader( FName );

    CODE_COMMA( ID_DEFER_P );
    CODE_COMMA( DefaultXT );

    ffFinishSecondary();

}
#ifndef PF_NO_INIT
/* Convert name then create deferred dictionary entry. */
static void CreateDeferredC( ExecToken DefaultXT, const char *CName )
{
    char FName[40];
    CStringToForth( FName, CName, sizeof(FName) );
    ffStringDefer( FName, DefaultXT );
}
#endif

/* Read the next token from the Source and create a word. */
void ffDefer( void )
{
    char *FName;

    FName = ffWord( BLANK );
    if( *FName > 0 )
    {
        ffStringDefer( FName, ID_QUIT_P );
    }
}

/* Unsmudge the word to make it visible. */
static void ffUnSmudge( void )
{
    *(char*)gVarContext &= ~FLAG_SMUDGE;
}

/* Implement ; */
ThrowCode ffSemiColon( void )
{
    ThrowCode exception = 0;
    gVarState = 0;

    if( (gDepthAtColon != DATA_STACK_DEPTH) &&
        (gDepthAtColon != DEPTH_AT_COLON_INVALID) ) /* Ignore if no ':' */
    {
        exception = THROW_SEMICOLON;
    }
    else
    {
        ffFinishSecondary();
    }
    gDepthAtColon = DEPTH_AT_COLON_INVALID;
    return exception;
}

/* Finish the definition of a Forth word. */
void ffFinishSecondary( void )
{
    CODE_COMMA( ID_EXIT );
    ffUnSmudge();
}

/**************************************************************/
/* Used to pull a number from the dictionary to the stack */
void ff2Literal( cell_t dHi, cell_t dLo )
{
    CODE_COMMA( ID_2LITERAL_P );
    CODE_COMMA( dHi );
    CODE_COMMA( dLo );
}
void ffALiteral( cell_t Num )
{
    CODE_COMMA( ID_ALITERAL_P );
    CODE_COMMA( Num );
}
void ffLiteral( cell_t Num )
{
    CODE_COMMA( ID_LITERAL_P );
    CODE_COMMA( Num );
}

#ifdef PF_SUPPORT_FP
void ffFPLiteral( PF_FLOAT fnum )
{
    /* Hack for Metrowerks compiler which won't compile the
     * original expression.
     */
    PF_FLOAT  *temp;
    cell_t    *dicPtr;

/* Make sure that literal float data is float aligned. */
    dicPtr = CODE_HERE + 1;
    while( (((ucell_t) dicPtr++) & (sizeof(PF_FLOAT) - 1)) != 0)
    {
        DBUG((" comma NOOP to align FPLiteral\n"));
        CODE_COMMA( ID_NOOP );
    }
    CODE_COMMA( ID_FP_FLITERAL_P );

    temp = (PF_FLOAT *)CODE_HERE;
    WRITE_FLOAT_DIC(temp,fnum);   /* Write to dictionary. */
    temp++;
    CODE_HERE = (cell_t *) temp;
}
#endif /* PF_SUPPORT_FP */

/**************************************************************/
static ThrowCode FindAndCompile( const char *theWord )
{
    cell_t Flag;
    ExecToken XT;
    cell_t Num;
    ThrowCode exception = 0;

    Flag = ffFind( theWord, &XT);
DBUG(("FindAndCompile: theWord = %8s, XT = 0x%x, Flag = %d\n", theWord, XT, Flag ));

/* Is it a normal word ? */
    if( Flag == -1 )
    {
        if( gVarState )  /* compiling? */
        {
            CODE_COMMA( XT );
        }
        else
        {
            exception = pfCatch( XT );
        }
    }
    else if ( Flag == 1 ) /* or is it IMMEDIATE ? */
    {
DBUG(("FindAndCompile: IMMEDIATE, theWord = 0x%x\n", theWord ));
        exception = pfCatch( XT );
    }
    else /* try to interpret it as a number. */
    {
/* Call deferred NUMBER? */
        cell_t NumResult;

DBUG(("FindAndCompile: not found, try number?\n" ));
        PUSH_DATA_STACK( theWord );   /* Push text of number */
        exception = pfCatch( gNumberQ_XT );
        if( exception ) goto error;

DBUG(("FindAndCompile: after number?\n" ));
        NumResult = POP_DATA_STACK;  /* Success? */
        switch( NumResult )
        {
        case NUM_TYPE_SINGLE:
            if( gVarState )  /* compiling? */
            {
                Num = POP_DATA_STACK;
                ffLiteral( Num );
            }
            break;

        case NUM_TYPE_DOUBLE:
            if( gVarState )  /* compiling? */
            {
                Num = POP_DATA_STACK;  /* get hi portion */
                ff2Literal( Num, POP_DATA_STACK );
            }
            break;

#ifdef PF_SUPPORT_FP
        case NUM_TYPE_FLOAT:
            if( gVarState )  /* compiling? */
            {
                ffFPLiteral( *gCurrentTask->td_FloatStackPtr++ );
            }
            break;
#endif

        case NUM_TYPE_BAD:
        default:
            ioType( theWord+1, *theWord );
            MSG( "  ? - unrecognized word!\n" );
            exception = THROW_UNDEFINED_WORD;
            break;

        }
    }
error:
    return exception;
}

/**************************************************************
** Forth outer interpreter.  Parses words from Source.
** Executes them or compiles them based on STATE.
*/
ThrowCode ffInterpret( void )
{
    cell_t flag;
    char *theWord;
    ThrowCode exception = 0;

/* Is there any text left in Source ? */
    while( gCurrentTask->td_IN < (gCurrentTask->td_SourceNum) )
    {

        pfDebugMessage("ffInterpret: calling ffWord(()\n");
        theWord = ffLWord( BLANK );
        DBUG(("ffInterpret: theWord = 0x%x, Len = %d\n", theWord, *theWord ));

        if( *theWord > 0 )
        {
            flag = 0;
            if( gLocalCompiler_XT )
            {
                PUSH_DATA_STACK( theWord );   /* Push word. */
                exception = pfCatch( gLocalCompiler_XT );
                if( exception ) goto error;
                flag = POP_DATA_STACK;  /* Compiled local? */
            }
            if( flag == 0 )
            {
                exception = FindAndCompile( theWord );
                if( exception ) goto error;
            }
        }

        DBUG(("ffInterpret: IN=%d, SourceNum=%d\n", gCurrentTask->td_IN,
            gCurrentTask->td_SourceNum ) );
    }
error:
    return exception;
}

/**************************************************************/
ThrowCode ffOK( void )
{
    cell_t exception = 0;
/* Check for stack underflow.   %Q what about overflows? */
    if( (gCurrentTask->td_StackBase - gCurrentTask->td_StackPtr) < 0 )
    {
        exception = THROW_STACK_UNDERFLOW;
    }
#ifdef PF_SUPPORT_FP  /* Check floating point stack too! */
    else if((gCurrentTask->td_FloatStackBase - gCurrentTask->td_FloatStackPtr) < 0)
    {
        exception = THROW_FLOAT_STACK_UNDERFLOW;
    }
#endif
    else if( gCurrentTask->td_InputStream == PF_STDIN)
    {
        if( !gVarState )  /* executing? */
        {
            if( !gVarQuiet )
            {
                MSG( "   ok\n" );
                if(gVarTraceStack) ffDotS();
            }
            else
            {
                EMIT_CR;
            }
        }
    }
    return exception;
}

/***************************************************************
** Cleanup Include stack by popping and closing files.
***************************************************************/
void pfHandleIncludeError( void )
{
    FileStream *cur;

    while( (cur = ffPopInputStream()) != PF_STDIN)
    {
        DBUG(("ffCleanIncludeStack: closing 0x%x\n", cur ));
        sdCloseFile(cur);
    }
}

/***************************************************************
** Interpret input in a loop.
***************************************************************/
ThrowCode ffOuterInterpreterLoop( void )
{
    cell_t exception = 0;
    do
    {
        exception = ffRefill();
        if(exception <= 0) break;

        exception = ffInterpret();
        if( exception == 0 )
        {
            exception = ffOK();
        }

    } while( exception == 0 );
    return exception;
}

/***************************************************************
** Include then close a file
***************************************************************/

ThrowCode ffIncludeFile( FileStream *InputFile )
{
    ThrowCode exception;

/* Push file stream. */
    exception = ffPushInputStream( InputFile );
    if( exception < 0 ) return exception;

/* Run outer interpreter for stream. */
    exception = ffOuterInterpreterLoop();
    if( exception )
    {
        int i;
/* Report line number and nesting level. */
        MSG("INCLUDE error on line #"); ffDot(gCurrentTask->td_LineNumber);
        MSG(", level = ");  ffDot(gIncludeIndex );
        EMIT_CR

/* Dump line of error and show offset in line for >IN */
        for( i=0; i<gCurrentTask->td_SourceNum; i++ )
        {
            char c = gCurrentTask->td_SourcePtr[i];
            if( c == '\t' ) c = ' ';
            EMIT(c);
        }
        EMIT_CR;
        for( i=0; i<(gCurrentTask->td_IN - 1); i++ ) EMIT('^');
        EMIT_CR;
    }

/* Pop file stream. */
    ffPopInputStream();

/* ANSI spec specifies that this should also close the file. */
    sdCloseFile(InputFile);

    return exception;
}

#endif /* !PF_NO_SHELL */

/***************************************************************
** Save current input stream on stack, use this new one.
***************************************************************/
Err ffPushInputStream( FileStream *InputFile )
{
    Err Result = 0;
    IncludeFrame *inf;

/* Push current input state onto special include stack. */
    if( gIncludeIndex < MAX_INCLUDE_DEPTH )
    {
        inf = &gIncludeStack[gIncludeIndex++];
        inf->inf_FileID = gCurrentTask->td_InputStream;
        inf->inf_IN = gCurrentTask->td_IN;
        inf->inf_LineNumber = gCurrentTask->td_LineNumber;
        inf->inf_SourceNum = gCurrentTask->td_SourceNum;
/* Copy TIB plus any NUL terminator into saved area. */
        if( (inf->inf_SourceNum > 0) && (inf->inf_SourceNum < (TIB_SIZE-1)) )
        {
            pfCopyMemory( inf->inf_SaveTIB, gCurrentTask->td_TIB, inf->inf_SourceNum+1 );
        }

/* Set new current input. */
        DBUG(( "ffPushInputStream: InputFile = 0x%x\n", InputFile ));
        gCurrentTask->td_InputStream = InputFile;
        gCurrentTask->td_LineNumber = 0;
    }
    else
    {
        ERR("ffPushInputStream: max depth exceeded.\n");
        return -1;
    }


    return Result;
}

/***************************************************************
** Go back to reading previous stream.
** Just return gCurrentTask->td_InputStream upon underflow.
***************************************************************/
FileStream *ffPopInputStream( void )
{
    IncludeFrame *inf;
    FileStream *Result;

DBUG(("ffPopInputStream: gIncludeIndex = %d\n", gIncludeIndex));
    Result = gCurrentTask->td_InputStream;

/* Restore input state. */
    if( gIncludeIndex > 0 )
    {
        inf = &gIncludeStack[--gIncludeIndex];
        gCurrentTask->td_InputStream = inf->inf_FileID;
        DBUG(("ffPopInputStream: stream = 0x%x\n", gCurrentTask->td_InputStream ));
        gCurrentTask->td_IN = inf->inf_IN;
        gCurrentTask->td_LineNumber = inf->inf_LineNumber;
        gCurrentTask->td_SourceNum = inf->inf_SourceNum;
/* Copy TIB plus any NUL terminator into saved area. */
        if( (inf->inf_SourceNum > 0) && (inf->inf_SourceNum < (TIB_SIZE-1)) )
        {
            pfCopyMemory( gCurrentTask->td_TIB, inf->inf_SaveTIB, inf->inf_SourceNum+1 );
        }

    }
DBUG(("ffPopInputStream: return = 0x%x\n", Result ));

    return Result;
}

/***************************************************************
** Convert file pointer to value consistent with SOURCE-ID.
***************************************************************/
cell_t ffConvertStreamToSourceID( FileStream *Stream )
{
    cell_t Result;
    if(Stream == PF_STDIN)
    {
        Result = 0;
    }
    else if(Stream == NULL)
    {
        Result = -1;
    }
    else
    {
        Result = (cell_t) Stream;
    }
    return Result;
}

/***************************************************************
** Convert file pointer to value consistent with SOURCE-ID.
***************************************************************/
FileStream * ffConvertSourceIDToStream( cell_t id )
{
    FileStream *stream;

    if( id == 0 )
    {
        stream = PF_STDIN;
    }
    else if( id == -1 )
    {
        stream = NULL;
    }
    else
    {
        stream = (FileStream *) id;
    }
    return stream;
}

/**************************************************************
** Receive line from input stream.
** Return length, or -1 for EOF.
*/
#define BACKSPACE  (8)
static cell_t readLineFromStream( char *buffer, cell_t maxChars, FileStream *stream )
{
    int   c;
    int   len;
    char *p;
    static int lastChar = 0;
    int   done = 0;

DBUGX(("readLineFromStream(0x%x, 0x%x, 0x%x)\n", buffer, len, stream ));
    p = buffer;
    len = 0;
    while( (len < maxChars) && !done )
    {
        c = sdInputChar(stream);
        switch(c)
        {
            case EOF:
                DBUG(("EOF\n"));
                done = 1;
                if( len <= 0 ) len = -1;
                break;

            case '\n':
                DBUGX(("EOL=\\n\n"));
                if( lastChar != '\r' ) done = 1;
                break;

            case '\r':
                DBUGX(("EOL=\\r\n"));
                done = 1;
                break;

            default:
                *p++ = (char) c;
                len++;
                break;
        }
        lastChar = c;
    }

/* NUL terminate line to simplify printing when debugging. */
    if( (len >= 0) && (len < maxChars) ) p[len] = '\0';

    return len;
}

/**************************************************************
** ( -- , fill Source from current stream )
** Return 1 if successful, 0 for EOF, or a negative error.
*/
cell_t ffRefill( void )
{
    cell_t Num;
    cell_t Result = 1;

/* reset >IN for parser */
    gCurrentTask->td_IN = 0;

/* get line from current stream */
    if( gCurrentTask->td_InputStream == PF_STDIN )
    {
    /* ACCEPT is deferred so we call it through the dictionary. */
        ThrowCode throwCode;
        PUSH_DATA_STACK( gCurrentTask->td_SourcePtr );
        PUSH_DATA_STACK( TIB_SIZE );
        throwCode = pfCatch( gAcceptP_XT );
        if (throwCode) {
            Result = throwCode;
            goto error;
        }
        Num = POP_DATA_STACK;
        if( Num < 0 )
        {
            Result = Num;
            goto error;
        }
    }
    else
    {
        Num = readLineFromStream( gCurrentTask->td_SourcePtr, TIB_SIZE,
            gCurrentTask->td_InputStream );
        if( Num == EOF )
        {
            Result = 0;
            Num = 0;
        }
    }

    gCurrentTask->td_SourceNum = Num;
    gCurrentTask->td_LineNumber++;  /* Bump for include. */

/* echo input if requested */
    if( gVarEcho && ( Num > 0))
    {
        ioType( gCurrentTask->td_SourcePtr, gCurrentTask->td_SourceNum );
        EMIT_CR;
    }

error:
    return Result;
}
