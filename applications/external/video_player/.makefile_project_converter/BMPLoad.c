//
//  BMPLoad.c
//  BMPLoad
//
//  Created by Y on 2015/12/24.
//  I do not claim any copyright of this code. Yutaka Yasuda.
//

#define _CRT_SECURE_NO_WARNINGS

#include "BMPLoad.h"

#pragma pack(push, 2) // アライメントを 2 バイト単位に指定

struct BITMAPFILEHEADER {
    uint16_t bfType; // 特にここが 16bit 幅なので 32bit アライメントされるとヘッダサイズが 14 でなく 16 バイトになってしまう
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
} ;

#define BITMAPFILEHEADER_SIZE 14 // アライメントに失敗していないことを強引に確認
#define BITMAPINFOHEADER_SIZE 40 // アライメントに失敗していないことを強引に確認

// Windows Bitmap ヘッダ 40 バイト
struct BITMAPINFOHEADER {
    uint32_t biSize;
    int32_t  biWidth;
    int32_t  biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t  biXPixPerMeter;
    int32_t  biYPixPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImporant;
} ;

#pragma pack(pop)

// エラーメッセージ（出力制御なし）
// stdarg ライブラリ関数群を利用
void errorMsg(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    vprintf(format, ap);
    va_end(ap);
}
// デバッグメッセージ（BMP_DEBUG をスイッチとして出力制御可）
void debugMsg(const char *format, ...)
{
    if( BMP_DEBUG ) errorMsg(format);
}

// BMP 形式のファイル（但し 24 or 32 bit color に限る）のビットマップを RGB データに読み込む
// ファイル名を指定して呼び出すとメモリを確保して RGB データを格納し、アドレスを戻す
// width, height, imageBits に関しても設定して戻す。imageBits は画素あたりのビット数。24 or 32。
unsigned char *BmpLoad(char *FileName, int *imageWidth, int *imageHeight, int *imageBits) {
    struct BITMAPFILEHEADER fileHeader;
    struct BITMAPINFOHEADER infoHeader;
    
    FILE *fp;
    int downToUp = 1; // 1: 下から上にデータが並ぶ（通常） 0: 逆転（上から下）
    unsigned char *Data; // イメージデータを格納する領域
    unsigned int size;// イメージのバイトサイズ
    
    int w, h, width, height; // カウンタ、縦横値の保存場所
    int lineSize; // 横一列ぶんのデータのバイト数
    unsigned char *lineData; // 横一列ぶんのデータの領域
    
    // コンパイル環境確認のために（アライメントが行われて構造体サイズが想定外になっていないことをチェック）
    if(sizeof(fileHeader) != BITMAPFILEHEADER_SIZE || sizeof(infoHeader) != BITMAPINFOHEADER_SIZE) {
        errorMsg("16bit width alighment required. %d\n", sizeof(fileHeader));
    }
    
    // 指定されたファイルを開く
    if ((fp = fopen(FileName, "rb"))==NULL){
        errorMsg("No such file. (%s)\n", FileName);
        return NULL;
    }
    
    // ファイルヘッダを読む
    if (fread(&fileHeader, sizeof(fileHeader), 1, fp) != 1) {
        errorMsg("Not BMP file (filesize under 14Bytes)\n");
        return NULL;
    }
    // 先頭 2 バイトのマジックナンバーが BM (0x42, 0x4d) であることを確認（htonsでネットワークバイトオーダで照合）
    //if(fileHeader.bfType != htons(0x424d)) {
    if (fileHeader.bfType != 0x424d && fileHeader.bfType != 0x4d42) {
        errorMsg("Not BMP file type (magc number = '%04x')\n", fileHeader.bfType);
        return NULL;
    }
    
    // ファイルヘッダを読む
    // debugMsg(" biSize  = %p  %d\n", &infoHeader.biSize, sizeof(infoHeader.biSize));
    // debugMsg(" biWidth = %p  %d\n", &infoHeader.biWidth, sizeof(infoHeader));
    if (fread(&infoHeader.biSize, sizeof(infoHeader.biSize), 1, fp) != 1) {
        errorMsg("No infoHeader field (too small filesize)\n");
        return NULL;
    }
    // Windows BitMap データ（ヘッダサイズ 40 バイト）であることを確認
    if(infoHeader.biSize != 40) {
        errorMsg("Not Windows BMP file type (header size = %d)\n", infoHeader.biSize);
        return NULL;
    }
    // 残りのファイルヘッダを読む
    if (fread(&infoHeader.biWidth, sizeof(infoHeader)-sizeof(infoHeader.biSize), 1, fp) != 1) {
        errorMsg("Not enough infoHeader field (too small filesize)\n");
        return NULL;
    }
    
    // 縦横サイズの確認
    width = infoHeader.biWidth;
    height = infoHeader.biHeight;
    debugMsg("width = %d, height = %d\n", infoHeader.biWidth, infoHeader.biHeight);
    if( height < 0) { // もし height が負の数の場合、それは「上から下」に向けてデータが並んでいることを意味する。
        downToUp = 0;
        height *= -1;
        debugMsg("(up to down scanned data)\n");
    }
    
    // プレーン数の確認（現在では常時 1 らしいが一応）
    if (infoHeader.biPlanes != 1) {
        errorMsg("Number of plain is not 1 (%d)\n", infoHeader.biPlanes);
        return NULL;
    }
    debugMsg("planes = %d\n", infoHeader.biPlanes);
    
    // ピクセルあたりのビット数を確認
    if ((infoHeader.biBitCount != 24) && (infoHeader.biBitCount != 32)) { // 24/32 bppでなければエラー
        errorMsg("only 24 or 32 bits per pixel format is acceptable (%d)\n", infoHeader.biBitCount);
        return NULL;
    }
    debugMsg("bits/pixel = %d\n", infoHeader.biBitCount);
    
    // ファイル先頭から所定位置までスキップしてビットマップデータ部分にジャンプ
    fseek(fp, fileHeader.bfOffBits, SEEK_SET);
    
    // 画像サイズの計算
    int pixSize = infoHeader.biBitCount / 8; // 画素あたりのバイト数を出しておく
    size = width * height * pixSize; // メモリサイズ
    Data = (unsigned char *) malloc(size);
    if (Data == NULL) {
        errorMsg("cannot alocate memory for the picture (%d bytes)\n", size);
        return NULL;
    }
    debugMsg("Data %p (%d bytes allocated)\n", Data, size);
    
    // ラインごとに読むための準備
    // 横 1 ラインのバイト数を求める（4バイトアライメント必須なので width * pixSize にパディングが有り得る）
    lineSize = width * pixSize; // とりあえず一行あたりの最低バイト数を求めておく
    if ((lineSize % 4) != 0) lineSize = ((lineSize / 4) + 1) * 4; // 境界ピッタリでなければ 4byte 境界にあわせる
    // debugMsg("lineSize = %d\n", lineSize);
    // ラインバッファを用意
    lineData = (unsigned char *) malloc(lineSize);
    if (Data == NULL) {
        errorMsg("cannot alocate memory for line buffer (%d bytes)\n", lineSize);
        return NULL;
    }
    
    // ラインごとに height 行ぶん読む
    for(h = 0; h < height; h++) {
        if (fread(lineData, lineSize, 1, fp) != 1 ){ // 横一行ぶん読む
            errorMsg("cannot read data body\n");
            return NULL;
        }
        unsigned char *target; // 格納先アドレス
        if(downToUp) { // 下から上にデータが並んでいる（配列並びに較べて上下逆転）している場合
            target = Data + (height -1 - h) * width * pixSize;
        } else {
            target = Data + h * width * pixSize;
        }
        // debugMsg("lineData (offset) = %d\n", lineData - Data);
        // BGR 並びになっている画素ごとの色情報を RGB に入れ替える
        for (w = 0; w < width; w++) {
            target[w * pixSize   ] = lineData[w * pixSize +2]; // Red
            target[w * pixSize +1] = lineData[w * pixSize +1]; // Green
            target[w * pixSize +2] = lineData[w * pixSize   ]; // Blue
            if(pixSize == 4) target[w * pixSize +3] = lineData[w * pixSize +3]; // Alpha
        }
    }
    
    // あと始末
    free(lineData);
    fclose(fp);
    
    // 戻り値などを設定
    *imageWidth = width;
    *imageHeight = height;
    *imageBits = pixSize * 8;
    return Data;
}


// RGB 形式のデータをダンプする（異常時は 1 が返る）
int RgbDump(unsigned char *Data, int width, int height, int imageBits)
{
    int w, h, pixSize;
    char format[16]="";
    pixSize = imageBits / 8; // 画素あたりのバイト数を出す
    if(imageBits == 24) {
        sprintf(format, "RGB");
    } else if(imageBits == 32) {
        sprintf(format, "RGB:A");
    } else {
        errorMsg("only 24 or 32 bits per pixel format is acceptable (%d)\n", imageBits);
        return 1;
    }
    printf("dump (%d x %d) format=\"%s\"\n", width, height, format);
    for(h=0; h<height; h++) {
        printf("%03d", h);
        for(w=0; w<width; w++) {
            // printf("#%d#\n", (h * width + w) * pixSize);
            printf(" %02x%02x%02x"
                   ,Data[ (h * width + w) * pixSize ]
                   ,Data[ (h * width + w) * pixSize + 1 ]
                   ,Data[ (h * width + w) * pixSize + 2 ] );
            if(pixSize == 4) { // 32bit Alpha つきフォーマットの場合は A 領域も出力
                printf(":%02x", Data[(h * width + w) * pixSize + 3 ]);
            }
        }
        printf("\n");
    }
    
    return 0;
}


