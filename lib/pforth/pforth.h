/* @(#) pforth.h 98/01/26 1.2 */
#ifndef _pforth_h
#define _pforth_h

/***************************************************************
** Include file for pForth, a portable Forth based on 'C'
**
** This file is included in any application that uses pForth as a library.
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
**
***************************************************************/

/* Define stubs for data types so we can pass pointers but not touch inside. */
typedef void *PForthTask;
typedef void *PForthDictionary;

#include <stdint.h>
/* Integer types for Forth cells, signed and unsigned: */
typedef intptr_t cell_t;
typedef uintptr_t ucell_t;

typedef ucell_t ExecToken;              /* Execution Token */
typedef cell_t ThrowCode;

#ifdef __cplusplus
extern "C" {
#endif

typedef struct pfTaskData_s pfTaskData_t;

/* Main entry point to pForth. */
ThrowCode pfDoForth( const char *DicName, const char *SourceName, cell_t IfInit );

/* Turn off messages. */
void  pfSetQuiet( cell_t IfQuiet );

/* Query message status. */
cell_t  pfQueryQuiet( void );

/* Send a message using low level I/O of pForth */
void  pfMessage( const char *CString );

/* Create a task used to maintain context of execution. */
PForthTask pfCreateTask( cell_t UserStackDepth, cell_t ReturnStackDepth );

/* Establish this task as the current task. */
void  pfSetCurrentTask( PForthTask task );

/* Delete task created by pfCreateTask */
void  pfDeleteTask( PForthTask task );

/* Build a dictionary with all the basic kernel words. */
PForthDictionary pfBuildDictionary( cell_t HeaderSize, cell_t CodeSize );

/* Create an empty dictionary. */
PForthDictionary pfCreateDictionary( cell_t HeaderSize, cell_t CodeSize );
PForthDictionary pfCreateStaticDictionary( uint8_t *Header, uint8_t *Code,
        cell_t HeaderSize, cell_t CodeSize );

/* Load dictionary from a file. */
PForthDictionary pfLoadDictionary( const char *FileName, ExecToken *EntryPointPtr, pfTaskData_t *gCurrentTask );

/* Load dictionary from static array in "pfdicdat.h". */
PForthDictionary pfLoadStaticDictionary( pfTaskData_t *gCurrentTask );

/* Delete dictionary data. */
void  pfDeleteDictionary( PForthDictionary dict );

/* Execute the pForth interpreter. Yes, QUIT is an odd name but it has historical meaning. */
ThrowCode pfQuit( pfTaskData_t *gCurrentTask );

/* Execute a single execution token in the current task and return 0 or an error code. */
ThrowCode pfCatch( ExecToken XT, pfTaskData_t *gCurrentTask );

/* Include the given pForth source code file. */
ThrowCode pfIncludeFile( const char *FileName, pfTaskData_t *gCurrentTask );

/* Execute a Forth word by name. */
ThrowCode  pfExecIfDefined( const char *CString, pfTaskData_t *gCurrentTask );

#ifdef __cplusplus
}
#endif

#endif  /* _pforth_h */
