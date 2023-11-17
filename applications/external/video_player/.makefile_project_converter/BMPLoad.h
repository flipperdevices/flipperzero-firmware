//
//  BMPLoad.h
//  BMPLoad
//
//  Created by Y on 2015/12/24.
//  I do not claim any copyright of this code. Yutaka Yasuda.
//

#pragma once

#ifndef BMPLoad_h
#define BMPLoad_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <stdint.h>

//#define BMP_DEBUG 1
#define BMP_DEBUG 0

/* Windows Bitmap ヘッダ情報
 https://ja.wikipedia.org/wiki/Windows_bitmap
 https://msdn.microsoft.com/en-us/library/dd183374(VS.85).aspx
 https://msdn.microsoft.com/en-us/library/dd183376(v=vs.85).aspx
 などを参照（メンバ名などは後者からの引き写し）
 */

#pragma pack(push, 2) // アライメントを 2 バイト単位に指定


#pragma pack(pop)

// prototypes

unsigned char *BmpLoad(char *FileName, int *imageWidth, int *imageHeight, int *imageBits);
int RgbDump(unsigned char *Data, int width, int height, int imageBits);


#endif /* BMPLoad_h */
