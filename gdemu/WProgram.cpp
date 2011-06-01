/**
 * WProgram
 * $Id$
 * \file WProgram.cpp
 * \brief WProgram
 */

/* 
 * Copyright (C) 2011  by authors
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
#include "WProgram.h"

// System includes

// Project includes

// using namespace ...;

void randomSeed(uint32_t seed)
{
	srand(seed);
}

int32_t random(int32_t howbig)
{
	if (howbig == 0) return 0;
	long value = (rand() & 0xFF) | ((rand() & 0xFF) << 8) | ((rand() & 0xFF) << 16) | ((rand() & 0x7F) << 24);
	value %= howbig;
	return value;
}

int32_t random(int32_t howsmall, int32_t howbig)
{
	if (howsmall >= howbig) return howsmall;
	int32_t diff = howbig - howsmall;
	return random(diff) + howsmall;
}

/* end of file */
