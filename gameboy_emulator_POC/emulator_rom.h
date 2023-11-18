#ifndef ROM_H
#define ROM_H
#ifdef __cplusplus

extern "C" {

#endif
int rom_load(const char *);
int rom_init(const unsigned char *);
const unsigned char *rom_getbytes(void);
unsigned int rom_get_mapper(void);

enum {
	NROM,
	MBC1,
	MBC2,
	MMM01,
	MBC3,
	MBC4,
	MBC5,
};
#ifdef __cplusplus

  }

#endif /* end of __cplusplus */
#endif
