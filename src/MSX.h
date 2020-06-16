#ifndef __MSX_H__
#define __MSX_H__

// MSX KEYS (ASCII codes)
#define KEY_ENTER 13
#define KEY_ESC   27
#define KEY_UP    30
#define KEY_DOWN  31

//  MSX I/O Ports
#define VDP_P0 0x98
#define VDP_P1 0x99
#define PPISLT 0xA8

// MSX BIOS Subroutines
#define CALSLT 0x001C
#define ENASLT 0x0024
#define CHGMOD 0x005F
#define CHGCLR 0x0062
#define CHSNS  0x009C
#define CHGET  0x009F
#define POSIT  0x00C6

// MSX BIOS System Area Variables
#define LINL32 0xF3AF
#define LINLEN 0xF3B0
#define CSRY   0xF3DC
#define CSRX   0xF3DD
#define FORCLR 0xF3E9
#define BAKCLR 0xF3EA
#define BDRCLR 0xF3EB
#define CGPNT  0xF91F
#define CGPNTA 0xF920
#define DPPAGE 0xFAF5
#define ACPAGE 0xFAF6
#define EXBRSA 0xFAF8
#define HIMEM  0xFC4A
#define JIFFY  0xFC9E
#define EXPTBL 0xFCC1
#define CSRSW  0xFCA9
#define SLTWRK 0xFD09

#endif