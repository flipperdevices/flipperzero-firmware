#pragma once

#ifdef __cplusplus
extern "C" {
#endif

static const unsigned char tpi_skey[] = { 0x12, 0x89, 0xAB, 0x45, 0xCD, 0xD8, 0x88, 0xFF };

/* registers */
#define TPI_REG_TPIIR	0x0F
#define TPI_REG_TPIPCR	0x02
#define TPI_REG_TPISR	0x00

#define TPI_REG_TPISR_NVMEN		(1 << 1)

/* TPI commands */
#define TPI_CMD_SLD		0x20
#define TPI_CMD_SLD_PI	0x24
#define TPI_CMD_SIN		0x10
#define TPI_CMD_SOUT	0x90
#define TPI_CMD_SSTCS	0xC0
#define TPI_CMD_SST		0x60
#define TPI_CMD_SST_PI	0x64

#define TPI_CMD_SLDCS	0x80
#define TPI_CMD_SSTPR	0x68
#define TPI_CMD_SKEY	0xE0

/* for TPI_CMD_SIN & TPI_CMD_SOUT */
#define TPI_SIO_ADDR(x) ((x & 0x30) << 1 | (x & 0x0F)) 

/* ATtiny4/5/9/10 I/O registers */
#define TPI_IOREG_NVMCSR		0x32
#define TPI_IOREG_NVMCMD		0x33

/* bit for NVMCSR */
#define TPI_IOREG_NVMCSR_NVMBSY	(1 << 7)

/* NVM commands */
#define TPI_NVMCMD_NO_OPERATION		0x00
#define TPI_NVMCMD_CHIP_ERASE		0x10
#define TPI_NVMCMD_SECTION_ERASE	0x14
#define TPI_NVMCMD_WORD_WRITE		0x1D

#ifdef __cplusplus
}
#endif

