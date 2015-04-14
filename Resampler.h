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

#ifndef RESAMPLER_H
#define RESAMPLER_H

#include <QtGlobal>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "Defines.h"
#include "DttSP.h"

#define FIR_MAX_TAPS 128

typedef enum
{
	DECIMATE_1 = 0,
	DECIMATE_2,
	NO_RESAMPLE,
	INTERPOLATE_1,
	INTERPOLATE_2
}RESAMPLE_STRATEGY;

typedef struct
{
	RESAMPLE_STRATEGY Strategy;

	int NumPoints;
	long double Stap;
}RESAMPLE_PARAM;

class FirLP
{
	bool isOk;
	float SampleRate;
	int Taps;
	int State;
	float Coef[FIR_MAX_TAPS*2];
	float ICoef[FIR_MAX_TAPS*2];
	float QCoef[FIR_MAX_TAPS*2];
	float rZBuf[FIR_MAX_TAPS];
	COMPLEX cZBuf[FIR_MAX_TAPS];
	QMutex mutex;

public:
	FirLP();

	bool initialize(float Scale, float Astop, float Fpass, float Fstop, float samplerate);
	void process(float *pSrcDstL, float *pSrcDstR, int size);

private:
	float Izero(float x);
};

class Resampler
{
	bool IsInitialized;
	float InSampleRate;
	float OutSampleRate;
	long double Mode;
	RESAMPLE_PARAM Param;
	FirLP Filter;
	FirLP Filter2;
	float timeResPosition;
	float DataI, DataQ;
	bool isStart;

public:
	Resampler();

	void reset();
	bool initialize(float inSampleRate, float outSampleRate);	// функция инициализации
	bool process(float *pSrcI, float *pSrcQ, int size, float *pDstI, float *pDstQ, int &outSize);

private:
	bool resample(float *pSrcI, float *pSrcQ, int size, float *pDstI, float *pDstQ, int &outSize);
};

#endif // RESAMPLER_H
