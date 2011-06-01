/**
 * SPIClass
 * $Id$
 * \file SPI.cpp
 * \brief SPIClass
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
#include "SPI.h"

// System includes

// Project includes
#include "gdemu_gameduino_spi.h"

// using namespace ...;

SPIClass SPI;

byte SPIClass::transfer(byte _data)
{
	return GDEMU::GameduinoSPI.transfer(_data);
}

/* end of file */
