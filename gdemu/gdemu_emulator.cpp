/**
 * EmulatorClass
 * $Id$
 * \file gdemu_emulator.cpp
 * \brief EmulatorClass
 * \date 2011-05-29 19:54GMT
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
#include "gdemu_emulator.h"

// System includes
#include <istream>
#include <ostream>
#include <iostream>

// Project includes
#include "WProgram.h"
#include "GD.h"
#include "gdemu_system.h"
#include "gdemu_graphics_driver.h"
#include "gdemu_graphics_machine.h"
#include "gdemu_j1.h"
#include "gdemu_audio_driver.h"
#include "gdemu_audio_machine.h"
#include "gdemu_gameduino_spi.h"
#include "gdemu_keyboard.h"
#include "omp.h"

// using namespace ...;

namespace GDEMU {

EmulatorClass Emulator;

void EmulatorClass::run(void (*setup)(), void (*loop)(), int flags)
{
	System.begin();
	GameduinoSPI.begin();
	GraphicsDriver.begin();
	if (flags & EmulatorEnableAudio) AudioDriver.begin();
	if (flags & EmulatorEnableJ1) J1.begin();
	if (flags & EmulatorEnableKeyboard) Keyboard.begin();

	#pragma omp parallel num_threads(3)
	{
		// graphics
		#pragma omp master 
		{
			System.makeMainThread();

			unsigned long taskId = 0;
			void *taskHandle;
			taskHandle = System.setThreadGamesCategory(&taskId);
			System.disableAutomaticPriorityBoost();
			System.makeRealtimePriorityThread();

			double targetSeconds = System.getSeconds();
			double deltaSeconds = 1.0 / 72.0;

			for (;;)
			{
				System.makeRealtimePriorityThread();

				System.update();
				targetSeconds += deltaSeconds;
				
				J1.flagReset(GameduinoSPI.getJ1Reset() == 0x01);
				
				// Render lines, the j1 is run by the graphics machine inbetween lines to allow chasing them
				{
					GameduinoSPI.setVBlank(0);
					// System.delay(0);
					System.switchThread();

					unsigned long procStart = System.getMicros();
					GraphicsMachine.process();
					unsigned long procDelta = System.getMicros() - procStart;

					if (procDelta > 8000)
						printf("process: %i micros (%i ms)\r\n", (int)procDelta, (int)procDelta / 1000);
				}
				
				// Flip buffer and also give a slice of time to the duino main thread
				{
					GameduinoSPI.setVBlank(1);
					System.prioritizeDuinoThread();

					unsigned long flipStart = System.getMicros();
					GraphicsMachine.flip();
					unsigned long flipDelta = System.getMicros() - flipStart;

					if (flipDelta > 8000)
						printf("flip: %i micros (%i ms)\r\n", (int)flipDelta, (int)flipDelta / 1000);
					
					//System.delay(2); // ensure slice of time to duino thread at cost of j1 cycles
					System.switchThread();
					System.unprioritizeDuinoThread();
				}
				
				//long currentMillis = millis();
				//long millisToWait = targetMillis - currentMillis;
				double currentSeconds = System.getSeconds();
				double secondsToWait = targetSeconds - currentSeconds;
				//if (millisToWait < -100) targetMillis = millis();
				if (secondsToWait < -0.25) // Skip freeze
				{
					//printf("skip freeze\n");
					targetSeconds = System.getSeconds();
				}

				//printf("millis to wait: %i", (int)millisToWait);

				if (secondsToWait > 0.0)
				{
					if (J1.isResetting())
					{
						System.delay((int)(secondsToWait * 1000.0));
					}
					else // run the j1 for the remaining time
					{
						System.makeNormalPriorityThread();

						J1.execute(0, (int)(secondsToWait * 1000000.0));
					}
				}
				
				System.holdDuinoThread(); // don't let the other thread hog cpu
				System.resumeDuinoThread();
				
				//fixRounding = -2 - fixRounding;
				// else delay(6);
			}
			System.revertThreadCategory(taskHandle);
		}

		// arduino
		if (omp_get_thread_num() == 1)
		{
			System.makeDuinoThread();
			
			unsigned long taskId = 0;
			void *taskHandle;
			taskHandle = System.setThreadGamesCategory(&taskId);
			System.disableAutomaticPriorityBoost();
			System.makeNormalPriorityThread();
			setup();
			for (;;) 
			{
				loop();
			}
			System.revertThreadCategory(taskHandle);
		}

		// sound
		if (omp_get_thread_num() == 2)
		{
			unsigned long taskId = 0;
			void *taskHandle;
			taskHandle = System.setThreadGamesCategory(&taskId);
			System.disableAutomaticPriorityBoost();
			System.makeHighPriorityThread();
			for (;;)
			{
				AudioMachine.process();
				if (flags & EmulatorEnableKeyboard) Keyboard.update();
				System.delay(10);
			}
			System.revertThreadCategory(taskHandle);
		}
	}
	
	if (flags & EmulatorEnableKeyboard) Keyboard.end();
	if (flags & EmulatorEnableJ1) J1.end();
	if (flags & EmulatorEnableAudio) AudioDriver.end();
	GraphicsDriver.end();
	GameduinoSPI.end();
	System.end();
}

} /* namespace GDEMU */

/* end of file */
