/*
 * FreeRTOS Kernel <DEVELOPMENT BRANCH>
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

#ifndef DATA_MODEL_H
#define DATA_MODEL_H

#if __DATA_MODEL__ == __DATA_MODEL_SMALL__
	#define pushm_x pushm.w
	#define popm_x popm.w
	#define push_x push.w
	#define pop_x pop.w
	#define mov_x mov.w
	#define cmp_x cmp.w
#endif

#if __DATA_MODEL__ == __DATA_MODEL_MEDIUM__
	#define pushm_x pushm.a
	#define popm_x popm.a
	#define push_x pushx.a
	#define pop_x popx.a
	#define mov_x mov.w
	#define cmp_x cmp.w
#endif

#if __DATA_MODEL__ == __DATA_MODEL_LARGE__
	#define pushm_x pushm.a
	#define popm_x popm.a
	#define push_x pushx.a
	#define pop_x popx.a
	#define mov_x movx.a
	#define cmp_x cmpx.a
#endif

#ifndef pushm_x
	#error The assembler options must define one of the following symbols: __DATA_MODEL_SMALL__, __DATA_MODEL_MEDIUM__, or __DATA_MODEL_LARGE__
#endif

#endif /* DATA_MODEL_H */

