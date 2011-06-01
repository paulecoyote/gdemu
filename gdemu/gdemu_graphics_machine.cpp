/**
 * GraphicsMachineClass
 * $Id$
 * \file gdemu_graphics_machine.cpp
 * \brief GraphicsMachineClass
 * \date 2011-05-25 20:50GMT
 * \author Jan Boon (Kaetemi)
 */

/* 
 * Copyright (C) 2011  Jan Boon (Kaetemi)
 * 
 * This file is part of GAMEDUINO EMULATOR.
 * GAMEDUINO EMULATOR is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 2 of the
 * License, or (at your option) any later version.
 * 
 * GAMEDUINO EMULATOR is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GAMEDUINO EMULATOR; see the file COPYING.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

// #include <...>
#include "gdemu_graphics_machine.h"

// System includes
#include "WProgram.h"

// Project includes
#include "GD.h"
#include "gdemu_system.h"
#include "gdemu_graphics_driver.h"
#include "gdemu_gameduino_spi.h"
#include "gdemu_j1.h"

// using namespace ...;

#define YLINE 0x8000

namespace GDEMU {

GraphicsMachineClass GraphicsMachine;
static unsigned char lineColl[400];

static inline int isAlpha1555(unsigned short argb1555) { return (argb1555 & 0x8000); }

static inline uint16_t readUInt16(unsigned char *gameduinoRam, int offset)
{
	return *(uint16_t *)(&gameduinoRam[offset]);
}

static inline uint16_t readUInt16Flipped(unsigned char *gameduinoRam, int offset)
{
	uint16_t value = readUInt16(gameduinoRam, offset);
	return value >> 8 | value << 8; // (gameduinoRam[offset] << 8) | (gameduinoRam[offset + 1]);
}

static inline void writeUInt16(unsigned char *gameduinoRam, int offset, uint16_t value)
{
	// printf("write: %i\n", value);
	*(uint16_t *)(&gameduinoRam[offset]) = value;
}

void GraphicsMachineClass::process()
{
	argb1555 *screenArgb1555 = GraphicsDriver.getBufferARGB1555();
	unsigned char *gameduinoRam = GameduinoSPI.getRam();	
	int *gameduinoRam32 = (int *)(void *)gameduinoRam;

	// clear collision ram
	memset(gameduinoRam + COLLISION, 0xFF, 256);

	unsigned long timeBegin;
	unsigned long timeDelta;

	timeBegin = System.getMicros();

	for (int y = 0; y < 300; ++y)
	{
		writeUInt16(gameduinoRam, YLINE, y);

		// for raster chasing purposes
		if (!J1.isResetting())
			J1.execute(256, 0);

		memset(lineColl, 0xFF, 400);
		argb1555 bgcolor = readUInt16(gameduinoRam, BG_COLOR); // only support one bgcolor per line, timing is not fine enough atm

		// character background
		{
			int scrollX = readUInt16(gameduinoRam, SCROLL_X);
			int scrollY = readUInt16(gameduinoRam, SCROLL_Y);
			
			int gridLineY = scrollY + y;
			gridLineY %= 512;
			int gridY = gridLineY / 8;
			int charY = gridLineY % 8;

			// Draw line from right to left for easier shifting
			int gridRowX = (scrollX % 512);
			int gridXEnd = gridRowX/ 8;
			int gridXBegin = gridXEnd + 50;
			int charX = 7 - (gridRowX % 8);
			
			for (int gridX = gridXBegin, x = 399; x >= 0; --gridX)
			{
				gridX = (gridX + 64) % 64;

				int charPic = gameduinoRam[RAM_PIC + (gridY * 64) + gridX];

				int chrIdx = RAM_CHR + (charPic * 16); // 256 * 16 = 4096 bytes
				int palIdx = RAM_PAL + (charPic * 8);
				
				unsigned short chrRow = readUInt16Flipped(gameduinoRam, chrIdx + ((charY) * 2));
				chrRow >>= (charX * 2);

				for (; charX < 8 && x >= 0; ++charX)
				{
					int screenOffset = ((299 - y) * /*3 * */400) + ((x) /** 3*/);
					unsigned short pixelBits = chrRow & 0x03;
					argb1555 pixel = readUInt16(gameduinoRam, palIdx + (pixelBits * 2));
					if (isAlpha1555(pixel)) screenArgb1555[screenOffset] = bgcolor;
					else screenArgb1555[screenOffset] = pixel;
					--x;
					chrRow >>= 2;
				}
				charX = 0;
			}
		}

		// sprites foreground
		if (!(gameduinoRam[SPR_DISABLE] & 0x01))
		{
			int spritesProcessed = 0;
			bool jkMode = (gameduinoRam[JK_MODE] & 0x01);
			int ramSpr = RAM_SPR + ((gameduinoRam[SPR_PAGE] & 0x01) * 0x0400);
			int ramSpr32 = ramSpr >> 2;

			for (int sprI = 0; sprI < 256; ++sprI)
			{
				int ramSprC32 = ramSpr32 + sprI;
				int spriteControl = gameduinoRam32[ramSprC32];
				int yCoord = (((spriteControl >> 16) + 16) & 0x01FF) - 16; // if (ycoord + 16 > 512) ycoord -= 512;
				
				if (yCoord <= y && yCoord + 15 >= y)
				{
					int xCoord = (((spriteControl) + 16) & 0x01FF) - 16;
					
					if (xCoord < 400 || xCoord + 15 >= 0)
					{
						int colIdx = COLLISION + sprI;
						int imgIdx = RAM_SPRIMG + ((spriteControl >> 17) & 0x3F00);
						
						int fromX = xCoord < 0 ? -xCoord : 0;
						int toX = xCoord + 15 >= 400 ? 400 - xCoord : 16;
						
						int paletteSelect = spriteControl >> 12;
						int paletteIdx;
						int pixelColorShiftRight;
						int pixelColorMask;

						if ((paletteSelect & 0x0C) == 0)
						{
							int paletteNb = paletteSelect & 0x3;
							paletteIdx = RAM_SPRPAL + (paletteNb * 512); // + (((pixelColor >> 0) & 0xFF) << 1);
							pixelColorShiftRight = 0;
							pixelColorMask = 0xFF;
						}
						else if ((paletteSelect & 0x0C) == 0x04)
						{
							//16color
							int ab = paletteSelect & 0x1;
							int n = (paletteSelect << 1) & 0x04;
							pixelColorShiftRight = n;
							pixelColorMask = 0x0F;
							paletteIdx = PALETTE16A + ((PALETTE16B - PALETTE16A) * ab); // + (((pixelColor >> n) & 0x0F) << 1);
						}
						else if (paletteSelect & 0x08)
						{
							//4color
							int ab = paletteSelect & 0x01;
							int n = paletteSelect & 0x06;
							pixelColorShiftRight = n;
							pixelColorMask = 0x03;
							paletteIdx = PALETTE4A + ((PALETTE4B - PALETTE4A) * ab); // + (((pixelColor >> n) & 0x03) << 1);
						}

						for (int xd = fromX; xd < toX; ++xd)
						{
							int x = xd + xCoord;
							int screenOffset = ((299 - y) * /*3 * */400) + ((x) /** 3*/);

							int screenCollOffset = x;
							
							int imgX = xd;
							int imgY = y - yCoord;
							
							if (spriteControl & 0x0200) { int ix = imgX; imgX = imgY; imgY = ix; } // flip x y
							if (spriteControl & 0x0400) imgX = 15 - imgX; // neg x
							if (spriteControl & 0x0800) imgY = 15 - imgY; // neg y
							
							int imageOffset = (16 * imgY) + imgX;
							int pixelColor = gameduinoRam[imgIdx + imageOffset];
							
							argb1555 argbColor = readUInt16(gameduinoRam, paletteIdx 
								+ (((pixelColor >> pixelColorShiftRight) & pixelColorMask) << 1));
							
							if (!isAlpha1555(argbColor))
							{
								{
									if (lineColl[screenCollOffset] != 0xFF)
									{
										/*(gameduinoRam[lineColl[screenCollOffset] + 3] >> 7)*/
										if (!(jkMode && ((gameduinoRam32[ramSpr32 + lineColl[screenCollOffset]] >> 31) == (spriteControl >> 31))))
										{
											// QUESTION: OVERWRITE POSSIBLY EXISTING VALUE IN COLLISION REGISTER OR NOT?
											gameduinoRam[colIdx] = lineColl[screenCollOffset];
										}
									}
									// else 
									{
										// QUESTION: OVERWRITE PREVIOUS SPRITE IN COLLISION CHECK BUFFER OR NOT?
										lineColl[screenCollOffset] = sprI;
									}
								}
							
								screenArgb1555[screenOffset] = argbColor;
							}
						}

						++spritesProcessed;
						if (spritesProcessed + 1 > 96)
							break;
					}
				}
			}
		}

		//writeUInt16(gameduinoRam, YLINE, y + 1);
		
		// screenshot support
		if (GameduinoSPI.isScreenshotRequested())
		{
			uint16_t screenshotY = readUInt16(gameduinoRam, SCREENSHOT_Y);
			if (screenshotY == y)
			{
				memcpy((void *)(&gameduinoRam[SCREENSHOT]), (void *)(&screenArgb1555[400 * y]), 800);
				gameduinoRam[SCREENSHOT_Y + 1] |= 0x80;
			}
		}
	}
	for (int y = 300; y < 333; ++y)
	{
		writeUInt16(gameduinoRam, YLINE, y);
		
		// for more raster chasing purposes
		if (!J1.isResetting())
			J1.execute(64, 0);

		// writeUInt16(gameduinoRam, YLINE, (y + 1) % 332);
	}

	timeDelta = System.getMicros() - timeBegin;
	//printf("render: %i micros (%i ms)\r\n", (int)timeDelta, (int)timeDelta / 1000);

	++gameduinoRam[FRAME];
}

void GraphicsMachineClass::flip()
{
	GraphicsDriver.renderBuffer();
	if (!GraphicsDriver.update()) exit(0); // llalallaa
}

} /* namespace GDEMU */

/* end of file */
