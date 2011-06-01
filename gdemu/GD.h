/*
 * Copyright (C) 2011 by James Bowman <jamesb@excamera.com>
 * Gameduino library for arduino.
 *
 */

#ifndef _GD_H_INCLUDED
#define _GD_H_INCLUDED

#include "wiring.h"
#include "avr/pgmspace.h"

#ifdef XESSPROTO
#define SS_PIN 10
#else
#define SS_PIN 9
#endif

class GDClass {
public:
  static void begin();
  static void end();
  static void __start(uint16_t addr);
  static void __wstart(uint16_t addr);
  static void __end(void);
  static byte rd(uint16_t addr);
  static void wr(uint16_t addr, byte v);
  static uint16_t rd16(uint16_t addr);
  static void wr16(uint16_t addr, uint16_t v);
  static void fill(int16_t addr, byte v, uint16_t count);
  static void copy(uint16_t addr, prog_uchar *src, int16_t count);
  static void setpal(int16_t pal, uint16_t rgb);
  static void sprite(int16_t spr, int16_t x, int16_t y, byte image, byte palette, byte rot = 0, byte jk = 0);
  static void sprite2x2(int16_t spr, int16_t x, int16_t y, byte image, byte palette, byte rot = 0, byte jk = 0);
  static void waitvblank();
  static void microcode(prog_uchar *src, int16_t count);
  static void uncompress(uint16_t addr, prog_uchar *src);

  static void voice(int16_t v, byte wave, uint16_t freq, byte lamp, byte ramp);
  static void ascii();
  static void putstr(int16_t x, int16_t y, const char *s);

  static void screenshot(uint16_t frame);

  void __wstartspr(uint16_t spr = 0);
  void xsprite(int16_t ox, int16_t oy, char x, char y, byte image, byte palette, byte rot = 0, byte jk = 0);
  void xhide();

  byte spr;   // Current sprite, incremented by xsprite/xhide above
};

extern GDClass GD;

#define RGB(r,g,b) ((((r) >> 3) << 10) | (((g) >> 3) << 5) | ((b) >> 3))
#define TRANSPARENT (1 << 15) // transparent for chars and sprites

#define RAM_PIC     0x0000    // Screen Picture, 64 x 64 = 4096 bytes
#define RAM_CHR     0x1000    // Screen Characters, 256 x 16 = 4096 bytes
#define RAM_PAL     0x2000    // Screen Character Palette, 256 x 8 = 2048 bytes

#define IDENT         0x2800
#define REV           0x2801
#define FRAME         0x2802
#define VBLANK        0x2803
#define SCROLL_X      0x2804
#define SCROLL_Y      0x2806
#define JK_MODE       0x2808
#define J1_RESET      0x2809
#define SPR_DISABLE   0x280a
#define SPR_PAGE      0x280b
#define IOMODE        0x280c

#define BG_COLOR      0x280e
#define SAMPLE_L      0x2810
#define SAMPLE_R      0x2812

#define MODULATED     0x2814
#define UNMODULATED   0x2815

#define SCREENSHOT_Y  0x281e

#define PALETTE16A 0x2840   // 16-color palette RAM A, 32 bytes
#define PALETTE16B 0x2860   // 16-color palette RAM B, 32 bytes
#define PALETTE4A  0x2880   // 4-color palette RAM A, 8 bytes
#define PALETTE4B  0x2888   // 4-color palette RAM A, 8 bytes
#define COMM       0x2890   // Communication buffer
#define COLLISION  0x2900   // Collision detection RAM, 256 bytes
#define VOICES     0x2a00   // Voice controls
#define J1_CODE    0x2b00   // J1 coprocessor microcode RAM
#define SCREENSHOT 0x2c00   // screenshot line RAM

#define RAM_SPR     0x3000    // Sprite Control, 512 x 4 = 2048 bytes
#define RAM_SPRPAL  0x3800    // Sprite Palettes, 4 x 256 = 2048 bytes
#define RAM_SPRIMG  0x4000    // Sprite Image, 64 x 256 = 16384 bytes

#endif

