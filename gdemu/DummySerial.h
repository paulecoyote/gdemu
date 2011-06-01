/**
 * DummySerial
 * $Id$
 * \file DummySerial.h
 * \brief DummySerial
 */

/* 
 * Copyright (C) 2011  Jan Boon (Kaetemi)
 * 
 * This file is part of GAMEDUINO EMU.
 * GAMEDUINO EMU is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 2 of the
 * License, or (at your option) any later version.
 * 
 * GAMEDUINO EMU is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GAMEDUINO EMU; see the file COPYING.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef GDEMU_DUMMYSERIAL_H
#define GDEMU_DUMMYSERIAL_H
// #include <...>

// System includes

// Project includes
#include "wiring.h"
#include "Stream.h"

/**
 * DummySerial
 * \brief DummySerial
 */
class DummySerial : public Stream
{
public:
	inline DummySerial() { }

    void begin(long) { }
    void end() { }
    virtual int available(void) { return -1; }
    virtual int peek(void) { return -1; }
    virtual int read(void) { return -1; }
    virtual void flush(void) { }
    virtual void write(uint8_t);
	
private:
	DummySerial(const DummySerial &);
	DummySerial &operator=(const DummySerial &);
	
}; /* class DummySerial */

extern DummySerial Serial;

#endif /* #ifndef GDEMU_DUMMYSERIAL_H */

/* end of file */
