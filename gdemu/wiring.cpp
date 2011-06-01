/**
 * wiring
 * $Id$
 * \file wiring.cpp
 * \brief wiring
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

#include <stdio.h>
#include "wiring.h"

// System includes

// Project includes
#include "gdemu_system.h"
#include "gdemu_gameduino_spi.h"

// using namespace ...;


// Defines
#define GDEMU_GAMEDUINO_SS_PIN          9
#define GDEMU_ARDUINO_INTERRUPT_PIN_0   2
#define GDEMU_ARDUINO_INTERRUPT_PIN_1   3


// Digital pins
static uint8_t s_DigitalPins[64] = { 0 };
void pinMode(uint8_t pin, uint8_t mode) { if (mode == INPUT) s_DigitalPins[pin] = LOW; }

// Interrupts
static const int s_InterruptPins[2] = { GDEMU_ARDUINO_INTERRUPT_PIN_0, GDEMU_ARDUINO_INTERRUPT_PIN_1 };
static void (*s_InterruptFunctions[2])() = { 0 };
static int s_InterruptModes[2] = { 0 };


// Interrupts
static void gdemuCallInterrupt(uint8_t i)
{
	bool isdt = GDEMU::System.isDuinoThread();
	if (!isdt) GDEMU::System.holdDuinoThread();
	s_InterruptFunctions[i]();
	if (!isdt) GDEMU::System.resumeDuinoThread();
}

static void gdemuHandleInterrupt(uint8_t i, uint8_t val, uint8_t prev)
{
	if (s_InterruptFunctions[i])
	{
		switch (s_InterruptModes[i])
		{
		case LOW:
			if (val == LOW)
				gdemuCallInterrupt(i);
			break;
		case CHANGE:
			if (val != prev)
				gdemuCallInterrupt(i);
			break;
		case RISING:
			if (val == HIGH && prev == LOW)
				gdemuCallInterrupt(i);
			break;
		case FALLING: 
			if (val == LOW && prev == HIGH)
				gdemuCallInterrupt(i);
			break;
		}
	}
}

void attachInterrupt(uint8_t interrupt, void (*function)(void), int mode)
{
	s_InterruptModes[interrupt] = mode;
	s_InterruptFunctions[interrupt] = function;
}

void detachInterrupt(uint8_t interrupt)
{
	s_InterruptFunctions[interrupt] = NULL;
}


// Time
uint32_t millis(void) { return (uint32_t)GDEMU::System.getMillis(); }
uint32_t micros(void) { return (uint32_t)GDEMU::System.getMicros(); }
void delay(uint32_t ms) { GDEMU::System.delay(ms); }
void delayMicroseconds(uint16_t us) { GDEMU::System.delayMicros((int)us); }


// Digital pins
void digitalWrite(uint8_t pin, uint8_t val)
{
	val = val > 0 ? HIGH : LOW;
	uint8_t prev = s_DigitalPins[pin];
	s_DigitalPins[pin] = val;
	switch (pin)
	{
	case GDEMU_GAMEDUINO_SS_PIN:
		GDEMU::GameduinoSPI.slaveSelectChanged(val);
		break;
	case GDEMU_ARDUINO_INTERRUPT_PIN_0:
		gdemuHandleInterrupt(0, val, prev);
		break;
	case GDEMU_ARDUINO_INTERRUPT_PIN_1:
		gdemuHandleInterrupt(1, val, prev);
		break;
	}
}

uint8_t digitalRead(uint8_t pin)
{
	return s_DigitalPins[pin];
}


// Analog pins
int16_t analogRead(uint8_t pin)
{
	return GDEMU::System.getAnalogRead(pin);
}

/* end of file */
