/* @(#) pfcompil.h 96/12/18 1.11 */

#ifndef _pforth_compile_h
#define _pforth_compile_h

/***************************************************************
** Include file for PForth Compiler
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
***************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

Err   ffPushInputStream( FileStream *InputFile, pfTaskData_t *gCurrentTask );
ExecToken NameToToken( const ForthString *NFA );
FileStream * ffConvertSourceIDToStream( cell_t id );
FileStream *ffPopInputStream( pfTaskData_t *gCurrentTask );
cell_t  ffConvertStreamToSourceID( FileStream *Stream );
cell_t  ffFind( const ForthString *WordName, ExecToken *pXT );
cell_t  ffFindC( const char *WordName, ExecToken *pXT, pfTaskData_t *gCurrentTask );
cell_t  ffFindNFA( const ForthString *WordName, const ForthString **NFAPtr );
cell_t  ffNumberQ( const char *FWord, cell_t *Num );
cell_t  ffRefill( pfTaskData_t *gCurrentTask );
cell_t  ffTokenToName( ExecToken XT, const ForthString **NFAPtr );
cell_t *NameToCode( ForthString *NFA );
PForthDictionary pfBuildDictionary( cell_t HeaderSize, cell_t CodeSize );
char *ffWord( char c, pfTaskData_t *gCurrentTask );
char *ffLWord( char c, pfTaskData_t *gCurrentTask );
const ForthString *NameToPrevious( const ForthString *NFA );
cell_t FindSpecialCFAs( void );
cell_t FindSpecialXTs( pfTaskData_t *gCurrentTask );
cell_t NotCompiled( const char *FunctionName );
void  CreateDicEntry( ExecToken XT, const ForthStringPtr FName, ucell_t Flags );
void  CreateDicEntryC( ExecToken XT, const char *CName, ucell_t Flags );
void  ff2Literal( cell_t dHi, cell_t dLo );
void  ffALiteral( cell_t Num );
void  ffColon( pfTaskData_t *gCurrentTask );
void  ffCreate( pfTaskData_t *gCurrentTask );
void  ffCreateSecondaryHeader( const ForthStringPtr FName);
void  ffDefer( pfTaskData_t *gCurrentTask );
void  ffFinishSecondary( void );
void  ffLiteral( cell_t Num );
void  ffStringCreate( ForthStringPtr FName);
void  ffStringDefer( const ForthStringPtr FName, ExecToken DefaultXT );
void  pfHandleIncludeError( pfTaskData_t *gCurrentTask );

ThrowCode ffSemiColon( pfTaskData_t *gCurrentTask );
ThrowCode ffOK( pfTaskData_t *gCurrentTask );
ThrowCode ffInterpret( pfTaskData_t *gCurrentTask );
ThrowCode ffOuterInterpreterLoop( pfTaskData_t *gCurrentTask );
ThrowCode ffIncludeFile( FileStream *InputFile, pfTaskData_t *gCurrentTask );

#ifdef PF_SUPPORT_FP
void ffFPLiteral( PF_FLOAT fnum );
#endif

#ifdef __cplusplus
}
#endif

#endif /* _pforth_compile_h */
