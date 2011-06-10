/**
 * GameduinoSPIClass
 * $Id$
 * \file gdemu_gameduino_spi.cpp
 * \brief GameduinoSPIClass
 * \date 2011-05-29 19:47GMT
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
#include "gdemu_gameduino_spi.h"

// System includes
#include <stdlib.h>
#include <stdio.h>

// Project includes
#include "gdemu_system.h"
#include "GD.h"

// using namespace ...;

namespace GDEMU {

GameduinoSPIClass GameduinoSPI;

// RAM
static uint8_t s_GdRam[65536 + 1]; // should be enough <:o)

// Read/write state
static uint8_t s_LastSS = HIGH;
static int s_WaitAddr = 2;
static int s_CurrentAddress = 0;
static bool s_Writing = false;

// VBlank switch buffer to avoid pulse misses caused by sleeping threads - arduino target
static uint8_t s_LastReturnedVBlank = 0;
static bool s_VBlankPulsedSinceLastRequest = false;

// VBlank switch buffer to avoid pulse misses caused by sleeping threads - j1 target
static short s_LastReturnedVBlank16 = 0;
static bool s_VBlankPulsedSinceLastRequest16 = false;

// J1Reset switch buffer to avoid pulse misses
static uint8_t s_LastReturnedJ1Reset = 0;
static bool s_J1ResetPulsedSinceLastRequest = false;

// Avoid getting hammered in wait loops
static int s_LastRequestedAddress = -1;
static int s_IdenticalRequestCounter = 0;

// Screenshot Y request
static bool s_ScreenshotRequested = false;

// Buffer because we don't playback realtime
// Implementation assumes native sampling rate (8000Hz) feed for convenience
#define D_GDEMU_SAMPLEBUFFER 800
static uint8_t s_SampleL1[D_GDEMU_SAMPLEBUFFER + 1];
static uint8_t s_SampleL2[D_GDEMU_SAMPLEBUFFER + 1];
static int s_SampleLW = 1;
static int s_SampleLR = 0;
static uint8_t s_SampleR1[D_GDEMU_SAMPLEBUFFER + 1];
static uint8_t s_SampleR2[D_GDEMU_SAMPLEBUFFER + 1];
static int s_SampleRW = 1;
static int s_SampleRR = 0;

void GameduinoSPIClass::begin()
{
	GDEMU::s_GdRam[IDENT] = 0x6D;
	GDEMU::s_GdRam[REV] = 0x10;
	GDEMU::s_GdRam[J1_RESET] = 0x01;
	GDEMU::s_SampleL1[0] = 0;
	GDEMU::s_SampleL2[0] = 0;
	GDEMU::s_SampleR1[0] = 0;
	GDEMU::s_SampleR2[0] = 0;
	// In case of temporary overflow due to racing conditions
	GDEMU::s_SampleL1[D_GDEMU_SAMPLEBUFFER] = 0;
	GDEMU::s_SampleL2[D_GDEMU_SAMPLEBUFFER] = 0;
	GDEMU::s_SampleR1[D_GDEMU_SAMPLEBUFFER] = 0;
	GDEMU::s_SampleR2[D_GDEMU_SAMPLEBUFFER] = 0;
	((short *)(void *)&GDEMU::s_GdRam[0x800a])[0] = 8000; // FREQHZ
}

void GameduinoSPIClass::end()
{

}

void GameduinoSPIClass::setVBlank(uint8_t value)
{
	if (value != s_LastReturnedVBlank)
		s_VBlankPulsedSinceLastRequest = true;
	if (value != s_LastReturnedVBlank16)
		s_VBlankPulsedSinceLastRequest16 = true;
	s_GdRam[VBLANK] = value;
}

bool GameduinoSPIClass::isSlaveSelected()
{
	return s_LastSS == LOW;
}

uint8_t GameduinoSPIClass::getSSPin()
{
	return SS_PIN;
}

uint8_t *GameduinoSPIClass::getRam()
{
	return s_GdRam;
}

void GameduinoSPIClass::slaveSelectChanged(uint8_t val)
{
	if (s_LastSS == HIGH && val == LOW)
	{
		s_WaitAddr = 2;
		s_CurrentAddress = 0;
	}
	s_LastSS = val;
}

unsigned short GameduinoSPIClass::getNextSampleL()
{
	short result = ((unsigned short)s_SampleL2[s_SampleLR]) << 8 | ((unsigned short)s_SampleL1[s_SampleLR] & 0xFF);

	int nextL = (s_SampleLR + 1) % D_GDEMU_SAMPLEBUFFER;
	if (nextL != s_SampleLW)
	{
		s_SampleLR = nextL;
	}
	
	return result;
}

unsigned short GameduinoSPIClass::getNextSampleR()
{
	short result = ((unsigned short)s_SampleR2[s_SampleRR]) << 8 | ((unsigned short)s_SampleR1[s_SampleRR] & 0xFF);

	int nextR = (s_SampleRR + 1) % D_GDEMU_SAMPLEBUFFER;
	if (nextR != s_SampleRW)
	{
		s_SampleRR = nextR;
	}
	
	return result;
}

uint8_t GameduinoSPIClass::getJ1Reset()
{
	if (s_J1ResetPulsedSinceLastRequest)
	{
		s_J1ResetPulsedSinceLastRequest = false;
		s_LastReturnedJ1Reset = s_LastReturnedJ1Reset == 1 ? 0 : 1;
	}
	else
		s_LastReturnedJ1Reset = s_GdRam[J1_RESET];
	return s_LastReturnedJ1Reset;
}

bool GameduinoSPIClass::isScreenshotRequested()
{
	return s_ScreenshotRequested;
}

// for use only by user code that would normally run on the arduino
uint8_t inline GameduinoSPIClass::readRam(int offset)
{
	if (s_LastRequestedAddress == offset)
	{
		++s_IdenticalRequestCounter;
		if (s_IdenticalRequestCounter > 8)
		{
			switch (offset)
			{
			case VBLANK:
				if (!s_VBlankPulsedSinceLastRequest)
				{
					// printf("anti vblank spam\n");
					System.delay(1);
				}
				break;
			default:
				// printf("anti read spam\n");
				System.switchThread(); // die :)
				// delay(0); // die :)
				break; 
			}
		}
	}
	else
	{
		s_IdenticalRequestCounter = 0;
		s_LastRequestedAddress = offset;
	}

	switch (offset)
	{
	case VBLANK:
		{
		if (s_VBlankPulsedSinceLastRequest)
		{
			s_VBlankPulsedSinceLastRequest = false;
			s_LastReturnedVBlank = s_LastReturnedVBlank == 1 ? 0 : 1;
		}
		else
			s_LastReturnedVBlank = s_GdRam[VBLANK];
		return s_LastReturnedVBlank;
		}
		break;
	}

	return s_GdRam[offset];
}

// for use only by user code that would normally run on the arduino
void inline GameduinoSPIClass::writeRam(int offset, uint8_t value)
{
	s_GdRam[offset] = value;
	
	switch (offset)
	{
	case SAMPLE_L:
		s_SampleL1[s_SampleLW] = value;
		break;
	case SAMPLE_L + 1:
		s_SampleL2[s_SampleLW] = value;
		++s_SampleLW;
		s_SampleLW %= D_GDEMU_SAMPLEBUFFER;
		break;
	case SAMPLE_R:
		s_SampleR1[s_SampleRW] = value;
		break;
	case SAMPLE_R + 1:
		++s_SampleRW;
		s_SampleR2[s_SampleRW] = value;
		s_SampleRW %= D_GDEMU_SAMPLEBUFFER;
		break;
	case SCREENSHOT_Y + 1:
		if (value & 0x80)
		{
			s_GdRam[offset] = value & 0x7F;
			s_ScreenshotRequested = true;
		}
		break;
	case J1_RESET:
		if (value != s_LastReturnedJ1Reset)
			s_J1ResetPulsedSinceLastRequest = true;
		break;
	}
}

//int readnb = 0;
//short lastyline = 0;
// for use only by user code running on the j1
short GameduinoSPIClass::readRam16(int offset)
{
	switch (offset)
	{
	case VBLANK:
		{
		if (s_VBlankPulsedSinceLastRequest16)
		{
			s_VBlankPulsedSinceLastRequest16 = false;
			s_LastReturnedVBlank16 = s_LastReturnedVBlank16 == 1 ? 0 : 1;
		}
		else
			s_LastReturnedVBlank16 = s_GdRam[VBLANK];
		return s_LastReturnedVBlank16;
		}		
	case 0x8012: // RANDOM
		return (rand() & 0xFF) | ((rand() & 0xFF) << 8);
	case 0x800e: // P2_V
		return digitalRead(2);
	case 0x800c: // FREQTICK
		short freqhz = ((short *)(void *)&s_GdRam[0x800a])[0];
		return (short)(System.getMicros() * freqhz / 10000000); // todo: be able to sync with sound samples :)
	}

	short result = ((short *)(void *)&s_GdRam[offset])[0];
	if (offset < 0x8000) result &= 0xFF;
	
	// printf("read8j1: %i @ %i\n", (int)result, offset);//(offset << 1));

	return result;
}

// for use only by user code running on the j1
void GameduinoSPIClass::writeRam16(int offset, short value)
{
	if (offset < 0x8000)
	{
		// works fine for now, since there's no dupe needed for this atm
		writeRam(offset, value & 0xFF);
	}
	else
	{
		((short *)(void *)&s_GdRam[offset])[0] = value;
		switch (offset)
		{
		case 0x800e: // P2_V
			digitalWrite(2, value);
			break;
		}
	}

	//printf("write8j1: %i @ %i\n", (int)value, offset);
}

uint8_t GameduinoSPIClass::transfer(uint8_t data)
{
	if (isSlaveSelected())
	{
		if (s_WaitAddr == 0)
		{
			int cAddr = s_CurrentAddress;
			++s_CurrentAddress;
			if (s_Writing)
			{
				writeRam(cAddr, data);
			}
			else
			{
				return readRam(cAddr);
			}
		}
		else if (s_WaitAddr == 2)
		{
			uint8_t writing = data & 0x80;
			uint8_t hb = data - writing;
			s_CurrentAddress = hb;
			s_CurrentAddress <<= 8;
			s_Writing = (writing == 0x80);
			--s_WaitAddr;
		}
		else if (s_WaitAddr == 1)
		{
			s_CurrentAddress |= data;
			/*uint8_t writing = data & 0x80;
			int hb = data & 0x7F;
			s_CurrentAddress |= (data << 8);
			s_Writing = (writing == 0x80);*/
			// printf("%s @ %i\r\n", s_Writing ? "WRITE" : "READ", s_CurrentAddress);
			--s_WaitAddr;
		}
		return 0;
	}
	else if (digitalRead(2) == LOW && s_GdRam[IOMODE] == 'F')
	{
		// dummy stuff for satisfying selftest.cpp
		if (data == 0 && s_CurrentAddress == 0xd7)
			return 0x94;
		s_CurrentAddress = data;
		return 0;
	}
	else
	{
		return 0;
	}
}


} /* namespace GDEMU */

/* end of file */
