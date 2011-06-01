/**
 * WProgram
 * $Id$
 * \file WProgram.h
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

#ifndef WPROGRAM_H
#define WPROGRAM_H
// #include <...>

// System includes
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Project includes
#include "wiring.h"
#include "WString.h"
#include "DummySerial.h"

// Functions
int32_t random(int32_t);
int32_t random(int32_t, int32_t);
void randomSeed(uint32_t);

#endif /* #ifndef WPROGRAM_H */

/* end of file */
