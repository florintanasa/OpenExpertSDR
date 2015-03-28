/*
 * This file is part of ExpertSDR
 *
 * ExpertSDR is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * ExpertSDR is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 *
 *
 * Copyright (C) 2012 Valery Mikhaylovsky
 * The authors can be reached by email at maksimus1210@gmail.com
 */

#ifndef RINGRESAMPLER_H
#define RINGRESAMPLER_H

#include <QtWidgets>
#include <QMutex>

#include "RingBuffer.h"
#include "../Resampler.h"
#include "../Defines.h"

class RingResampler
{
	QMutex mutex;
	QMutex mutexPaToVac;
	QMutex mutexVacToPa;

	int buffSize;
	RingBuffer *pDataInputL , *pDataInputR ;
	RingBuffer *pDataOutputL, *pDataOutputR;

	float *pInDataL , *pInDataR ;
	float *pOutDataL, *pOutDataR;

	float inGainValue;
	float outGainValue;

	bool  isOpened;

	Resampler *resPaToVac;
	Resampler *resVacToPa;

	float paSampleRate;
	float vacSampleRate;

public:
	RingResampler();
	~RingResampler();

	bool open(int inSampleRate, int outSampleRate);
	void close();

	bool isOpen();

	bool setData(float *pBufL, float *pBufR, int size);
	bool getData(float *pBufL, float *pBufR, int size);

	void setInGain(float dB);
	void setOutGain(float dB);

	float inGain();
	float outGain();

	void paProcess(const void *In, void *Out, unsigned long FrameCount, void *UserData);	// функция обратного вызова
};

#endif // RINGRESAMPLER_H
