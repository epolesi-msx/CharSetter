/*-----------------------------------------------------------------------------
  CharSetter v1.0
  Copyright (c)2020 by Emerson Polesi
  
  A MSX Charset Changer.
  
  This program changes the original character set of a MSX manchine. It is 
  meant to run on ROM, in a cartridge.

  It can be useful for those who wants to run MSX software designed for
  different languages of the native machine language, so that all characters 
  and symbols can be displayed correctly on the software original language..
  
  This program acts only on the charset font graphics. It does not change
  keyboard mapping or configuration.
  
  The charset to be used is defined in compile time, by compiler directives,
  as follows:
  
    CS_ARABIAN     =  Arabian
    CS_BRAZIL_G    =  Brazilian BRASCII (Gradiente)
    CS_BRAZIL_S    =  Brazilian BRASCII (Sharp)
    CS_BRAZIL_G10  =  Brazilian Gradiente v1.0 (superseded by BRASCII)
	CS_BRAZIL_S11  =  Brazilian Sharp 1.1 (superseded by BRASCII)
	CS_GERMAN      =  German
	CS_INTERNAT    =  International
	CS_JAPANESE    =  Japanese
	CS_KOREAN      =  Korean
	CS_RUSSIAN     =  Russian
	
	MULTI_CS       =  Any of the above, via menu selection
	
  The use of a single charset directive will produce a file meant to 16kb ROM.
  The use of the MULTI_CS (multi-charset) directive, will produce a file meant
  for 32kb ROM.
  
  The generated file doesn't contain 16-byte ROM header, so, it must be 
  unserted manually, after compilation.
  
  To disable the program during the machine boot, keep the cursor UP key 
  pressed for a few moments.
  
  There are future plans for some improvements, like BASIC extensiond and 
  MSX-DOS utilities to allow easy reconfiguration of the charset after the
  boot.

  ============================================================================
  Licensing:

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>
-----------------------------------------------------------------------------*/  

#include <string.h>

#include "MSX.h"
#include "NewDataFont.h"
#include "Charsets.h"

#define NORMAL		0	// normal text
#define INVERT		1	// inverted text

#define FALSE		0
#define TRUE		1

#define FG_COLOR	12	// foreground color
#define BG_COLOR	1	// background color

#define ABORT		255	// abort indication

typedef unsigned char BYTE;
typedef unsigned int  WORD;

BYTE runCount();
BYTE inkey();
BYTE input();
void delayVDP(WORD);
void prepareStage();
void blinkFinalText(const BYTE *);
void setFont(const BYTE *);

#ifndef MULTI_CS

const BYTE * const charsetName[] = { CS_NAME };
const BYTE * const charsetFont[] = { CS_FONT };

#else
	
BYTE showMenu(int, int);
void clearMenuArea();

const BYTE * const charsetName[] = {
	CS_NAME_ARABIAN, 	CS_NAME_BRAZIL_G, 	CS_NAME_BRAZIL_S,
	CS_NAME_BRAZIL_G10,	CS_NAME_BRAZIL_S11,	CS_NAME_GERMAN,
	CS_NAME_INTERNAT, 	CS_NAME_JAPANESE,	CS_NAME_KOREAN,
	CS_NAME_RUSSIAN
};

const BYTE * const charsetFont[] = {
	CS_FONT_ARABIAN,	CS_FONT_BRAZIL_G,	CS_FONT_BRAZIL_S,
	CS_FONT_BRAZIL_G10,	CS_FONT_BRAZIL_S11,	CS_FONT_GERMAN,
	CS_FONT_INTERNAT,	CS_FONT_JAPANESE,	CS_FONT_KOREAN,
	CS_FONT_RUSSIAN
};

#endif

/*-----------------------------------------------------------------------------
  First code to run. The start.
-----------------------------------------------------------------------------*/  
void main(void) __naked
{
	// save slot ID and current screen colors for later use
__asm
	ld hl,#FORCLR
	ld a,(hl)
	push af				; foreground color
	inc hl
	ld a,(hl)
	push af				; background color
	inc hl
	ld a,(hl)
	push af				; border color
	push bc				; slot ID of the current page (register C)
__endasm;

	// for MULTI_CS (32kb ROM), activate page 2 on the same slot as page 1
#ifdef MULTI_CS
__asm
	ld a,c
	ld hl,#0x8000
	call ENASLT			; set page 2 to same slot as page 1 (the current page)
__endasm;
#endif

    // ensure to run just once (avoid page mirroring issues)
	if (runCount() > 0) goto skipRun;
	// test for skip key (cursor up)
	for (int ct=0; ct<3; ct++) {
		if (inkey()) if (input() == KEY_UP) goto skipRun;
		delayVDP(1);  // give time for key checking on fast machines
	}
	BYTE op = 0;
	prepareStage();  // prepare stage screen

#ifdef MULTI_CS
	// show selection menu/submenu
	do {
		op = showMenu(0, op);  // show main menu
		clearMenuArea();  // clear menu
		if (op == 1) {  // brazilian, show submenu
			op = showMenu(1, 0);  // show sub menu (brazilian)
			clearMenuArea();  // clear menu
			if (op != ABORT) break;
			op = 1;
		}
	} while (op == 1);
	if (op == ABORT) goto endRun;  // user abort
#endif

	// blink selected option and activate the respective font
	blinkFinalText(charsetName[op]);
	delayVDP(10);  // 1s delay
	setFont(charsetFont[op]);

endRun:
	
	// restore original screen colors and reset screen before leaving
__asm
	pop bc				; slot ID
	pop af				; original border color
	ld hl,#BDRCLR
	ld (hl),a
	pop af				; original background color
	dec hl
	ld (hl),a
	pop af				; original foreground color
	dec hl
	ld (hl),a
	call _setScreenMode	; reset screen
	ret
__endasm;

skipRun:

	// run skipped, just clean stack and leave
__asm
	pop af				; slot ID
	pop af				; original border color
	pop af				; original background color
	pop af				; original foreground color
	ret
__endasm;

}  // main()

/*-----------------------------------------------------------------------------
  Set program's screen mode (screen 1, width 32).
-----------------------------------------------------------------------------*/  
void setScreenMode() __naked
{
__asm
	di
	ld a,#32			; widht 32
	ld (#LINL32),a
	ld (#LINLEN),a
	ld a,#1				; screen 1
	call CHGMOD
	ld hl,#CSRSW
	ld (hl),#0			; disable cursor
	ei
	ret;
__endasm;
}  // setScreenMode()

/*-----------------------------------------------------------------------------
  Set screen colors.
  Border color will be the same as background's.
  
  Parameters:
	fg 		(in) - foreground color.
	bg 		(in) - background color.
-----------------------------------------------------------------------------*/  
void setColors(BYTE fg, BYTE bg) __naked
{
	fg; bg;
__asm
	di
	ld iy,#2
	add iy,sp
	ld a,(iy)
	ld hl,#FORCLR		
	ld (hl),a
	ld a,1(iy)
	ld hl,#BAKCLR
	ld (hl),a
	ld hl,#BDRCLR
	ld (hl),a
	call CHGCLR
	ei
	ret
__endasm;
}  // setColors()

/*-----------------------------------------------------------------------------
  Set cursor position on the screen.
  
	x 		(in) - Column (1 to 32).
	y 		(in) - Line (1 to 24).
-----------------------------------------------------------------------------*/  
void locate(BYTE x, BYTE y) __naked
{
	x; y;
__asm
	di
	ld iy,#2
	add iy,sp
	ld h,(iy)			; x (column)
	ld l,1(iy)			; y (line)
	call #POSIT
	ei
	ret
__endasm;
}  // locate

/*-----------------------------------------------------------------------------
  Write a byte to the a specified VRAM address.
  
  Parameters:
	addr	(in) - VRAM address.
	b		(in) - The byte to be written.
-----------------------------------------------------------------------------*/  
void vPoke(WORD addr, BYTE b) __naked
{
	addr; b;
__asm
	ld iy,#2
	add iy,sp
	di
	ld a,0(iy)          ; VRAM addr low byte
	out (#VDP_P1),a
	ld a,1(iy)          ; VRAM addr high byte
	set 6,a             ; set writing 
	out (#VDP_P1),a
	ei
	ld a,2(iy)
	out (#VDP_P0),a		; put byte into VRAM
	ret
__endasm;
}  // vPoke()

/*-----------------------------------------------------------------------------
  Write a character directly to VRAM.
  The VRAM address will be calculated based on the current cursor position.
  The cursor position will be updated.
  
  Parameters:
	c 		(in) - The character to be writen on screen.
-----------------------------------------------------------------------------*/  
void putChar(BYTE c)
{
	BYTE *csrY = (BYTE *) CSRY;  // current cursor line
	BYTE *csrX = (BYTE *) CSRX;  // current cursor column
	WORD addr = 0x1800 + ((*csrY - 1) * 32) + (*csrX - 1);  // VRAM address
	vPoke(addr, c);  // write the character to VRAM
	// update the cursor position
	if (*csrX < 32) *csrX = *csrX + 1;  // increment column, if possible
	else {  // line wrapping...
		*csrX = 1;  // set column to 1
		*csrY = *csrY + 1;  // increase line
		if (*csrY > 24) *csrY = 1;  // end of screen, jump to 1st line
	}
}  // putChar()

/*-----------------------------------------------------------------------------
  Write a text to screen.
  
  Parameters:
	text	(in) - The text to be written (null-terminated).
	invert	(in) - NORMAL to use normal font. INVERTED to use inverted font.
-----------------------------------------------------------------------------*/  
void printStr(const char *text, BYTE invert)
{
	while (*text) putChar(*text++ | (invert ? 0x80 : 0));  // write text
}  // printStr()

/*-----------------------------------------------------------------------------
  Write a text to the screen, line centered.
  
  Parameters:
	text	(in) - The text to be written (null-terminated).
	line	(in) - The screen line.
	invert	(in) - NORMAL to use normal font. INVERTED to use inverted font.
-----------------------------------------------------------------------------*/  
void printStrCenter(const char *text, BYTE line, BYTE invert)
{
	locate(16 - (strlen(text) / 2), line);  // set cursor position
	while (*text) putChar(*text++ | (invert ? 0x80 : 0));  // write text
}  // printStrCenter()

/*-----------------------------------------------------------------------------
  Draw a box frame at the specified coordinates of the screen.
  
  Parameters:
	x1		(in) - Start column.
	y1		(in) - Star line.
	x2		(in) - End column.
	y2		(in) - End line.
-----------------------------------------------------------------------------*/  
void drawBox(BYTE x1, BYTE y1, BYTE x2, BYTE y2)
{
	locate(x1, y1);
	putChar(24);
	for (int x=x1+1; x<x2; x++) putChar(23);
	putChar(25);
	for (int y=y1+1; y<y2; y++) {
		locate(x1, y); 
		putChar(22);
		locate(x2, y);
		putChar(22);
	}
	locate(x1, y2);
	putChar(26);
	for (int x=x1+1; x<x2; x++) putChar(23);
	putChar(27);
}  // drawBox()

/*-----------------------------------------------------------------------------
  Copy a block of bytes to VRAM.
  
  Parameters:
	src 	(in) - a pointer to the soucre memory block.
	dst 	(in) - destination VRAM address.
	len 	(in) - the size of the block to be copied (in bytes).
	invert	(in) - If TRUE, bytes are copied as-is. If FALSE, bytes are
				   copied inverted to VRAM.
-----------------------------------------------------------------------------*/  
void copyToVRAM(BYTE *src, WORD dst, WORD len, BYTE invert) __naked
{
	src; dst; len; invert;
__asm
	push bc
	push de
	ld iy,#6
	add iy,sp
	di
	ld a,2(iy)          ; VRAM dst addr low byte
	out (#VDP_P1),a
	ld a,3(iy)          ; VRAM dst addr high byte
	set 6,a             ; set writing 
	out (#VDP_P1),a
	ei
	ld e,4(iy)			; DE = len
	ld d,5(iy)
	ld l,(iy)			; HL = src
	ld h,1(iy)
	ld c,6(iy)			; C  = invert
loop_ctv:
	ld a,c
	or a
	ld a,(hl)			; A = source byte
	jp z,cont_ctv		; if not invert, jump
	cpl					; invert byte
cont_ctv:
	out (#VDP_P0),a		; put byte into VRAM
	inc hl
	dec de
	ld a,d
	or e
	jp nz,loop_ctv
	pop bc
	pop de
	ret
__endasm;
} // copyToVRAM()

/*-----------------------------------------------------------------------------
  Prepare the main screen stage.
-----------------------------------------------------------------------------*/  
void prepareStage()
{
	setScreenMode();  // screen 1, width 32
	setColors(FG_COLOR, BG_COLOR);  // set screen colors
	// put the program's font in the pattern table area of VRAM
	copyToVRAM(new_data_font, 0, sizeof(new_data_font), NORMAL);
	copyToVRAM(new_data_font, 0x400, sizeof(new_data_font), INVERT);
	// screen elements
	drawBox(1, 1, 32, 3);
	locate(9, 2);
	printStr("CharSetter v1.0", NORMAL);
	drawBox(1, 22, 32, 24);
	vPoke(0x1AC7, 0);
	locate(9, 23);
	printStr("2020 by E.Polesi", NORMAL);
}  // prepareState()

/*-----------------------------------------------------------------------------
  Blink text in the middle of the stage.
  
  Parameters:
    str		(in) - Text to blink (null-terminated).
-----------------------------------------------------------------------------*/  
void blinkFinalText(const BYTE *str)
{
	for (int i=1; i<=9; i++) {
		printStrCenter(str, 12, i%2);
		delayVDP(1);
	}
}  // blinkFinalText()

/*-----------------------------------------------------------------------------
  Activate a charset font in the system BIOS area.
  
  Parameters:
	font	(in) - Pointer to the charset font bitmap.
-----------------------------------------------------------------------------*/  
void setFont(const BYTE *font) __naked
{
	font;
__asm
	ld iy,#2
	add iy,sp
	ld a,2(iy)			; slot ID stack (not parameter)
	ld (#CGPNT),a		; charset font slot ID
	ld l,(iy)
	ld h,1(iy)
	ld (#CGPNTA),hl		; charset font addr
	ret
__endasm;
}  // setFont()

/*-----------------------------------------------------------------------------
  Z80-based delay routine.
  On a Z80 running at ~3.58MHz, each "delay unit" will take about 1ms.
  
  Parameters:
	ct		(in) - The quantity of "delay units" to wait.
-----------------------------------------------------------------------------*/  
/*
void delayZ80(WORD ct) __naked
{
	ct;
__asm
	push bc
	push de
	ld iy,#6
	add iy,sp
	ld e,(iy)
	ld d,1(iy)
loop_d1:
	ld bc,#0x2F
loop_d2:
	ex (sp),hl
	ex (sp),hl
	dec bc
	ld a,c
	or b
	jr nz,loop_d2
	dec de
	ld a,e
	or d
	jr nz,loop_d1
	pop de
	pop bc
	ret
__endasm;
}  // delayZ80()
*/

/*-----------------------------------------------------------------------------
  VDP-based delay routine.
  It uses constant VDP interruptions to get some accurate timing.
  The routine takes in acoount the VDP frequency (50 or 60Hz), so, it should 
  perform equally on any MSX machine.
  Each "delay unit" will take about 100ms.
  
  Parameters:
	ct		(in) - The quantity of "delay units" to wait.
-----------------------------------------------------------------------------*/  
void delayVDP(WORD ct) __naked
{
	ct;
__asm
	push bc
	push de
	ld iy,#6
	add iy,sp
	ld e,(iy)
	ld d,1(iy)
loop_d21:
	ld c,#5
	ld a,(#0x2B)
	and #0x80
	jr nz,cont_d21
	inc c
cont_d21:
	di
	ld iy,#JIFFY
	xor a
	ld (iy),a
	ld 1(iy),a
	ei
loop_d22:
    halt
	ld a,(iy)
	cp c
	jp z,cont_d23
	jp po,cont_d22
	xor #0x80
cont_d22:
    jp m,loop_d22
cont_d23:
	dec de
	ld a,e
	or d
	jr nz,loop_d21
	pop de
	pop bc
	ret
__endasm;
} // delayVDP()

/*-----------------------------------------------------------------------------
  Get the current run counter and update (increment) it.
  
  Return: The run counter (before increment).
-----------------------------------------------------------------------------*/  
BYTE runCount() __naked
{
// counter addr = SLTWRK + main_slot*32 + sub_slot*8 + 2
__asm
	push de
	ld iy,#2
	add iy,sp
	ld a,2(iy)			; slot ID in stack (not parameter)
	and #3
	ld l,a
	xor a
	ld h,a
	add hl,hl
	add hl,hl
	add hl,hl
	add hl,hl
	add hl,hl
	ex de,hl
	ld a,2(iy)			; slot ID in stack (not parameter)
	and #12
	ld l,a
	xor a
	ld h,a
	add hl,hl
	add hl,de
	ld de,#SLTWRK
	add hl,de
	inc hl
	inc hl
	ld a,(hl)
	push af
	inc a
	ld (hl),a
	pop af
	ld l,a
	xor a
	ld h,a
	pop de
	ret
__endasm;
}  // runCount()

/*-----------------------------------------------------------------------------
  Check if a key was pressed.
  This routine doesn't wait for a key press, it returns immediately.
  
  Return: TRUE if a key was pressed or FALSE otherwise.
-----------------------------------------------------------------------------*/  
BYTE inkey() __naked
{
__asm
	call CHSNS
	ld a,#0
	ld h,a
	jr z,inkey_end
	inc a
inkey_end:
	ld l,a
	ret
__endasm;
}  // inkey()

/*-----------------------------------------------------------------------------
  Read a key from the keyboard.
  If a key was already pressed, it will return it immediately, otherwise, 
  it will wait for a key pressing.
  
  Return: The pressed key ASCII code.
-----------------------------------------------------------------------------*/  
BYTE input() __naked
{
__asm
	call CHGET
	ld l,a
	xor a
	ld h,a
	ret
__endasm;
}  // input()

#ifdef MULTI_CS

#define OP2ID(menu, op) (menu ? op+1 : (op<2 ? op : op+3))

/*-----------------------------------------------------------------------------
  Return the text of a menu/submenu option..
  
  Parameters:
	menu 	(in) - the menu number.
	opt 	(in) - the option number.
  
  Return: The option text.
-----------------------------------------------------------------------------*/  
const BYTE *option(int menu, int op)
{
	if (menu == 0  &&  op == 1) return ((const BYTE *) "Brazilian");
	return charsetName[OP2ID(menu, op)];
}

/*-----------------------------------------------------------------------------
  Show menu and wait for user selection.
  
  Return: The selected option or ABORT in case of abortion.
-----------------------------------------------------------------------------*/  
BYTE showMenu(int menu, int op)
{
	BYTE k=0, fi=1, ct=0;
	int numOpt = menu ? 4 : 7;
	int li = (10 - numOpt) / 2 + 9;
	
	// show menu
	printStrCenter("__ SELECT OPTION __", 7, NORMAL);
	for (int i=0; i<numOpt; i++) {
		printStrCenter(option(menu,i), li+i, i==op);
	}
	
	// select option
	do {
		if (!inkey()) {  // no key pressed...
			delayVDP(1);  // 0,1s delay to allow good key checking
			ct++;
			if (ct == 5) {  // 0,5s delay counter to allow blinking
				fi = 1-fi;  // change font inverted/normal state
				ct = 0;  // reset blinking time counter
				printStrCenter(option(menu,op), li+op, fi);
			}
		}
		else {  // key pressed...
			k = input();
			if (k == KEY_ESC) return ABORT;  // ESC, return ABORT
			if (k != KEY_UP  &&  k != KEY_DOWN) continue;  // invalid key
			printStrCenter(option(menu,op), li+op, NORMAL); // old option
			if (k == KEY_UP) op = (op>0) ? op-1 : numOpt-1;  // UP
			if (k == KEY_DOWN) op = (op<numOpt-1) ? op+1 : 0;  // DOWN
			printStrCenter(option(menu,op), li+op, INVERT);  // mew option
			ct = 0;  // reset blinking time counter
			fi = 1;  // set font to inverted
		}
	} while (k != KEY_ENTER);  // wait for option selection
	
	return OP2ID(menu, op);  // return selected option
}  // mainMenu()

/*-----------------------------------------------------------------------------
  Clean the menu area.
-----------------------------------------------------------------------------*/  
void clearMenuArea()
{
	locate(1, 4);
	for (int i=0; i<18; i++) {
		printStr("                                ", NORMAL);
	}
}  // clearMenuArea()

#endif