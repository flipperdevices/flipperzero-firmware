#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>
#include <stdlib.h>

#include <lib/toolbox/args.h>
#include <lib/subghz/receiver.h>
#include <lib/subghz/subghz_keystore.h>
#include <lib/subghz/protocols/base.h>
#include <lib/subghz/protocols/raw.h>
#include <lib/subghz/protocols/princeton.h>
#include <lib/subghz/subghz_tx_rx_worker.h>

#include "constants.h"

#include <gui/icon_i.h>


const uint8_t _I_HappyFlipper_128x64_0[] = {0x01,0x00,0xc2,0x03,0xc9,0x49,0x29,0x20,0x2f,0x01,0x78,0x0b,0xc0,0x59,0xa4,0xa5,0x65,0x65,0x55,0x5d,0x55,0x53,0x53,0x52,0xd2,0x82,0x16,0x53,0x51,0xd1,0x51,0x31,0x31,0x29,0x2d,0x2b,0x29,0x28,0x04,0x36,0x2a,0x29,0x29,0xa9,0x69,0x29,0x19,0x19,0x15,0x14,0x10,0xba,0x41,0x54,0x82,0x12,0x13,0x21,0xa1,0x61,0x21,0x11,0x19,0x15,0x13,0x11,0x10,0x04,0x2d,0x4a,0x95,0xf5,0xff,0xd4,0xc9,0xf5,0xa4,0xc9,0x66,0x24,0x9b,0xf2,0x8d,0x52,0x6a,0x95,0x6a,0x4f,0xa4,0xa3,0x57,0xec,0x4c,0x95,0x06,0x39,0x3c,0x55,0x2a,0x55,0x2a,0xd4,0xea,0x34,0x9a,0x2c,0x96,0x2b,0xa9,0xd3,0xa5,0x49,0x69,0x3a,0xea,0x86,0x4a,0x85,0x49,0x85,0x49,0x65,0x39,0x4d,0x25,0x43,0x7b,0x56,0xc8,0xd2,0x3a,0x53,0xdc,0xac,0x93,0x33,0x92,0xb2,0x52,0xb2,0x52,0x9a,0x4c,0x9a,0x41,0xbd,0x92,0x65,0x6e,0x97,0x99,0x15,0x26,0x45,0x24,0xc4,0xe4,0x24,0x94,0x2c,0x95,0x22,0xeb,0xfe,0xa5,0x48,0xab,0xf6,0xaa,0xae,0x4a,0xa9,0x49,0xa9,0x0d,0xc4,0x4c,0x39,0x5e,0x56,0xa6,0xbf,0xff,0xc9,0x7b,0x31,0x15,0x4c,0x48,0x38,0x50,0x03,0x28,0xaa,0xc8,0xbe,0x14,0xff,0x94,0x92,0x53,0x97,0xd2,0xa5,0xc7,0x15,0x48,0x94,0xc8,0xb4,0xa1,0x71,0x9e,0xe7,0xf2,0x94,0xae,0x16,0x96,0x53,0x95,0xc5,0x55,0x25,0x55,0x2d,0x4d,0x20,0x2e,0x34,0xef,0x9c,0x8b,0x23,0x7c,0xe4,0xe4,0xe9,0x12,0x49,0x0e,0x47,0x09,0x44,0x89,0x44,0xa9,0x3f,0x4e,0x5f,0xff,0xef,0x27,0x8a,0xc9,0x74,0xac,0x71,0xa5,0x4a,0x65,0x3a,0x5d,0x26,0x52,0xcb,0x15,0xa4,0xd7,0xa9,0x5a,0x0a,0x4c,0x4b,0x4b,0x92,0xc9,0x04,0x46,0x54,0x12,0x1a,0xc6,0xa4,0xf0,0x3a,0x88,0xb6,0x4f,0x49,0x08,0xd2,0xc9,0x2a,0x49,0x91,0xa2,0xd0,0xb2,0xb4,0x3a,0x55,0x4b,0x0b,0xa8,0xd2,0xcf,0xea,0xb2,0x8a,0x4e,0x26,0x55,0x25,0x77,0x8e,0xab,0x28,0x96,0x19,0xf7,0x17,0xd3,0x72,0xd0,0x85,0x86,0xa5,0x44,0xa4,0xc4,0x72,0xbc,0x44,0x38,0xd5,0x6f,0xfa,0x9d,0x97,0x11,0x78,0x39,0x44,0x38,0xba,0x46,0x90,0x3e,0x1d,0x56,0x22,0x4f,0xf2,0xaf,0xea,0x95,0x43,0x49,0x58,0x9b,0xf0,0x44,0x63,0xa5,0xaa,0xff,0xe5,0x79,0x1a,0x89,0x88,0x5f,0x83,0x48,0x95,0x08,0x44,0x02,0x31,0x2d,0xcc,0x17,0x23,0xca,0x0c,0x0e,0xaa,0x55,0x51,0xd5,0x51,0x25,0x3c,0x96,0x90,0xdf,0xe9,0xda,0x9a,0x57,0xf3,0x2b,0x54,0xa2,0x51,0x31,0x51,0x29,0x4d,0x0a,0xa4,0xd2,0x1e,0x4c,0x42,0xa5,0x24,0xff,0xb2,0xc7,0x25,0x52,0xa8,0xe9,0x01,0xa3,0x64,0xc4,0xf5,0x13,0x63,0x84,0xe9,0x66,0xea,0x81,0xb1,0x8a,0xea,0xa9,0x58,0x4c,0x41,0x50,0x71,0x20,0x61,0x94,0xff,0xe5,0xd8,0x9d,0x25,0x13,0x91,0x04,0xc8,0xca,0xaa,0xbe,0xa9,0x27,0x4a,0x89,0x28,0xf5,0x6a,0xe9,0x75,0x2a,0x4e,0xaa,0x25,0x69,0x28,0x8c,0x89,0x26,0x3a,0x5d,0x4e,0x4b,0xe5,0xff,0xca,0xa3,0xc6,0x20,0x41,0x12,0x88,0xe5,0x2d,0x39,0x2a,0x45,0xd7,0x4b,0x7f,0xc9,0x7d,0xf2,0x32,0x4d,0x2d,0x22,0x49,0xa9,0xd2,0x48,0xba,0x52,0x48,0xaf,0x0b,0x93,0x4f,0xd4,0xfb,0x2a,0xe7,0x31,0x11,0x89,0x4c,0x75,0x36,0x4d,0x4f,0x22,0xa9,0xef,0x3b,0x0e,0xea,0x19,0x0a,0x88,0xc8,0xb2,0x29,0xc1,0x1f,0x0c,0x2a,0x4b,0x89,0xfe,0x64,0xba,0x93,0xfb,0xea,0x70,0x6a,0x92,0xaa,0x4b,0x48,0x7a,0x5f,0x42,0xd0,0xe4,0x7f,0xf7,0x5f,0x4f,0xc3,0x2f,0x44,0xc4,0xd1,0x06,0x23,0x4a,0xb2,0x52,0x6f,0xd2,0x52,0xd8,0xef,0xf4,0x9c,0x59,0x74,0xa7,0x4b,0x52,0x18,0x8e,0x52,0x93,0x92,0xe9,0x65,0xf4,0x92,0x45,0x98,0xf5,0x2b,0x34,0x58,0x96,0x24,0x62,0x32,0x1b,0x53,0xcc,0x6b,0x8f,0x91,0xa4,0xf4,0xa2,0x52,0x3a,0x96,0xa6,0xa8,0x31,0x1a,0x51,0x40,0x51,0xa2,0xf9,0x1c,0xa5,0xd3,0x4b,0xee,0x8a,0x48,0xb1,0x28,0x01,0xa9,0x23,0xc5,0xe0,0x3a,0xbf,0xf4,0x9f,0x7f,0x48,0xcd,0x68,0xea,0x95,0x2a,0x94,0x4b,0x2b,0xb2,0x92,0x52,0x74,0x9e,0xea,0xbf,0xf6,0x5f,0x95,0x9a,0x53,0x70,0xaa,0x21,0xd2,0x48,0x36,0x89,0x61,0x89,0x7e,0x75,0xbe,0xfd,0xdc,0x82,0xf3,0x9a,0x94,0x52,0x61,0x11,0x5a,0x6c,0xe6,0xa3,0xa2,0xd2,0x7a,0xbf,0xf5,0x79,0xbd,0x36,0xc5,0x31,0xaa,0xe5,0x75,0x54,0x8c,0x95,0x4b,0x15,0x14,0xa4,0xe4,0x2f,0x3f,0xfa,0x56,0x46,0xe9,0x6a,0x90,0xe8,0x81,0xa3,0x29,0xaa,0x54,0x5d,0xc3,0xa6,0x93,0x8e,0x47,0x4b,0xe2,0xe2,0x53,0x65,0x32,0x49,0x2d,0x0b,0x33,0x14,0xa1,0x55,0x29,0x07,0xf1,0xa9,0x4b,0xa6,0xb2,0x9c,0x99,0xec,0x69,0x15,0x59,0x96,0x97,0x29,0x22,0x8a,0xe5,0x72,0x58,0x4d,0xd5,0xe3,0x25,0x2c,0x94,0xe5,0x65,0x59,0x2a,0xa6,0x4a,0x55,0x95,0xd1,0x2e,0x87,0x55,0xe0,0xcd,0xc9,0x2b,0x34,0xad,0x15,0x0a,0x93,0x99,0x92,0x68,0x69,0x1f,0x4a,0x15,0x22,0x21,0x15,0x97,0xd5,0x4d,0x03,0x14,0xcc,0x87,0x46,0x92,0xdd,0x1b,0xf0,0x9c,0x51,0x23,0x54,0xc1,0x75,0x22,0x5a,0x99,0x24,0x93,0x58,0x62,0x19,0x2b,0xb4,0x68,0x95,0x5a,0x4e,0x06,0x68,0x63,0x1a,0x15,0x48,0xb2,0x32,0x4c,0x97,0xa2,0x5f,0x29,0x94,0xc8,0xa2,0x55,0xaa,0x4c,0x2b,0x2f,0x94,0xc2,0x6a,0xb3,0x54,0xa9,0x2a,0x74,0x50,0xe3,0x49,0xd2,0xc8,0xb2,0x55,0x03,0x00,0xca,0x62,0x32,0xcc,0x8f,0xff,0xff,0x84,0x92,0x61,0x32,0x8b,0xc1,0x92,0xb6,0xc6,0xa5,0x4a,0xa5,0x52,0x57,0x83,0xbf,0x29,0x8d,0x56,0x45,0x45,0x95,0x52,0xa5,0x36,0x9c,0x25,0x4a,0xa3,0x9e,0xb2,0x7f,0xf2,0x75,0x0d,0x56,0xa2,0xa9,0xaa,0xa2,0x64,0x68,0x58,0x22,0xf8,0xe2,0xa5,0x94,0x99,0x14,0x97,0x4b,0x04,0x82,0xe2,0x62,0x06,0x61,0xaa,0x4d,0x69,0x62,0x31,0xaa,0xe4,0xb2,0x94,0x88,0x55,0xa7,0x55,0x52,0xd2,0xd4,0xaa,0x92,0x5a,0x26,0x57,0x21,0x52,};
const uint8_t* const _I_HappyFlipper_128x64[] = {_I_HappyFlipper_128x64_0};

const uint8_t _I_TanksSplashScreen_128x64_0[] = {0x01,0x00,0xbe,0x03,0xc9,0x49,0x29,0x20,0x2f,0x01,0x78,0x0b,0xc0,0x59,0xa4,0xa5,0x65,0x65,0x55,0x5d,0x55,0x53,0x53,0x52,0xd2,0x82,0x16,0x53,0x51,0xd1,0x51,0x31,0x31,0x29,0x2d,0x2b,0x29,0x28,0x04,0x36,0x2a,0x29,0x29,0xa9,0x69,0x29,0x19,0x19,0x15,0x14,0x10,0xba,0x41,0x54,0x82,0x12,0x13,0x21,0xa1,0x61,0x21,0x11,0x19,0x15,0x13,0x11,0x10,0x04,0x2d,0x4a,0xb4,0x4f,0xdd,0x25,0xeb,0xd1,0xff,0xd3,0x94,0x6a,0x93,0xcc,0xb9,0x64,0xa3,0xc5,0x1a,0xc9,0x56,0xb3,0xb2,0x4f,0x4e,0x4b,0x15,0x4a,0x95,0x4a,0xb5,0x3a,0x8d,0x26,0x88,0x10,0x35,0x4a,0x64,0x99,0x28,0x34,0xbd,0x26,0x42,0x85,0x49,0x85,0x49,0x65,0x39,0x4d,0x25,0x43,0x13,0xea,0xa5,0xa8,0xc7,0x2d,0x12,0xaa,0xd5,0x32,0x56,0x4a,0x56,0x4a,0x53,0x49,0x93,0x54,0xa5,0x99,0x19,0x24,0xa6,0xbf,0xa9,0x91,0x62,0x64,0x98,0x9c,0x84,0x92,0x85,0x92,0xa4,0x79,0x3d,0x66,0x51,0xa9,0x4f,0xf2,0x59,0x4a,0x95,0x26,0xa5,0x25,0xa4,0xe5,0x64,0x99,0x59,0xcf,0xfe,0x8c,0x32,0x1f,0x48,0x88,0x64,0xa0,0xe1,0x40,0x0c,0xa2,0xab,0x22,0xd1,0x6a,0x33,0xfa,0x4d,0xe7,0xea,0xab,0x42,0xa4,0x65,0x69,0x12,0x99,0x16,0x97,0x11,0x54,0xba,0x6e,0xbf,0xdf,0xfa,0x7e,0x73,0x07,0x94,0x95,0x48,0x72,0xb5,0x2d,0x4d,0x23,0x4b,0x22,0xf2,0xcf,0xe4,0xf4,0x99,0x45,0x5e,0x6f,0xa9,0xa2,0x52,0xa4,0x58,0x4a,0x24,0x4a,0x25,0x49,0xc4,0x7b,0x0d,0xe3,0xf2,0x92,0xd2,0xe2,0x5a,0x9c,0xa5,0x52,0x53,0x29,0xd2,0xe9,0x32,0x95,0x5c,0x57,0xfb,0xdf,0x9f,0xa4,0x64,0xe9,0x54,0x59,0x4e,0x26,0xa4,0xa4,0x10,0x90,0xc8,0xaa,0xb9,0x2b,0xe5,0x4b,0x2b,0x4a,0x92,0x44,0xa8,0x4c,0x31,0xca,0x52,0x28,0xb4,0x2c,0xa6,0x52,0x49,0x41,0x84,0xc8,0xb2,0x44,0xb1,0xaa,0x49,0x62,0xb2,0x19,0x2b,0xbc,0x69,0x28,0x51,0xcd,0x55,0xea,0xb2,0x7a,0x2b,0x24,0x69,0x14,0x9a,0x55,0x26,0x23,0x95,0x89,0x6a,0xb2,0x5e,0x4d,0x3e,0x22,0x97,0xa8,0x92,0x54,0x65,0x42,0x41,0x64,0x8d,0x23,0x48,0xd6,0x1f,0x4d,0x36,0xa5,0x53,0x89,0x87,0x07,0x22,0x3f,0x19,0x20,0xb4,0x75,0x35,0x2a,0x47,0xd7,0x3f,0x94,0x93,0xe9,0x2a,0x59,0x11,0x68,0xc8,0xb5,0x31,0x2c,0x94,0x4b,0x4f,0x52,0xf9,0x4b,0xef,0xb4,0xfa,0x4c,0x52,0x54,0xcb,0x1a,0xa4,0x4b,0xa9,0x24,0xd2,0x52,0xb1,0x59,0x4e,0x26,0x5a,0xb1,0x2a,0xd2,0xb8,0x05,0xe6,0x3a,0x48,0x4d,0x26,0x93,0x92,0xaa,0x7f,0x77,0x32,0x2f,0xeb,0xe0,0x70,0x80,0x81,0x79,0x91,0x48,0x64,0xb2,0x19,0x36,0x4e,0xa9,0xa2,0xaf,0xe5,0x65,0x52,0xac,0x4c,0x90,0x4a,0x2b,0x51,0xaa,0xd2,0xa9,0x72,0x4c,0x4e,0x52,0xe9,0x22,0xa8,0xe8,0x43,0xc3,0x91,0x41,0x8e,0x27,0x12,0x77,0x1a,0x50,0x24,0x7a,0x55,0x4c,0x55,0x30,0x3c,0x34,0xa2,0xf8,0x9e,0x86,0x89,0xa7,0xd4,0x67,0x34,0x96,0x41,0x60,0x9c,0x07,0x23,0x92,0xd5,0x19,0x44,0xb0,0x39,0x68,0x95,0xde,0x13,0x79,0x60,0x0c,0xa8,0x20,0x32,0x5c,0x8d,0x02,0x93,0x09,0xc4,0xf4,0xb4,0xbc,0xaa,0x97,0x7b,0x17,0x12,0xa0,0xca,0x9b,0xe3,0x4a,0xd6,0xe4,0xaa,0x75,0x29,0x24,0x5f,0xa7,0xab,0xd3,0x55,0xb4,0x9a,0xca,0x24,0x87,0x2a,0xf6,0x1a,0xa1,0x88,0x69,0x57,0xe9,0x77,0xfa,0xab,0xbd,0x25,0x8e,0xa7,0x55,0xa0,0x2c,0x0e,0x26,0x93,0x29,0xc8,0xfa,0x3d,0xb9,0xfb,0x27,0xd5,0x9c,0x31,0x28,0x95,0x37,0x08,0xc7,0x1e,0xad,0x3e,0x55,0x25,0xdf,0xc9,0x37,0x3c,0x49,0xfd,0x27,0x49,0xa4,0xc8,0x4a,0xa9,0x2e,0x21,0xf9,0x51,0x40,0x85,0x3e,0xd4,0xdd,0xf2,0xae,0x01,0x92,0xd4,0x62,0x54,0x9c,0x9e,0x52,0xbf,0x92,0xe5,0x7f,0xc0,0x63,0xd6,0x90,0x44,0x62,0xba,0xa1,0x10,0xaf,0xc6,0x93,0xf4,0x92,0x7c,0x76,0x7a,0xf9,0x26,0xb7,0x57,0xa4,0xd4,0x51,0x21,0x55,0x5a,0xa3,0x80,0x72,0x5f,0xb9,0xfd,0x16,0x9d,0x49,0xcd,0x42,0xe5,0x34,0x59,0x4d,0x45,0x7c,0x39,0x31,0xe0,0xd3,0xaf,0xfa,0xa8,0xf6,0xaa,0xc9,0x6e,0x38,0x8d,0x16,0x55,0x52,0xe9,0x69,0x29,0x59,0x29,0xd6,0x26,0x21,0x59,0x88,0xfa,0x73,0x43,0xc1,0x18,0x0c,0x97,0xc9,0x4a,0xca,0x69,0x3c,0x92,0xfd,0x2f,0xd3,0x4b,0xd2,0xb2,0x42,0x65,0x32,0x5d,0x4d,0x23,0x93,0x28,0x90,0x90,0x46,0xde,0x23,0x1a,0xff,0x13,0x53,0x52,0xa2,0x8c,0x47,0x2b,0x52,0xc9,0xea,0xb2,0x5a,0x4c,0xb0,0x8c,0x7e,0x9a,0x9a,0x2c,0x97,0x25,0x90,0xd2,0x48,0x2a,0x84,0xe1,0xaa,0x4a,0x65,0xa2,0x31,0x89,0x62,0xe5,0x35,0x77,0x90,0xc5,0x69,0xbc,0x91,0x50,0xc8,0xb2,0x5d,0x4b,0xfe,0x57,0x49,0xfe,0xab,0xd3,0x0b,0xe3,0xa2,0xc9,0x54,0xb2,0x32,0x47,0x28,0xe9,0x74,0x92,0x82,0x20,0xe3,0x36,0x45,0xf1,0xca,0x49,0x2c,0x8c,0x21,0xa4,0x52,0x7d,0xdc,0xbb,0xff,0xf6,0x4f,0x2f,0xb4,0x8d,0x06,0x53,0x49,0xe4,0xd1,0x09,0xe3,0xd2,0x99,0xe4,0x68,0x59,0x2d,0x76,0xa1,0x50,0x34,0xa0,0x30,0xcc,0xb5,0x31,0x29,0x2f,0xd3,0x8b,0x7f,0xd4,0xc9,0x3f,0xb1,0x7d,0x24,0xae,0xa2,0x3a,0x1b,0x23,0x84,0x69,0x5f,0xdd,0xd6,0x92,0x17,0xc9,0xc5,0xdf,0x22,0xba,0x69,0x25,0x4b,0x53,0xa9,0x52,0x8f,0x43,0x7f,0x14,0xa4,0xd5,0xaa,0xb5,0x2a,0x16,0x86,0x39,0x49,0x20,0x89,0x38,0x43,0xc3,0x5f,0xf2,0xea,0xa4,0x96,0x2d,0x5d,0x6a,0x95,0x71,0x95,0x26,0x86,0xc9,0x92,0xfe,0x7f,0xe5,0x7b,0xed,0xd5,0x3e,0x93,0xab,0xed,0x45,0x39,0x59,0x20,0xe8,0xea,0x6a,0x9f,0xff,0xfd,0x0a,0x93,0x45,0xa9,0x6a,0xb5,0x52,0xad,0x4e,0x4b,0x4b,0x90,0x2f,0x8e,0x17,0xfb,0x4f,0xca,0x7d,0x3f,0xf2,0x5c,0x8f,0xfe,0xbf,0x22,0x94,0xc2,0xe4,0xb2,0xab,0xa6,0xca,0x24,0xd8,0x1c,0x93,0x10,0x47,0x45,0x54,0xa1,0x5b,0x29,0x55,0x10,};
const uint8_t* const _I_TanksSplashScreen_128x64[] = {_I_TanksSplashScreen_128x64_0};

const uint8_t _I_enemy_down_0[] = {0x00,0x21,0x3f,0x33,0x3f,0x2d,0x0c,};
const uint8_t* const _I_enemy_down[] = {_I_enemy_down_0};

const uint8_t _I_enemy_left_0[] = {0x00,0x3e,0x1c,0x17,0x17,0x1c,0x3e,};
const uint8_t* const _I_enemy_left[] = {_I_enemy_left_0};

const uint8_t _I_enemy_right_0[] = {0x00,0x1f,0x0e,0x3a,0x3a,0x0e,0x1f,};
const uint8_t* const _I_enemy_right[] = {_I_enemy_right_0};

const uint8_t _I_enemy_up_0[] = {0x00,0x0c,0x2d,0x3f,0x33,0x3f,0x21,};
const uint8_t* const _I_enemy_up[] = {_I_enemy_up_0};

const uint8_t _I_projectile_down_0[] = {0x00,0x00,0x12,0x1e,0x1e,0x0c,0x00,};
const uint8_t* const _I_projectile_down[] = {_I_projectile_down_0};

const uint8_t _I_projectile_left_0[] = {0x00,0x00,0x1c,0x0e,0x0e,0x1c,0x00,};
const uint8_t* const _I_projectile_left[] = {_I_projectile_left_0};

const uint8_t _I_projectile_right_0[] = {0x00,0x00,0x0e,0x1c,0x1c,0x0e,0x00,};
const uint8_t* const _I_projectile_right[] = {_I_projectile_right_0};

const uint8_t _I_projectile_up_0[] = {0x00,0x00,0x0c,0x1e,0x1e,0x12,0x00,};
const uint8_t* const _I_projectile_up[] = {_I_projectile_up_0};

const uint8_t _I_tank_base_0[] = {0x00,0x21,0x33,0x0c,0x1e,0x0c,0x3f,};
const uint8_t* const _I_tank_base[] = {_I_tank_base_0};

const uint8_t _I_tank_down_0[] = {0x00,0x21,0x3f,0x3f,0x3f,0x2d,0x0c,};
const uint8_t* const _I_tank_down[] = {_I_tank_down_0};

const uint8_t _I_tank_explosion_0[] = {0x00,0x1a,0x25,0x16,0x29,0x15,0x2a,};
const uint8_t* const _I_tank_explosion[] = {_I_tank_explosion_0};

const uint8_t _I_tank_hedgehog_0[] = {0x00,0x21,0x12,0x0c,0x0c,0x12,0x21,};
const uint8_t* const _I_tank_hedgehog[] = {_I_tank_hedgehog_0};

const uint8_t _I_tank_left_0[] = {0x00,0x3e,0x1c,0x1f,0x1f,0x1c,0x3e,};
const uint8_t* const _I_tank_left[] = {_I_tank_left_0};

const uint8_t _I_tank_right_0[] = {0x00,0x1f,0x0e,0x3e,0x3e,0x0e,0x1f,};
const uint8_t* const _I_tank_right[] = {_I_tank_right_0};

const uint8_t _I_tank_stone_0[] = {0x00,0x12,0x3f,0x1e,0x1e,0x3f,0x12,};
const uint8_t* const _I_tank_stone[] = {_I_tank_stone_0};

const uint8_t _I_tank_up_0[] = {0x00,0x0c,0x2d,0x3f,0x3f,0x3f,0x21,};
const uint8_t* const _I_tank_up[] = {_I_tank_up_0};

const uint8_t _I_tank_wall_0[] = {0x00,0x3f,0x2d,0x3f,0x3f,0x2d,0x3f,};
const uint8_t* const _I_tank_wall[] = {_I_tank_wall_0};

const Icon I_HappyFlipper_128x64 = {.width=128,.height=64,.frame_count=1,.frame_rate=0,.frames=_I_HappyFlipper_128x64};
const Icon I_TanksSplashScreen_128x64 = {.width=128,.height=64,.frame_count=1,.frame_rate=0,.frames=_I_TanksSplashScreen_128x64};
const Icon I_enemy_down = {.width=6,.height=6,.frame_count=1,.frame_rate=0,.frames=_I_enemy_down};
const Icon I_enemy_left = {.width=6,.height=6,.frame_count=1,.frame_rate=0,.frames=_I_enemy_left};
const Icon I_enemy_right = {.width=6,.height=6,.frame_count=1,.frame_rate=0,.frames=_I_enemy_right};
const Icon I_enemy_up = {.width=6,.height=6,.frame_count=1,.frame_rate=0,.frames=_I_enemy_up};
const Icon I_projectile_down = {.width=6,.height=6,.frame_count=1,.frame_rate=0,.frames=_I_projectile_down};
const Icon I_projectile_left = {.width=6,.height=6,.frame_count=1,.frame_rate=0,.frames=_I_projectile_left};
const Icon I_projectile_right = {.width=6,.height=6,.frame_count=1,.frame_rate=0,.frames=_I_projectile_right};
const Icon I_projectile_up = {.width=6,.height=6,.frame_count=1,.frame_rate=0,.frames=_I_projectile_up};
const Icon I_tank_base = {.width=6,.height=6,.frame_count=1,.frame_rate=0,.frames=_I_tank_base};
const Icon I_tank_down = {.width=6,.height=6,.frame_count=1,.frame_rate=0,.frames=_I_tank_down};
const Icon I_tank_explosion = {.width=6,.height=6,.frame_count=1,.frame_rate=0,.frames=_I_tank_explosion};
const Icon I_tank_hedgehog = {.width=6,.height=6,.frame_count=1,.frame_rate=0,.frames=_I_tank_hedgehog};
const Icon I_tank_left = {.width=6,.height=6,.frame_count=1,.frame_rate=0,.frames=_I_tank_left};
const Icon I_tank_right = {.width=6,.height=6,.frame_count=1,.frame_rate=0,.frames=_I_tank_right};
const Icon I_tank_stone = {.width=6,.height=6,.frame_count=1,.frame_rate=0,.frames=_I_tank_stone};
const Icon I_tank_up = {.width=6,.height=6,.frame_count=1,.frame_rate=0,.frames=_I_tank_up};
const Icon I_tank_wall = {.width=6,.height=6,.frame_count=1,.frame_rate=0,.frames=_I_tank_wall};

typedef struct {
    //    +-----x
    //    |
    //    |
    //    y
    uint8_t x;
    uint8_t y;
} Point;

typedef enum {
    CellEmpty = 1,
    CellWall,
    CellExplosion,
    CellTankUp,
    CellTankRight,
    CellTankDown,
    CellTankLeft,
    CellEnemyUp,
    CellEnemyRight,
    CellEnemyDown,
    CellEnemyLeft,
    CellProjectileUp,
    CellProjectileRight,
    CellProjectileDown,
    CellProjectileLeft,
} GameCellState;

typedef enum {
    MenuStateSingleMode,
    MenuStateCooperativeServerMode,
    MenuStateCooperativeClientMode,
} MenuState;

typedef enum {
    GameStateMenu,
    GameStateSingle,
    GameStateCooperativeServer,
    GameStateCooperativeClient,
    GameStateGameOver,
} GameState;

typedef enum {
    DirectionUp,
    DirectionRight,
    DirectionDown,
    DirectionLeft,
} Direction;

typedef enum {
    ModeSingle,
    ModeCooperative,
} Mode;

typedef struct {
    Point coordinates;
    Direction direction;
    bool explosion;
    bool is_p1;
    bool is_p2;
} ProjectileState;

typedef struct {
    Point coordinates;
    uint16_t score;
    uint8_t lives;
    Direction direction;
    bool moving;
    bool shooting;
    bool live;
    uint8_t cooldown;
    uint8_t respawn_cooldown;
} PlayerState;

typedef struct {
    // char map[FIELD_WIDTH][FIELD_HEIGHT];
    char thisMap[16][11];
    Point team_one_respawn_points[3];
    Point team_two_respawn_points[3];
    Mode mode;
    bool server;
    GameState state;
    MenuState menu_state;
    ProjectileState* projectiles[100];
    PlayerState* bots[6];
    uint8_t enemies_left;
    uint8_t enemies_live;
    uint8_t enemies_respawn_cooldown;
    uint8_t received;
    uint8_t sent;
    PlayerState* p1;
    PlayerState* p2;
} TanksState;

typedef enum {
    EventTypeTick,
    EventTypeKey,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} TanksEvent;

typedef enum {
    GoesUp,
    GoesRight,
    GoesDown,
    GoesLeft,
    Shoots,
} ClientAction;

//char map[FIELD_HEIGHT][FIELD_WIDTH + 1] = {
char thisMap[11][16 + 1] = {
    "*       -  *   -",
    "  -  -  =       ",
    "        -  -   2",
    "1    =     - -- ",
    "--   =     - -- ",
    "a-1  =  -  =   2",
    "--   =     - -- ",
    "1    =     - -- ",
    "        -  -   2",
    "  -  -  =       ",
    "*       -  *   -",
};

static void tanks_game_write_cell(unsigned char* data, int8_t x, int8_t y, GameCellState cell) {
    uint8_t index = y * 16 + x;
    data[index] = cell;
    //    if (x % 2) {
    //        data[index] = (data[index] & 0b00001111) + (cell << 4);
    //    } else {
    //        data[index] = (data[index] & 0b0000) + cell;
    //    }
}

// Enum with < 16 items => 4 bits in cell, 2 cells in byte
unsigned char* tanks_game_serialize(const TanksState* const tanks_state) {
    static unsigned char result[11 * 16 + 1];

    for(int8_t x = 0; x < FIELD_WIDTH; x++) {
        for(int8_t y = 0; y < FIELD_HEIGHT; y++) {
            result[(y * FIELD_WIDTH + x)] = 0;

            GameCellState cell = CellEmpty;

            if(tanks_state->thisMap[x][y] == '-') {
                cell = CellWall;

                tanks_game_write_cell(result, x, y, cell);
            }
        }
    }

    for(uint8_t i = 0; i < 6; i++) {
        if(tanks_state->bots[i] != NULL) {
            GameCellState cell = CellEmpty;

            switch(tanks_state->bots[i]->direction) {
            case DirectionUp:
                cell = CellEnemyUp;
                break;
            case DirectionDown:
                cell = CellEnemyDown;
                break;
            case DirectionRight:
                cell = CellEnemyRight;
                break;
            case DirectionLeft:
                cell = CellEnemyLeft;
                break;
            }

            tanks_game_write_cell(
                result,
                tanks_state->bots[i]->coordinates.x,
                tanks_state->bots[i]->coordinates.y,
                cell);
        }
    }

    for(int8_t x = 0; x < 100; x++) {
        if(tanks_state->projectiles[x] != NULL) {
            GameCellState cell = CellEmpty;

            switch(tanks_state->projectiles[x]->direction) {
            case DirectionUp:
                cell = CellProjectileUp;
                break;
            case DirectionDown:
                cell = CellProjectileDown;
                break;
            case DirectionRight:
                cell = CellProjectileRight;
                break;
            case DirectionLeft:
                cell = CellProjectileLeft;
                break;
            }

            tanks_game_write_cell(
                result,
                tanks_state->projectiles[x]->coordinates.x,
                tanks_state->projectiles[x]->coordinates.y,
                cell);
        }
    }

    if(tanks_state->p1 != NULL && tanks_state->p1->live) {
        GameCellState cell = CellEmpty;

        switch(tanks_state->p1->direction) {
        case DirectionUp:
            cell = CellTankUp;
            break;
        case DirectionDown:
            cell = CellTankDown;
            break;
        case DirectionRight:
            cell = CellTankRight;
            break;
        case DirectionLeft:
            cell = CellTankLeft;
            break;
        }

        tanks_game_write_cell(
            result, tanks_state->p1->coordinates.x, tanks_state->p1->coordinates.y, cell);
    }

    if(tanks_state->p2 != NULL && tanks_state->p2->live) {
        GameCellState cell = CellEmpty;

        switch(tanks_state->p2->direction) {
        case DirectionUp:
            cell = CellTankUp;
            break;
        case DirectionDown:
            cell = CellTankDown;
            break;
        case DirectionRight:
            cell = CellTankRight;
            break;
        case DirectionLeft:
            cell = CellTankLeft;
            break;
        }

        tanks_game_write_cell(
            result, tanks_state->p2->coordinates.x, tanks_state->p2->coordinates.y, cell);
    }

    return result;
}

static void
    tanks_game_render_cell(GameCellState cell, uint8_t x, uint8_t y, Canvas* const canvas) {
    const Icon* icon;

    if(cell == CellEmpty) {
        return;
    }

    switch(cell) {
    case CellWall:
        icon = &I_tank_wall;
        break;
    case CellExplosion:
        icon = &I_tank_explosion;
        break;
    case CellTankUp:
        icon = &I_tank_up;
        break;
    case CellTankRight:
        icon = &I_tank_right;
        break;
    case CellTankDown:
        icon = &I_tank_down;
        break;
    case CellTankLeft:
        icon = &I_tank_left;
        break;
    case CellEnemyUp:
        icon = &I_enemy_up;
        break;
    case CellEnemyRight:
        icon = &I_enemy_right;
        break;
    case CellEnemyDown:
        icon = &I_enemy_down;
        break;
    case CellEnemyLeft:
        icon = &I_enemy_left;
        break;
    case CellProjectileUp:
        icon = &I_projectile_up;
        break;
    case CellProjectileRight:
        icon = &I_projectile_right;
        break;
    case CellProjectileDown:
        icon = &I_projectile_down;
        break;
    case CellProjectileLeft:
        icon = &I_projectile_left;
        break;
    default:
        return;
        break;
    }

    canvas_draw_icon(canvas, x * CELL_LENGTH_PIXELS, y * CELL_LENGTH_PIXELS - 1, icon);
}

static void tanks_game_render_constant_cells(Canvas* const canvas) {
    for(int8_t x = 0; x < FIELD_WIDTH; x++) {
        for(int8_t y = 0; y < FIELD_HEIGHT; y++) {
            char cell = thisMap[y][x];

            if(cell == '=') {
                canvas_draw_icon(
                    canvas, x * CELL_LENGTH_PIXELS, y * CELL_LENGTH_PIXELS - 1, &I_tank_stone);
                continue;
            }

            if(cell == '*') {
                canvas_draw_icon(
                    canvas, x * CELL_LENGTH_PIXELS, y * CELL_LENGTH_PIXELS - 1, &I_tank_hedgehog);
                continue;
            }

            if(cell == 'a') {
                canvas_draw_icon(
                    canvas, x * CELL_LENGTH_PIXELS, y * CELL_LENGTH_PIXELS - 1, &I_tank_base);
                continue;
            }
        }
    }
}

void tanks_game_deserialize_and_write_to_state(unsigned char* data, TanksState* const tanks_state) {
    for(uint8_t i = 0; i < 11 * 16; i++) {
        uint8_t x = i % 16;
        uint8_t y = i / 16;
        tanks_state->thisMap[x][y] = data[i];
    }
}

void tanks_game_deserialize_and_render(unsigned char* data, Canvas* const canvas) {
    //for (uint8_t i = 0; i < 11 * 16 / 2; i++) {
    for(uint8_t i = 0; i < 11 * 16; i++) {
        char cell = data[i];
        uint8_t x = i % 16; // One line (16 cells) = 8 bytes
        uint8_t y = i / 16;

        //        GameCellState first = cell >> 4;
        //        GameCellState second = cell & 0b00001111;

        tanks_game_render_cell(cell, x, y, canvas);
        //        tanks_game_render_cell(second, x + 1, y, canvas);
    }

    tanks_game_render_constant_cells(canvas);
}

static void tanks_game_render_callback(Canvas* const canvas, void* ctx) {
    const TanksState* tanks_state = acquire_mutex((ValueMutex*)ctx, 25);
    if(tanks_state == NULL) {
        return;
    }

    // Before the function is called, the state is set with the canvas_reset(canvas)
    if(tanks_state->state == GameStateMenu) {
        canvas_draw_icon(canvas, 0, 0, &I_TanksSplashScreen_128x64);
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str_aligned(canvas, 124, 10, AlignRight, AlignBottom, "Single");
        canvas_draw_str_aligned(canvas, 124, 25, AlignRight, AlignBottom, "Co-op S");
        canvas_draw_str_aligned(canvas, 124, 40, AlignRight, AlignBottom, "Co-op C");

        switch(tanks_state->menu_state) {
        case MenuStateSingleMode:
            canvas_draw_icon(canvas, 74, 3, &I_tank_right);
            break;
        case MenuStateCooperativeServerMode:
            canvas_draw_icon(canvas, 74, 18, &I_tank_right);
            break;
        case MenuStateCooperativeClientMode:
            canvas_draw_icon(canvas, 74, 33, &I_tank_right);
            break;
        }

        canvas_draw_frame(canvas, 0, 0, 128, 64);

        release_mutex((ValueMutex*)ctx, tanks_state);
        return;
    }

    // Field right border
    canvas_draw_box(canvas, FIELD_WIDTH * CELL_LENGTH_PIXELS, 0, 2, SCREEN_HEIGHT);

    // Cooperative client
    if(tanks_state->mode == ModeCooperative && !tanks_state->server) {
        for(int8_t x = 0; x < FIELD_WIDTH; x++) {
            for(int8_t y = 0; y < FIELD_HEIGHT; y++) {
                tanks_game_render_cell(tanks_state->thisMap[x][y], x, y, canvas);
            }
        }

        tanks_game_render_constant_cells(canvas);

        release_mutex((ValueMutex*)ctx, tanks_state);
        return;
    }

    // Player
    //    Point coordinates = tanks_state->p1->coordinates;
    //    const Icon *icon;
    //    switch (tanks_state->p1->direction) {
    //    case DirectionUp:
    //        icon = &I_tank_up;
    //        break;
    //    case DirectionDown:
    //        icon = &I_tank_down;
    //        break;
    //    case DirectionRight:
    //        icon = &I_tank_right;
    //        break;
    //    case DirectionLeft:
    //        icon = &I_tank_left;
    //        break;
    //    default:
    //        icon = &I_tank_explosion;
    //    }

    //    if (tanks_state->p1->live) {
    //        canvas_draw_icon(canvas, coordinates.x * CELL_LENGTH_PIXELS, coordinates.y * CELL_LENGTH_PIXELS - 1, icon);
    //    }
    //
    //    for(int8_t x = 0; x < FIELD_WIDTH; x++) {
    //        for(int8_t y = 0; y < FIELD_HEIGHT; y++) {
    //            switch (tanks_state->thisMap[x][y]) {
    //            case '-':
    //                canvas_draw_icon(canvas, x * CELL_LENGTH_PIXELS, y * CELL_LENGTH_PIXELS - 1, &I_tank_wall);
    //                break;
    //
    //            case '=':
    //                canvas_draw_icon(canvas, x * CELL_LENGTH_PIXELS, y * CELL_LENGTH_PIXELS - 1, &I_tank_stone);
    //                break;
    //
    //            case '*':
    //                canvas_draw_icon(canvas, x * CELL_LENGTH_PIXELS, y * CELL_LENGTH_PIXELS - 1, &I_tank_hedgehog);
    //                break;
    //
    //            case 'a':
    //                canvas_draw_icon(canvas, x * CELL_LENGTH_PIXELS, y * CELL_LENGTH_PIXELS - 1, &I_tank_base);
    //                break;
    //            }
    //        }
    //    }

    //    for (
    //        uint8_t i = 0;
    //        i < 6;
    //        i++
    //    ) {
    //        if (tanks_state->bots[i] != NULL) {
    //            const Icon *icon;
    //
    //            switch(tanks_state->bots[i]->direction) {
    //            case DirectionUp:
    //                icon = &I_enemy_up;
    //                break;
    //            case DirectionDown:
    //                icon = &I_enemy_down;
    //                break;
    //            case DirectionRight:
    //                icon = &I_enemy_right;
    //                break;
    //            case DirectionLeft:
    //                icon = &I_enemy_left;
    //                break;
    //            default:
    //                icon = &I_tank_explosion;
    //            }
    //
    //            canvas_draw_icon(
    //                canvas,
    //                tanks_state->bots[i]->coordinates.x * CELL_LENGTH_PIXELS,
    //                tanks_state->bots[i]->coordinates.y * CELL_LENGTH_PIXELS - 1,
    //                icon);
    //        }
    //    }

    //    for(int8_t x = 0; x < 100; x++) {
    //        if (tanks_state->projectiles[x] != NULL) {
    //            ProjectileState *projectile = tanks_state->projectiles[x];
    //
    //            if (projectile->explosion) {
    //                canvas_draw_icon(
    //                    canvas,
    //                    projectile->coordinates.x * CELL_LENGTH_PIXELS,
    //                    projectile->coordinates.y * CELL_LENGTH_PIXELS - 1,
    //                    &I_tank_explosion);
    //                continue;
    //            }
    //
    //            const Icon *icon;
    //
    //            switch(projectile->direction) {
    //            case DirectionUp:
    //                icon = &I_projectile_up;
    //                break;
    //            case DirectionDown:
    //                icon = &I_projectile_down;
    //                break;
    //            case DirectionRight:
    //                icon = &I_projectile_right;
    //                break;
    //            case DirectionLeft:
    //                icon = &I_projectile_left;
    //                break;
    //            default:
    //                icon = &I_tank_explosion;
    //            }
    //
    //            canvas_draw_icon(
    //                canvas,
    //                projectile->coordinates.x * CELL_LENGTH_PIXELS,
    //                projectile->coordinates.y * CELL_LENGTH_PIXELS - 1,
    //                icon);
    //        }
    //    }

    // Info
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontSecondary);
    char buffer1[13];
    snprintf(buffer1, sizeof(buffer1), "live: %u", tanks_state->enemies_live);
    canvas_draw_str_aligned(canvas, 127, 8, AlignRight, AlignBottom, buffer1);

    snprintf(buffer1, sizeof(buffer1), "left: %u", tanks_state->enemies_left);
    canvas_draw_str_aligned(canvas, 127, 18, AlignRight, AlignBottom, buffer1);

    snprintf(buffer1, sizeof(buffer1), "p1 l: %u", tanks_state->p1->lives);
    canvas_draw_str_aligned(canvas, 127, 28, AlignRight, AlignBottom, buffer1);

    snprintf(buffer1, sizeof(buffer1), "p1 s: %u", tanks_state->p1->score);
    canvas_draw_str_aligned(canvas, 127, 38, AlignRight, AlignBottom, buffer1);

    if(tanks_state->state == GameStateCooperativeServer && tanks_state->p2) {
        snprintf(buffer1, sizeof(buffer1), "rec: %u", tanks_state->received);
        canvas_draw_str_aligned(canvas, 127, 48, AlignRight, AlignBottom, buffer1);

        snprintf(buffer1, sizeof(buffer1), "snt: %u", tanks_state->sent);
        canvas_draw_str_aligned(canvas, 127, 58, AlignRight, AlignBottom, buffer1);
        //        snprintf(buffer1, sizeof(buffer1), "p2 l: %u", tanks_state->p2->lives);
        //        canvas_draw_str_aligned(canvas, 127, 48, AlignRight, AlignBottom, buffer1);
        //
        //        snprintf(buffer1, sizeof(buffer1), "p2 s: %u", tanks_state->p2->score);
        //        canvas_draw_str_aligned(canvas, 127, 58, AlignRight, AlignBottom, buffer1);
    }

    if(tanks_state->state == GameStateCooperativeClient) {
        snprintf(buffer1, sizeof(buffer1), "rec: %u", tanks_state->received);
        canvas_draw_str_aligned(canvas, 127, 48, AlignRight, AlignBottom, buffer1);
    }

    // Game Over banner
    if(tanks_state->state == GameStateGameOver) {
        canvas_set_color(canvas, ColorWhite);
        canvas_draw_box(canvas, 34, 20, 62, 24);

        canvas_set_color(canvas, ColorBlack);
        canvas_draw_frame(canvas, 34, 20, 62, 24);
        canvas_set_font(canvas, FontPrimary);

        if(tanks_state->enemies_left == 0 && tanks_state->enemies_live == 0) {
            canvas_draw_str(canvas, 37, 31, "You win!");
        } else {
            canvas_draw_str(canvas, 37, 31, "Game Over");
        }

        canvas_set_font(canvas, FontSecondary);
        char buffer[13];
        snprintf(buffer, sizeof(buffer), "Score: %u", tanks_state->p1->score);
        canvas_draw_str_aligned(canvas, 64, 41, AlignCenter, AlignBottom, buffer);
    }

    // TEST start
    unsigned char* data = tanks_game_serialize(tanks_state);
    tanks_game_deserialize_and_render(data, canvas);
    // TEST enf

    release_mutex((ValueMutex*)ctx, tanks_state);
}

static void tanks_game_input_callback(InputEvent* input_event, FuriMessageQueue* event_queue) {
    furi_assert(event_queue);

    TanksEvent event = {.type = EventTypeKey, .input = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

static void tanks_game_update_timer_callback(FuriMessageQueue* event_queue) {
    furi_assert(event_queue);

    TanksEvent event = {.type = EventTypeTick};
    furi_message_queue_put(event_queue, &event, 0);
}

static bool tanks_get_cell_is_free(TanksState* const tanks_state, Point point) {
    // Tiles
    if(tanks_state->thisMap[point.x][point.y] != ' ') {
        return false;
    }

    // Projectiles
    for(int8_t x = 0; x < 100; x++) {
        if(tanks_state->projectiles[x] != NULL) {
            if(tanks_state->projectiles[x]->coordinates.x == point.x &&
               tanks_state->projectiles[x]->coordinates.y == point.y) {
                return false;
            }
        }
    }

    // Player 1
    if(tanks_state->p1 != NULL) {
        if(tanks_state->p1->coordinates.x == point.x &&
           tanks_state->p1->coordinates.y == point.y) {
            return false;
        }
    }

    // Player 2
    if(tanks_state->p2 != NULL) {
        if(tanks_state->p2->coordinates.x == point.x &&
           tanks_state->p2->coordinates.y == point.y) {
            return false;
        }
    }

    // Bots
    for(int8_t x = 0; x < 6; x++) {
        if(tanks_state->bots[x] != NULL) {
            if(tanks_state->bots[x]->coordinates.x == point.x &&
               tanks_state->bots[x]->coordinates.y == point.y) {
                return false;
            }
        }
    }

    return true;
}

static uint8_t tanks_get_random_free_respawn_point_index(
    TanksState* const tanks_state,
    Point respawn_points[3]) {
    uint8_t first = rand() % 3;
    int8_t add = rand() % 2 ? +1 : -1;
    int8_t second = first + add;
    uint8_t third;

    if(second == 4) {
        second = 0;
    } else if(second == -1) {
        second = 3;
    }

    for(uint8_t i = 0; i < 3; i++) {
        if(i != first && i != second) {
            third = i;
        }
    }

    if(tanks_get_cell_is_free(tanks_state, respawn_points[first])) {
        return first;
    }

    if(tanks_get_cell_is_free(tanks_state, respawn_points[second])) {
        return second;
    }

    if(tanks_get_cell_is_free(tanks_state, respawn_points[third])) {
        return third;
    }

    return -1;
}

static void tanks_game_init_game(TanksState* const tanks_state, GameState type) {
    srand(DWT->CYCCNT);

    tanks_state->state = type;

    for(int8_t x = 0; x < 100; x++) {
        if(tanks_state->projectiles[x] != NULL) {
            free(tanks_state->projectiles[x]);
            tanks_state->projectiles[x] = NULL;
        }
    }

    int8_t team_one_respawn_points_counter = 0;
    int8_t team_two_respawn_points_counter = 0;

    for(int8_t x = 0; x < FIELD_WIDTH; x++) {
        for(int8_t y = 0; y < FIELD_HEIGHT; y++) {
            tanks_state->thisMap[x][y] = ' ';

            if(thisMap[y][x] == '1') {
                Point respawn = {x, y};
                tanks_state->team_one_respawn_points[team_one_respawn_points_counter++] = respawn;
            }

            if(thisMap[y][x] == '2') {
                Point respawn = {x, y};
                tanks_state->team_two_respawn_points[team_two_respawn_points_counter++] = respawn;
            }

            if(thisMap[y][x] == '-') {
                tanks_state->thisMap[x][y] = '-';
            }

            if(thisMap[y][x] == '=') {
                tanks_state->thisMap[x][y] = '=';
            }

            if(thisMap[y][x] == '*') {
                tanks_state->thisMap[x][y] = '*';
            }

            if(thisMap[y][x] == 'a') {
                tanks_state->thisMap[x][y] = 'a';
            }
        }
    }

    uint8_t index1 = tanks_get_random_free_respawn_point_index(
        tanks_state, tanks_state->team_one_respawn_points);
    Point c = {
        tanks_state->team_one_respawn_points[index1].x,
        tanks_state->team_one_respawn_points[index1].y};

    PlayerState p1 = {
        c,
        0,
        4,
        DirectionRight,
        0,
        0,
        1,
        SHOT_COOLDOWN,
        PLAYER_RESPAWN_COOLDOWN,
    };

    PlayerState* p1_state = malloc(sizeof(PlayerState));
    *p1_state = p1;

    tanks_state->p1 = p1_state;

    if(type == GameStateCooperativeServer) {
        int8_t index2 = tanks_get_random_free_respawn_point_index(
            tanks_state, tanks_state->team_one_respawn_points);
        Point c = {
            tanks_state->team_one_respawn_points[index2].x,
            tanks_state->team_one_respawn_points[index2].y};

        PlayerState p2 = {
            c,
            0,
            4,
            DirectionRight,
            0,
            0,
            1,
            SHOT_COOLDOWN,
            PLAYER_RESPAWN_COOLDOWN,
        };

        PlayerState* p2_state = malloc(sizeof(PlayerState));
        *p2_state = p2;

        tanks_state->p2 = p2_state;
    }

    tanks_state->enemies_left = 5;
    tanks_state->enemies_live = 0;
    tanks_state->enemies_respawn_cooldown = RESPAWN_COOLDOWN;
    tanks_state->received = 0;
    tanks_state->sent = 0;

    if(type == GameStateCooperativeClient) {
        for(int8_t x = 0; x < FIELD_WIDTH; x++) {
            for(int8_t y = 0; y < FIELD_HEIGHT; y++) {
                tanks_state->thisMap[x][y] = CellEmpty;
            }
        }
    }
}

static bool
    tanks_game_collision(Point const next_step, bool shoot, TanksState const* const tanks_state) {
    if(next_step.x < 0 || next_step.y < 0) {
        return true;
    }

    if(next_step.x >= FIELD_WIDTH || next_step.y >= FIELD_HEIGHT) {
        return true;
    }

    char tile = tanks_state->thisMap[next_step.x][next_step.y];

    if(tile == '*' && !shoot) {
        return true;
    }

    if(tile == '-' || tile == '=' || tile == 'a') {
        return true;
    }

    for(uint8_t i = 0; i < 6; i++) {
        if(tanks_state->bots[i] != NULL) {
            if(tanks_state->bots[i]->coordinates.x == next_step.x &&
               tanks_state->bots[i]->coordinates.y == next_step.y) {
                return true;
            }
        }
    }

    if(tanks_state->p1 != NULL && tanks_state->p1->live &&
       tanks_state->p1->coordinates.x == next_step.x &&
       tanks_state->p1->coordinates.y == next_step.y) {
        return true;
    }

    if(tanks_state->p2 != NULL && tanks_state->p2->live &&
       tanks_state->p2->coordinates.x == next_step.x &&
       tanks_state->p2->coordinates.y == next_step.y) {
        return true;
    }

    return false;
}

static Point tanks_game_get_next_step(Point coordinates, Direction direction) {
    Point next_step = {coordinates.x, coordinates.y};

    switch(direction) {
    // +-----x
    // |
    // |
    // y
    case DirectionUp:
        next_step.y--;
        break;
    case DirectionRight:
        next_step.x++;
        break;
    case DirectionDown:
        next_step.y++;
        break;
    case DirectionLeft:
        next_step.x--;
        break;
    }
    return next_step;
}

static uint8_t tanks_game_get_free_projectile_index(TanksState* const tanks_state) {
    uint8_t freeProjectileIndex;
    for(freeProjectileIndex = 0; freeProjectileIndex < 100; freeProjectileIndex++) {
        if(tanks_state->projectiles[freeProjectileIndex] == NULL) {
            return freeProjectileIndex;
        }
    }

    return 0;
}

static void tanks_game_shoot(
    TanksState* const tanks_state,
    PlayerState* tank_state,
    bool is_p1,
    bool is_p2) {
    tank_state->cooldown = SHOT_COOLDOWN;

    uint8_t freeProjectileIndex = tanks_game_get_free_projectile_index(tanks_state);

    ProjectileState* projectile_state = malloc(sizeof(ProjectileState));
    Point next_step = tanks_game_get_next_step(tank_state->coordinates, tank_state->direction);

    projectile_state->direction = tank_state->direction;
    projectile_state->coordinates = next_step;
    projectile_state->is_p1 = is_p1;
    projectile_state->is_p2 = is_p2;

    bool crush = tanks_game_collision(projectile_state->coordinates, true, tanks_state);
    projectile_state->explosion = crush;

    tanks_state->projectiles[freeProjectileIndex] = projectile_state;
}

static void tanks_game_process_game_step(TanksState* const tanks_state) {
    if(tanks_state->state == GameStateMenu) {
        return;
    }

    if(tanks_state->enemies_left == 0 && tanks_state->enemies_live == 0) {
        tanks_state->state = GameStateGameOver;
    }

    if(!tanks_state->p1->live && tanks_state->p1->lives == 0) {
        tanks_state->state = GameStateGameOver;
    }

    if(tanks_state->state == GameStateGameOver) {
        return;
    }

    if(tanks_state->p1 != NULL) {
        if(!tanks_state->p1->live && tanks_state->p1->respawn_cooldown > 0) {
            tanks_state->p1->respawn_cooldown--;
        }
    }

    // Player 1 spawn
    if(tanks_state->p1 && !tanks_state->p1->live && tanks_state->p1->lives > 0) {
        int8_t index = tanks_get_random_free_respawn_point_index(
            tanks_state, tanks_state->team_one_respawn_points);

        if(index != -1) {
            Point point = tanks_state->team_one_respawn_points[index];
            Point c = {point.x, point.y};
            tanks_state->p1->coordinates = c;
            tanks_state->p1->live = true;
            tanks_state->p1->direction = DirectionRight;
            tanks_state->p1->cooldown = SHOT_COOLDOWN;
            tanks_state->p1->respawn_cooldown = SHOT_COOLDOWN;
        }
    }

    // Player 2 spawn
    if(tanks_state->state == GameStateCooperativeServer && tanks_state->p2 &&
       !tanks_state->p2->live && tanks_state->p2->lives > 0) {
        int8_t index = tanks_get_random_free_respawn_point_index(
            tanks_state, tanks_state->team_one_respawn_points);

        if(index != -1) {
            Point point = tanks_state->team_one_respawn_points[index];
            Point c = {point.x, point.y};
            tanks_state->p2->coordinates = c;
            tanks_state->p2->live = true;
            tanks_state->p2->direction = DirectionRight;
            tanks_state->p2->cooldown = SHOT_COOLDOWN;
            tanks_state->p2->respawn_cooldown = SHOT_COOLDOWN;
        }
    }

    // Bot turn
    for(uint8_t i = 0; i < 6; i++) {
        if(tanks_state->bots[i] != NULL) {
            PlayerState* bot = tanks_state->bots[i];
            if(bot->cooldown) {
                bot->cooldown--;
            }

            // Rotate
            if(rand() % 3 == 0) {
                bot->direction = (rand() % 4);
            }

            // Move
            if(rand() % 2 == 0) {
                Point next_step = tanks_game_get_next_step(bot->coordinates, bot->direction);
                bool crush = tanks_game_collision(next_step, false, tanks_state);

                if(!crush) {
                    bot->coordinates = next_step;
                }
            }

            // Shoot
            if(bot->cooldown == 0 && rand() % 3 != 0) {
                tanks_game_shoot(tanks_state, bot, false, false);
            }
        }
    }

    // Bot spawn
    if(tanks_state->enemies_respawn_cooldown) {
        tanks_state->enemies_respawn_cooldown--;
    }

    if(tanks_state->enemies_left > 0 && tanks_state->enemies_live <= 4 &&
       tanks_state->enemies_respawn_cooldown == 0) {
        int8_t index = tanks_get_random_free_respawn_point_index(
            tanks_state, tanks_state->team_two_respawn_points);

        if(index != -1) {
            tanks_state->enemies_left--;
            tanks_state->enemies_live++;
            tanks_state->enemies_respawn_cooldown = RESPAWN_COOLDOWN;
            Point point = tanks_state->team_two_respawn_points[index];

            Point c = {point.x, point.y};

            PlayerState bot = {
                c,
                0,
                0,
                DirectionLeft,
                0,
                0,
                1,
                SHOT_COOLDOWN,
                PLAYER_RESPAWN_COOLDOWN,
            };

            uint8_t freeEnemyIndex;
            for(freeEnemyIndex = 0; freeEnemyIndex < 6; freeEnemyIndex++) {
                if(tanks_state->bots[freeEnemyIndex] == NULL) {
                    break;
                }
            }

            PlayerState* bot_state = malloc(sizeof(PlayerState));
            *bot_state = bot;

            tanks_state->bots[freeEnemyIndex] = bot_state;
        }
    }

    if(tanks_state->p1 != NULL && tanks_state->p1->live && tanks_state->p1->moving) {
        Point next_step =
            tanks_game_get_next_step(tanks_state->p1->coordinates, tanks_state->p1->direction);
        bool crush = tanks_game_collision(next_step, false, tanks_state);

        if(!crush) {
            tanks_state->p1->coordinates = next_step;
        }
    }

    // Player 2 spawn
    if(tanks_state->state == GameStateCooperativeServer && tanks_state->p2 &&
       tanks_state->p2->live && tanks_state->p2->moving) {
        Point next_step =
            tanks_game_get_next_step(tanks_state->p2->coordinates, tanks_state->p2->direction);
        bool crush = tanks_game_collision(next_step, false, tanks_state);

        if(!crush) {
            tanks_state->p2->coordinates = next_step;
        }
    }

    for(int8_t x = 0; x < 100; x++) {
        if(tanks_state->projectiles[x] != NULL) {
            ProjectileState* projectile = tanks_state->projectiles[x];
            Point c = projectile->coordinates;

            if(projectile->explosion) {
                // Break a wall
                if(tanks_state->thisMap[c.x][c.y] == '-') {
                    tanks_state->thisMap[c.x][c.y] = ' ';
                }

                // Kill a bot
                for(uint8_t i = 0; i < 6; i++) {
                    if(tanks_state->bots[i] != NULL) {
                        if(tanks_state->bots[i]->coordinates.x == c.x &&
                           tanks_state->bots[i]->coordinates.y == c.y) {
                            if(projectile->is_p1) {
                                tanks_state->p1->score++;
                            }

                            if(projectile->is_p2) {
                                tanks_state->p2->score++;
                            }

                            // No friendly fire
                            if(projectile->is_p1 || projectile->is_p2) {
                                tanks_state->enemies_live--;
                                free(tanks_state->bots[i]);
                                tanks_state->bots[i] = NULL;
                            }
                        }
                    }
                }

                // Destroy the flag
                if(tanks_state->thisMap[c.x][c.y] == 'a') {
                    tanks_state->state = GameStateGameOver;
                    return;
                }

                // Kill a player 1
                if(tanks_state->p1 != NULL) {
                    if(tanks_state->p1->live && tanks_state->p1->coordinates.x == c.x &&
                       tanks_state->p1->coordinates.y == c.y) {
                        tanks_state->p1->live = false;
                        tanks_state->p1->lives--;
                        tanks_state->p1->respawn_cooldown = PLAYER_RESPAWN_COOLDOWN;
                    }
                }

                // Kill a player 2
                if(tanks_state->p2 != NULL) {
                    if(tanks_state->p2->live && tanks_state->p2->coordinates.x == c.x &&
                       tanks_state->p2->coordinates.y == c.y) {
                        tanks_state->p2->live = false;
                        tanks_state->p2->lives--;
                        tanks_state->p2->respawn_cooldown = PLAYER_RESPAWN_COOLDOWN;
                    }
                }

                // Delete projectile
                free(tanks_state->projectiles[x]);
                tanks_state->projectiles[x] = NULL;
                continue;
            }

            Point next_step =
                tanks_game_get_next_step(projectile->coordinates, projectile->direction);
            bool crush = tanks_game_collision(next_step, true, tanks_state);
            projectile->coordinates = next_step;

            if(crush) {
                projectile->explosion = true;
            }
        }
    }

    if(tanks_state->p1->cooldown > 0) {
        tanks_state->p1->cooldown--;
    }

    if(tanks_state->p2 != NULL && tanks_state->p2->cooldown > 0) {
        tanks_state->p2->cooldown--;
    }

    if(tanks_state->p1 != NULL && tanks_state->p1->live && tanks_state->p1->shooting &&
       tanks_state->p1->cooldown == 0) {
        tanks_game_shoot(tanks_state, tanks_state->p1, true, false);
    }

    tanks_state->p1->moving = false;
    tanks_state->p1->shooting = false;

    if(tanks_state->p2 != NULL) {
        tanks_state->p2->moving = false;
        tanks_state->p2->shooting = false;
    }
}

int32_t tanks_game_app(void* p) {
    srand(DWT->CYCCNT);

    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(TanksEvent));

    TanksState* tanks_state = malloc(sizeof(TanksState));

    tanks_state->state = GameStateMenu;
    tanks_state->menu_state = MenuStateSingleMode;

    ValueMutex state_mutex;
    if(!init_mutex(&state_mutex, tanks_state, sizeof(TanksState))) {
        // furi_log_print(FURI_LOG_ERROR, "cannot create mutex\r\n");
        free(tanks_state);
        return 255;
    }

    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, tanks_game_render_callback, &state_mutex);
    view_port_input_callback_set(view_port, tanks_game_input_callback, event_queue);

    FuriTimer* timer =
        furi_timer_alloc(tanks_game_update_timer_callback, FuriTimerTypePeriodic, event_queue, timer);
    furi_timer_start(timer, furi_kernel_get_tick_frequency() / 4);

    // Open GUI and register view_port
    Gui* gui = furi_record_open("gui");
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    TanksEvent event;

    // Initialize network thing.
    uint32_t frequency = 433920000;
    size_t message_max_len = 180;
    uint8_t incomingMessage[180] = {0};
    SubGhzTxRxWorker* subghz_txrx = subghz_tx_rx_worker_alloc();
    subghz_tx_rx_worker_start(subghz_txrx, frequency);
    furi_hal_power_suppress_charge_enter();

    for(bool processing = true; processing;) {
        FuriStatus event_status = furi_message_queue_get(event_queue, &event, 100);

        TanksState* tanks_state = (TanksState*)acquire_mutex_block(&state_mutex);

        if(event_status == FuriStatusOk) {
            // press events
            if(event.type == EventTypeKey) {
                if(event.input.type == InputTypePress) {
                    switch(event.input.key) {
                    case InputKeyUp:
                        if(tanks_state->state == GameStateMenu) {
                            if(tanks_state->menu_state == MenuStateCooperativeServerMode) {
                                tanks_state->menu_state = MenuStateSingleMode;
                            } else if(tanks_state->menu_state == MenuStateCooperativeClientMode) {
                                tanks_state->menu_state = MenuStateCooperativeServerMode;
                            }
                        } else if(tanks_state->state == GameStateCooperativeClient) {
                            string_t goesUp;
                            char arr[2];
                            arr[0] = GoesUp;
                            arr[1] = 0;
                            string_set(goesUp, (char*)&arr);

                            subghz_tx_rx_worker_write(
                                subghz_txrx,
                                (uint8_t*)string_get_cstr(goesUp),
                                strlen(string_get_cstr(goesUp)));

                        } else {
                            tanks_state->p1->moving = true;
                            tanks_state->p1->direction = DirectionUp;
                        }
                        break;
                    case InputKeyDown:
                        if(tanks_state->state == GameStateMenu) {
                            if(tanks_state->menu_state == MenuStateSingleMode) {
                                tanks_state->menu_state = MenuStateCooperativeServerMode;
                            } else if(tanks_state->menu_state == MenuStateCooperativeServerMode) {
                                tanks_state->menu_state = MenuStateCooperativeClientMode;
                            }
                        } else if(tanks_state->state == GameStateCooperativeClient) {
                            string_t goesDown;
                            char arr[2];
                            arr[0] = GoesDown;
                            arr[1] = 0;
                            string_set(goesDown, (char*)&arr);

                            subghz_tx_rx_worker_write(
                                subghz_txrx,
                                (uint8_t*)string_get_cstr(goesDown),
                                strlen(string_get_cstr(goesDown)));
                        } else {
                            tanks_state->p1->moving = true;
                            tanks_state->p1->direction = DirectionDown;
                        }
                        break;
                    case InputKeyRight:
                        if(tanks_state->state == GameStateCooperativeClient) {
                            string_t goesRight;
                            char arr[2];
                            arr[0] = GoesRight;
                            arr[1] = 0;
                            string_set(goesRight, (char*)&arr);

                            subghz_tx_rx_worker_write(
                                subghz_txrx,
                                (uint8_t*)string_get_cstr(goesRight),
                                strlen(string_get_cstr(goesRight)));
                        } else {
                            tanks_state->p1->moving = true;
                            tanks_state->p1->direction = DirectionRight;
                        }
                        break;
                    case InputKeyLeft:
                        if(tanks_state->state == GameStateCooperativeClient) {
                            string_t goesLeft;
                            char arr[2];
                            arr[0] = GoesLeft;
                            arr[1] = 0;
                            string_set(goesLeft, (char*)&arr);

                            subghz_tx_rx_worker_write(
                                subghz_txrx,
                                (uint8_t*)string_get_cstr(goesLeft),
                                strlen(string_get_cstr(goesLeft)));
                        } else {
                            tanks_state->p1->moving = true;
                            tanks_state->p1->direction = DirectionLeft;
                        }
                        break;
                    case InputKeyOk:
                        if(tanks_state->state == GameStateMenu) {
                            if(tanks_state->menu_state == MenuStateSingleMode) {
                                tanks_state->server = true;
                                tanks_game_init_game(tanks_state, GameStateSingle);
                                break;
                            } else if(tanks_state->menu_state == MenuStateCooperativeServerMode) {
                                tanks_state->server = true;
                                tanks_game_init_game(tanks_state, GameStateCooperativeServer);
                                break;
                            } else if(tanks_state->menu_state == MenuStateCooperativeClientMode) {
                                tanks_state->server = false;
                                tanks_game_init_game(tanks_state, GameStateCooperativeClient);
                                break;
                            }
                        } else if(tanks_state->state == GameStateGameOver) {
                            tanks_game_init_game(tanks_state, tanks_state->state);
                        } else if(tanks_state->state == GameStateCooperativeClient) {
                            string_t shoots;
                            char arr[2];
                            arr[0] = Shoots;
                            arr[1] = 0;
                            string_set(shoots, (char*)&arr);

                            subghz_tx_rx_worker_write(
                                subghz_txrx,
                                (uint8_t*)string_get_cstr(shoots),
                                strlen(string_get_cstr(shoots)));
                        } else {
                            tanks_state->p1->shooting = true;
                        }
                        break;
                    case InputKeyBack:
                        processing = false;
                        break;
                    }
                }
            } else if(event.type == EventTypeTick) {
                if(tanks_state->state == GameStateCooperativeServer) {
                    if(subghz_tx_rx_worker_available(subghz_txrx)) {
                        memset(incomingMessage, 0x00, message_max_len);
                        subghz_tx_rx_worker_read(subghz_txrx, incomingMessage, message_max_len);

                        if(incomingMessage != NULL) {
                            tanks_state->received++;

                            switch(incomingMessage[0]) {
                            case GoesUp:
                                tanks_state->p2->moving = true;
                                tanks_state->p2->direction = DirectionUp;
                                break;
                            case GoesRight:
                                tanks_state->p2->moving = true;
                                tanks_state->p2->direction = DirectionRight;
                                break;
                            case GoesDown:
                                tanks_state->p2->moving = true;
                                tanks_state->p2->direction = DirectionDown;
                                break;
                            case GoesLeft:
                                tanks_state->p2->moving = true;
                                tanks_state->p2->direction = DirectionLeft;
                                break;
                            case Shoots:
                                tanks_state->p2->shooting = true;
                                break;
                            default:
                                break;
                            }
                        }
                    }

                    tanks_game_process_game_step(tanks_state);

                    string_t serializedData;
                    unsigned char* data = tanks_game_serialize(tanks_state);
                    char arr[11 * 16 + 1];

                    for(uint8_t i = 0; i < 11 * 16; i++) {
                        arr[i] = data[i];
                    }

                    arr[11 * 16] = 0;

                    string_set(serializedData, (char*)&arr);

                    subghz_tx_rx_worker_write(
                        subghz_txrx,
                        (uint8_t*)string_get_cstr(serializedData),
                        strlen(string_get_cstr(serializedData)));

                    tanks_state->sent++;
                } else if(tanks_state->state == GameStateSingle) {
                    tanks_game_process_game_step(tanks_state);
                } else if(tanks_state->state == GameStateCooperativeClient) {
                    if(subghz_tx_rx_worker_available(subghz_txrx)) {
                        memset(incomingMessage, 0x00, message_max_len);
                        subghz_tx_rx_worker_read(subghz_txrx, incomingMessage, message_max_len);

                        tanks_state->received++;

                        tanks_game_deserialize_and_write_to_state(
                            (unsigned char*)incomingMessage, tanks_state);
                    }
                }
            }
        } else {
            // event timeout
        }

        view_port_update(view_port);
        release_mutex(&state_mutex, tanks_state);
        furi_delay_ms(1);
    }

    furi_delay_ms(10);
    furi_hal_power_suppress_charge_exit();

    if(subghz_tx_rx_worker_is_running(subghz_txrx)) {
        subghz_tx_rx_worker_stop(subghz_txrx);
        subghz_tx_rx_worker_free(subghz_txrx);
    }

    furi_timer_free(timer);
    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    furi_record_close("gui");
    view_port_free(view_port);
    furi_message_queue_free(event_queue);
    delete_mutex(&state_mutex);

    if(tanks_state->p1 != NULL) {
        free(tanks_state->p1);
    }

    if(tanks_state->p2 != NULL) {
        free(tanks_state->p2);
    }

    free(tanks_state);

    return 0;
}
