/**
 * AudioDriverClass
 * $Id$
 * \file gdemu_audio_driver_wasapi.cpp
 * \brief AudioDriverClass
 * \date 2011-05-29 19:38GMT
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
#include "gdemu_audio_driver.h"

// System includes
#include "gdemu_system_windows.h"

// Project includes

// using namespace ...;

namespace GDEMU {

AudioDriverClass AudioDriver;

#define REFTIMES_PER_SEC  10000000
#define REFTIMES_PER_MILLISEC  10000

static IMMDeviceEnumerator *s_MMDeviceEnumerator = NULL;
static IMMDevice *s_MMDevice = NULL;
static IAudioClient *s_AudioClient = NULL;
static IAudioRenderClient *s_AudioRenderClient = NULL;

static int s_AudioFrequency = 0;
static unsigned int s_BufferFrameCount = 0;
static unsigned int s_NumFramesAvailable = 0;

static double s_TestRad;

#define D_GDEMU_AUDIOCHANNELS 2
#define D_GDEMU_AUDIOBITS 16

void AudioDriverClass::begin()
{
	const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
	const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
	const IID IID_IAudioClient = __uuidof(IAudioClient);
	const IID IID_IAudioRenderClient = __uuidof(IAudioRenderClient);

	HRESULT hr;

	hr = CoCreateInstance(CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, IID_IMMDeviceEnumerator, (void**)&s_MMDeviceEnumerator);
	if (hr) SystemWindows.ErrorHResult(hr);

	hr = s_MMDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &s_MMDevice);
	if (hr) SystemWindows.ErrorHResult(hr);

	hr = s_MMDevice->Activate(IID_IAudioClient, CLSCTX_ALL, NULL, (void**)&s_AudioClient);
	if (hr) SystemWindows.ErrorHResult(hr);

	WAVEFORMATEX *pwfx;
	hr = s_AudioClient->GetMixFormat(&pwfx);
	s_AudioFrequency = pwfx->nSamplesPerSec;

	WAVEFORMATEX wfx;
	wfx.wFormatTag = 0x0001;
	wfx.nChannels = D_GDEMU_AUDIOCHANNELS;
	wfx.nSamplesPerSec = pwfx->nSamplesPerSec;
	wfx.nBlockAlign = D_GDEMU_AUDIOCHANNELS * D_GDEMU_AUDIOBITS / 8;
	wfx.nAvgBytesPerSec = pwfx->nSamplesPerSec * wfx.nBlockAlign;
	wfx.wBitsPerSample = D_GDEMU_AUDIOBITS;
	wfx.cbSize = 0;

	long hnsRequestedDuration = REFTIMES_PER_SEC / 10;
	hr = s_AudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, hnsRequestedDuration, 0, &wfx, NULL);
	if (hr) SystemWindows.ErrorHResult(hr);
	
	hr = s_AudioClient->GetBufferSize(&s_BufferFrameCount);
	if (hr) SystemWindows.ErrorHResult(hr);

	hr = s_AudioClient->GetService(IID_IAudioRenderClient, (void **)&s_AudioRenderClient);
	if (hr) SystemWindows.ErrorHResult(hr);
	
	hr = s_AudioClient->Start();
	if (hr) SystemWindows.ErrorHResult(hr);

	// done
}

bool AudioDriverClass::update()
{
	// sin test

	/*short *audioData;

	int hr; 
	unsigned int numFramesAvailable;
	unsigned int numFramesPadding;

	// test
	hr = s_AudioClient->GetCurrentPadding(&numFramesPadding);
	if (hr) SystemWindows.ErrorHResult(hr);

	numFramesAvailable = s_BufferFrameCount - numFramesPadding;
	hr = s_AudioRenderClient->GetBuffer(numFramesAvailable, (BYTE **)&audioData);
	if (hr) SystemWindows.ErrorHResult(hr);

	double addRad = 3.14159 / (double)s_AudioFrequency * 2.0 * 440.0;
	// fill
	for (unsigned int i = 0; i < numFramesAvailable; ++i)
	{
		s_TestRad += addRad;
		audioData[i * 2] = (short)((sin(s_TestRad)) * 5000);
		audioData[(i * 2) + 1] = audioData[i * 2];
	}

	s_AudioRenderClient->ReleaseBuffer(numFramesAvailable, 0);
	if (hr) SystemWindows.ErrorHResult(hr);*/

	return true;
}

int AudioDriverClass::getFrequency()
{
	return s_AudioFrequency;
}

void AudioDriverClass::beginBuffer(short **buffer, int *samples)
{
	HRESULT hr;

	unsigned int numFramesPadding;

	hr = s_AudioClient->GetCurrentPadding(&numFramesPadding);
	if (hr) SystemWindows.ErrorHResult(hr);

	s_NumFramesAvailable = s_BufferFrameCount - numFramesPadding;
	*samples = s_NumFramesAvailable;

	if (s_NumFramesAvailable == 0)
	{
		buffer = NULL;
	}
	else
	{
		hr = s_AudioRenderClient->GetBuffer(s_NumFramesAvailable, (BYTE **)buffer);
		if (hr) SystemWindows.ErrorHResult(hr);
	}
}

void AudioDriverClass::endBuffer()
{
	HRESULT hr;

	hr = s_AudioRenderClient->ReleaseBuffer(s_NumFramesAvailable, 0);
	if (hr) SystemWindows.ErrorHResult(hr);
}

void AudioDriverClass::end()
{
	// todo*/***
}

} /* namespace GDEMU */

/* end of file */
