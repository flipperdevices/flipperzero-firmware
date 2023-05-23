#pragma once
#ifndef BITBANDING_H
#define BITBANDING_H

#define MASK_TO_BIT31(A)        (A==0x80000000)? 31 : 0
#define MASK_TO_BIT30(A)        (A==0x40000000)? 30 : MASK_TO_BIT31(A)
#define MASK_TO_BIT29(A)        (A==0x20000000)? 29 : MASK_TO_BIT30(A)
#define MASK_TO_BIT28(A)        (A==0x10000000)? 28 : MASK_TO_BIT29(A)
#define MASK_TO_BIT27(A)        (A==0x08000000)? 27 : MASK_TO_BIT28(A)
#define MASK_TO_BIT26(A)        (A==0x04000000)? 26 : MASK_TO_BIT27(A)
#define MASK_TO_BIT25(A)        (A==0x02000000)? 25 : MASK_TO_BIT26(A)
#define MASK_TO_BIT24(A)        (A==0x01000000)? 24 : MASK_TO_BIT25(A)
#define MASK_TO_BIT23(A)        (A==0x00800000)? 23 : MASK_TO_BIT24(A)
#define MASK_TO_BIT22(A)        (A==0x00400000)? 22 : MASK_TO_BIT23(A)
#define MASK_TO_BIT21(A)        (A==0x00200000)? 21 : MASK_TO_BIT22(A)
#define MASK_TO_BIT20(A)        (A==0x00100000)? 20 : MASK_TO_BIT21(A)
#define MASK_TO_BIT19(A)        (A==0x00080000)? 19 : MASK_TO_BIT20(A)
#define MASK_TO_BIT18(A)        (A==0x00040000)? 18 : MASK_TO_BIT19(A)
#define MASK_TO_BIT17(A)        (A==0x00020000)? 17 : MASK_TO_BIT18(A)
#define MASK_TO_BIT16(A)        (A==0x00010000)? 16 : MASK_TO_BIT17(A)
#define MASK_TO_BIT15(A)        (A==0x00008000)? 15 : MASK_TO_BIT16(A)
#define MASK_TO_BIT14(A)        (A==0x00004000)? 14 : MASK_TO_BIT15(A)
#define MASK_TO_BIT13(A)        (A==0x00002000)? 13 : MASK_TO_BIT14(A)
#define MASK_TO_BIT12(A)        (A==0x00001000)? 12 : MASK_TO_BIT13(A)
#define MASK_TO_BIT11(A)        (A==0x00000800)? 11 : MASK_TO_BIT12(A)
#define MASK_TO_BIT10(A)        (A==0x00000400)? 10 : MASK_TO_BIT11(A)
#define MASK_TO_BIT09(A)        (A==0x00000200)? 9  : MASK_TO_BIT10(A)
#define MASK_TO_BIT08(A)        (A==0x00000100)? 8  : MASK_TO_BIT09(A)
#define MASK_TO_BIT07(A)        (A==0x00000080)? 7  : MASK_TO_BIT08(A)
#define MASK_TO_BIT06(A)        (A==0x00000040)? 6  : MASK_TO_BIT07(A)
#define MASK_TO_BIT05(A)        (A==0x00000020)? 5  : MASK_TO_BIT06(A)
#define MASK_TO_BIT04(A)        (A==0x00000010)? 4  : MASK_TO_BIT05(A)
#define MASK_TO_BIT03(A)        (A==0x00000008)? 3  : MASK_TO_BIT04(A)
#define MASK_TO_BIT02(A)        (A==0x00000004)? 2  : MASK_TO_BIT03(A)
#define MASK_TO_BIT01(A)        (A==0x00000002)? 1  : MASK_TO_BIT02(A)
#define MASK_TO_BIT(A)          (A==0x00000001)? 0  : MASK_TO_BIT01(A)

#ifndef SET
#define SET                      1
#define RESET                    0
#endif

#define BIT_BAND_PER(REG,BIT_MASK) (*(volatile uint32_t*)(PERIPH_BB_BASE+32*((uint32_t)(&(REG))-PERIPH_BASE)+4*((uint32_t)(MASK_TO_BIT(BIT_MASK)))))

#define BIT_BAND_SRAM(RAM,BIT) (*(volatile uint32_t*)(SRAM_BB_BASE+32*((uint32_t)((void*)(RAM))-SRAM_BASE)+4*((uint32_t)(BIT))))

//Example: BIT_BAND_PER(TIM1->SR, TIM_SR_UIF) = 0; //сбросить бит TIM_SR_UIF в TIM1->SR
//Example2: BIT_BAND_SRAM(&a, 13) = 1; //установить 13-й бит в переменной "a"
//Example3: BIT_BAND_SRAM(&a, 13) ^= 1; //инвертировать 13-й бит в "a", не задевая другие биты переменной (псевдо-атомарность)
#endif