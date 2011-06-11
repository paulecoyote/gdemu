/**
 * AudioMachineClass
 * $Id$
 * \file gdemu_audio_machine.cpp
 * \brief AudioMachineClass
 * \date 2011-05-29 19:43GMT
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
#include "gdemu_audio_machine.h"

// System includes
#include "WProgram.h"

// Project includes
#include "gdemu_gameduino_spi.h"
#include "gdemu_audio_driver.h"
#include "GD.h"

// using namespace ...;

namespace GDEMU {

AudioMachineClass AudioMachine;

#define D_GDEMU_AUDIOAMP 64 /*1 /*16 /**/
#define D_GDEMU_AUDIOFP double

static D_GDEMU_AUDIOFP s_AudioRad[64] = { 0 };

// for cheap nearest resampling because proper resampling is not implemented in the wasapi driver
static double s_SecondsPassedFor8000 = 1.0 / 8000.0;
static const double s_SecondsPerSample8000 = 1.0 / 8000.0;
static short s_SampleL = 0;
static short s_SampleR = 0;

void AudioMachineClass::process()
{
	unsigned char *gameduinoRam = GameduinoSPI.getRam();

	int audioFrequency = AudioDriver.getFrequency();
	double secondsPerSample = 1.0 / (double)audioFrequency;

	short *audioBuffer;
	int samples;

	AudioDriver.beginBuffer(&audioBuffer, &samples);

	if (audioBuffer != NULL)
	{
		// sample (with quick n dirty resampling with horrible ringing) :)
		for (int s = 0; s < samples; ++s)
		{
			if (s_SecondsPassedFor8000 >= s_SecondsPerSample8000)
			{
				s_SecondsPassedFor8000 -= s_SecondsPerSample8000;
				/*unsigned short sampleL = GameduinoSPI.getNextSampleL();
				unsigned short sampleR = GameduinoSPI.getNextSampleR();
				if (sampleL == 0) sampleL = 0x8000; // because!
				if (sampleR == 0) sampleR = 0x8000;
				s_SampleL = (short)((int)sampleL - 0x8000);
				s_SampleR = (short)((int)sampleR - 0x8000);*/
				s_SampleL = GameduinoSPI.getNextSampleL();
				s_SampleR = GameduinoSPI.getNextSampleR();
			}
			audioBuffer[s * 2] = s_SampleL;
			audioBuffer[(s * 2) + 1] = s_SampleR;
			s_SecondsPassedFor8000 += secondsPerSample;
		}
		// voices
		for (int i = 0; i < 64; ++i)
		{
			int voiceIdx = VOICES + (4 * i);

			int frequency;
			bool whiteNoise;
			int leftAmpI;
			int rightAmpI;
			D_GDEMU_AUDIOFP leftAmp;
			D_GDEMU_AUDIOFP rightAmp;
			
			frequency = gameduinoRam[voiceIdx];
			frequency += (gameduinoRam[voiceIdx + 1] & 0x7F) << 8;

			if (frequency)
			{
				whiteNoise = (gameduinoRam[voiceIdx + 1] & 0x80) == 0x80;
				leftAmpI = gameduinoRam[voiceIdx + 2];
				leftAmpI *= D_GDEMU_AUDIOAMP;
				leftAmp = (D_GDEMU_AUDIOFP)leftAmpI;
				rightAmpI = gameduinoRam[voiceIdx + 3];
				rightAmpI *= D_GDEMU_AUDIOAMP;
				rightAmp = (D_GDEMU_AUDIOFP)rightAmpI;

				if (leftAmpI || rightAmpI)
				{
					if (whiteNoise)
					{
						const int randSub = RAND_MAX * 6;
						const int ampDiv = RAND_MAX * 6;
						for (int s = 0; s < samples; ++s)
						{
							// QUESTION: DOES THE WHITE NOISE TAKE FREQUENCY INTO ACCOUNT?
							int noiseValue = rand() + rand() + rand() + rand() + rand() + rand() + rand() + rand() + rand() + rand() + rand() + rand() - randSub;
							int noiseValueLeft = noiseValue * leftAmpI;
							int noiseValueRight = noiseValue * rightAmpI;
							noiseValueLeft /= ampDiv;
							noiseValueRight /= ampDiv;
							audioBuffer[s * 2] += (short)(noiseValueLeft);
							audioBuffer[(s * 2) + 1] += (short)(noiseValueRight);
						}
					}
					else
					{
						D_GDEMU_AUDIOFP addRad = (D_GDEMU_AUDIOFP)PI / (D_GDEMU_AUDIOFP)audioFrequency * (D_GDEMU_AUDIOFP)0.5 * (D_GDEMU_AUDIOFP)frequency;
						D_GDEMU_AUDIOFP audRad = s_AudioRad[i];

						for (int s = 0; s < samples; ++s)
						{
							D_GDEMU_AUDIOFP sinValue = sin(audRad);
							audioBuffer[s * 2] += (short)(sinValue * leftAmp);
							audioBuffer[(s * 2) + 1] += (short)(sinValue * rightAmp);

							/*for (int i = 0; i < (int)((1.0 + sinValue) * 64); ++i) putchar(' ');
							putchar('|');
							putchar('\n');*/
							
							audRad += addRad;
							while (audRad > (PI * 2)) audRad -= (PI * 2);
						}

						s_AudioRad[i] = audRad;
					}
				}
			}
		}

		AudioDriver.endBuffer();
	}
}

} /* namespace GDEMU */

/* end of file */
