/**
 * inttypes
 * $Id$
 * \file inttypes.h
 * \brief inttypes
 * \date 2011-06-01 19:00GMT
 * \author Jan Boon (Kaetemi)
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

#ifndef INTTYPES_H
#define INTTYPES_H
// #include <...>

// System includes

// Project includes

#ifdef _MSC_VER
typedef unsigned __int8 uint8_t;
typedef signed __int8 int8_t;
typedef unsigned __int16 uint16_t;
typedef signed __int16 int16_t;
typedef unsigned __int32 uint32_t;
typedef signed __int32 int32_t;
typedef unsigned __int64 uint64_t;
typedef signed __int64 int64_t;
#else
#include <stdint.h>
#endif

#endif /* #ifndef INTTYPES_H */

/* end of file */
