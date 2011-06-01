/*
 * Copyright (c) 2011 by James Bowman <jamesb@excamera.com>
 * Gameduino library for arduino.
 *
 */

#include "WProgram.h"
#include "avr/pgmspace.h"
#include "GD.h"
#include "SPI.h"

GDClass GD;

void GDClass::begin() {
  pinMode(SS_PIN, OUTPUT);
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  // SPSR = (1 << SPI2X);
  digitalWrite(SS_PIN, HIGH);

  GD.wr(J1_RESET, 1);           // HALT coprocessor
  fill(RAM_PIC, 0, 1024 * 10);  // Zero all character RAM
  fill(RAM_SPRPAL, 0, 2048);    // Sprite palletes black
  fill(RAM_SPR, 0, 64 * 256);   // Clear all sprite data
  fill(VOICES, 0, 256);         // Silence
  fill(PALETTE16A, 0, 128);     // Black 16-, 4-palletes and COMM

  GD.wr16(SCROLL_X, 0);
  GD.wr16(SCROLL_Y, 0);
  GD.wr(JK_MODE, 0);
  GD.wr(SPR_DISABLE, 0);
  GD.wr(SPR_PAGE, 0);
  GD.wr(IOMODE, 0);
  GD.wr16(BG_COLOR, 0);
  GD.wr16(SAMPLE_L, 0);
  GD.wr16(SAMPLE_R, 0);
  GD.wr16(SCREENSHOT_Y, 0);
  GD.wr(MODULATED, 0);
  GD.wr(UNMODULATED, 0);
  int16_t i;
  __wstart(RAM_SPR);
  for (i = 0; i < 512; i++)
    GD.xhide();
  __end();
}

void GDClass::end() {
}

void GDClass::__start(uint16_t addr) // start an SPI transaction to addr
{
  digitalWrite(SS_PIN, LOW);
  SPI.transfer(highByte(addr));
  SPI.transfer(lowByte(addr));  
}

void GDClass::__wstart(uint16_t addr) // start an SPI write transaction to addr
{
  __start(0x8000|addr);
}

void GDClass::__wstartspr(uint16_t sprnum)
{
  __start((0x8000 | RAM_SPR) + (sprnum << 2));
  spr = 0;
}

void GDClass::__end() // end the SPI transaction
{
  digitalWrite(SS_PIN, HIGH);
}

byte GDClass::rd(uint16_t addr)
{
  __start(addr);
  byte r = SPI.transfer(0);
  __end();
  return r;
}

void GDClass::wr(uint16_t addr, byte v)
{
  __wstart(addr);
  SPI.transfer(v);
  __end();
}

uint16_t GDClass::rd16(uint16_t addr)
{
  uint16_t r;

  __start(addr);
  r = SPI.transfer(0);
  r |= (SPI.transfer(0) << 8);
  __end();
  return r;
}

void GDClass::wr16(uint16_t addr, uint16_t v)
{
  __wstart(addr);
  SPI.transfer(lowByte(v));
  SPI.transfer(highByte(v));
  __end();
}

void GDClass::fill(int16_t addr, byte v, uint16_t count)
{
  __wstart(addr);
  while (count--)
    SPI.transfer(v);
  __end();
}

void GDClass::copy(uint16_t addr, PROGMEM prog_uchar *src, int16_t count)
{
  __wstart(addr);
  while (count--) {
    SPI.transfer(pgm_read_byte_near(src));
    src++;
  }
  __end();
}

void GDClass::microcode(PROGMEM prog_uchar *src, int16_t count)
{
  wr(J1_RESET, 1);
  copy(J1_CODE, src, count);
  wr(J1_RESET, 0);
}

void GDClass::setpal(int16_t pal, uint16_t rgb)
{
  wr16(RAM_PAL + (pal << 1), rgb);
}

void GDClass::sprite(int16_t spr, int16_t x, int16_t y, byte image, byte palette, byte rot, byte jk)
{
  __wstart(RAM_SPR + (spr << 2));
  SPI.transfer(lowByte(x));
  SPI.transfer((palette << 4) | (rot << 1) | (highByte(x) & 1));
  SPI.transfer(lowByte(y));
  SPI.transfer((jk << 7) | (image << 1) | highByte(y) & 1);
  __end();
}

void GDClass::xsprite(int16_t ox, int16_t oy, char x, char y, byte image, byte palette, byte rot, byte jk)
{
  if (rot & 2)
    x = -16-x;
  if (rot & 4)
    y = -16-y;
  if (rot & 1) {
      int16_t s;
      s = x; x = y; y = s;
  }
  ox += x;
  oy += y;
  SPI.transfer(lowByte(ox));
  SPI.transfer((palette << 4) | (rot << 1) | (highByte(ox) & 1));
  SPI.transfer(lowByte(oy));
  SPI.transfer((jk << 7) | (image << 1) | highByte(oy) & 1);
  spr++;
}

void GDClass::xhide()
{
  SPI.transfer(lowByte(400));
  SPI.transfer(highByte(400));
  SPI.transfer(lowByte(400));
  SPI.transfer(highByte(400));
  spr++;
}

void GDClass::sprite2x2(int16_t spr, int16_t x, int16_t y, byte image, byte palette, byte rot, byte jk)
{
  __wstart(0x3000 + (spr << 2));
  GD.xsprite(x, y, -16, -16, image + 0, palette, rot, jk);
  GD.xsprite(x, y,   0, -16, image + 1, palette, rot, jk);
  GD.xsprite(x, y, -16,   0, image + 2, palette, rot, jk);
  GD.xsprite(x, y,   0,   0, image + 3, palette, rot, jk);
  __end();
}

void GDClass::waitvblank()
{
  // Wait for the VLANK to go from 0 to 1: this is the start
  // of the vertical blanking interval.

  while (rd(VBLANK) == 1)
    ;
  while (rd(VBLANK) == 0)
    ;
}

/* Fixed ascii font, useful for debug */

#include "font8x8.h"
static byte stretch[16] = {
  0x00, 0x03, 0x0c, 0x0f,
  0x30, 0x33, 0x3c, 0x3f,
  0xc0, 0xc3, 0xcc, 0xcf,
  0xf0, 0xf3, 0xfc, 0xff
};
void GDClass::ascii()
{
  int16_t i;
  for (i = 0; i < 768; i++) {
    byte b = pgm_read_byte_near(font8x8 + i);
    byte h = stretch[b >> 4];
    byte l = stretch[b & 0xf];
    GD.wr(0x1000 + (16 * ' ') + (2 * i), h);
    GD.wr(0x1000 + (16 * ' ') + (2 * i) + 1, l);
  }
  for (i = 0x20; i < 0x80; i++) {
    GD.setpal(4 * i + 0, TRANSPARENT);
    GD.setpal(4 * i + 3, RGB(255,255,255));
  }
  GD.fill(RAM_PIC, ' ', 4096);
}

void GDClass::putstr(int16_t x, int16_t y, const char *s)
{
  GD.__wstart((y << 6) + x);
  while (*s)
    SPI.transfer(*s++);
  GD.__end();
}

void GDClass::voice(int16_t v, byte wave, uint16_t freq, byte lamp, byte ramp)
{
  __wstart(VOICES + (v << 2));
  SPI.transfer(lowByte(freq));
  SPI.transfer(highByte(freq) | (wave << 7));
  SPI.transfer(lamp);
  SPI.transfer(ramp);
  __end();
}

void GDClass::screenshot(uint16_t frame)
{
  int16_t yy, xx;
  byte undone[38];  // 300-long bitmap of lines pending

  // initialize to 300 ones
  memset(undone, 0xff, 37);
  undone[37] = 0xf;
  int16_t nundone = 300;

  Serial.write(0xa5);   // sync byte
  Serial.write(lowByte(frame));
  Serial.write(highByte(frame));

  while (nundone) {
    // find a pending line a short distance ahead of the raster
    int16_t hwline = GD.rd16(SCREENSHOT_Y) & 0x1ff;
    for (yy = (hwline + 7) % 300; ((undone[yy>>3] >> (yy&7)) & 1) == 0; yy = (yy + 1) % 300)
      ;
    GD.wr16(SCREENSHOT_Y, 0x8000 | yy);   // ask for it

    // housekeeping while waiting: mark line done and send yy
    undone[yy>>3] ^= (1 << (yy&7));
    nundone--;
    Serial.write(lowByte(yy));
    Serial.write(highByte(yy));
    while ((GD.rd(SCREENSHOT_Y + 1) & 0x80) == 0)
      ;

    // Now send the line, compressing zero pixels
    uint16_t zeroes = 0;
    for (xx = 0; xx < 800; xx += 2) {
      uint16_t v = GD.rd16(SCREENSHOT + xx);
      if (v == 0) {
        zeroes++;
      } else {
        if (zeroes) {
          Serial.write(lowByte(zeroes));
          Serial.write(0x80 | highByte(zeroes));
          zeroes = 0;
        }
        Serial.write(lowByte(v));
        Serial.write(highByte(v));
      }
    }
    if (zeroes) {
      Serial.write(lowByte(zeroes));
      Serial.write(0x80 | highByte(zeroes));
    }
  }
  GD.wr16(SCREENSHOT_Y, 0);   // restore screen to normal
}

class GDflashbits {
public:
  void begin(prog_uchar *s) {
    src = s;
    mask = 0x01;
  }
  byte get1(void) {
    byte r = (pgm_read_byte_near(src) & mask) != 0;
    mask <<= 1;
    if (!mask) {
      mask = 1;
      src++;
    }
    return r;
  }
  unsigned short getn(byte n) {
    unsigned short r = 0;
    while (n--) {
      r <<= 1;
      r |= get1();
    }
    return r;
  }
private:
  prog_uchar *src;
  byte mask;
};

static GDflashbits GDFB;

void GDClass::uncompress(uint16_t addr, prog_uchar *src)
{
  GDFB.begin(src);
  byte b_off = GDFB.getn(4);
  byte b_len = GDFB.getn(4);
  byte minlen = GDFB.getn(2);
  unsigned short items = GDFB.getn(16);
  while (items--) {
    if (GDFB.get1() == 0) {
      GD.wr(addr++, GDFB.getn(8));
    } else {
      int16_t offset = -GDFB.getn(b_off) - 1;
      int16_t l = GDFB.getn(b_len) + minlen;
      while (l--) {
        GD.wr(addr, GD.rd(addr + offset));
        addr++;
      }
    }
  }
}
