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


#include <QtGlobal>

#include "Resampler.h"
#include <cstring>

float buffI[4096];
float buffQ[4096];
float ph = 0;

Resampler::Resampler()
{
	InSampleRate  = 0.0f;
	OutSampleRate = 0.0f;
	IsInitialized = false;
	Mode = 0.0f;
	Param.Strategy = NO_RESAMPLE;
	Param.Stap = 0.0f;
	Param.NumPoints = 0;
	timeResPosition = -2.0f;

	DataI = 0;
	DataQ = 0;

	isStart = false;
}

void Resampler::reset()
{
	isStart = false;
	timeResPosition = -2;
}

bool Resampler::initialize(float inSampleRate, float outSampleRate)
{
	Mode = (long double)inSampleRate/(long double)outSampleRate;
	if(Mode > 1.0f)
	{
		if((Mode - ((int)Mode)) == 0.0f)
			Param.Strategy = DECIMATE_1;
		else
			Param.Strategy = DECIMATE_2;
	}
	else if(Mode < 1.0f)
	{
		float tmp = outSampleRate / inSampleRate;
		if((tmp - ((int)tmp)) == 0.0f)
			Param.Strategy = INTERPOLATE_1;
		else
			Param.Strategy = INTERPOLATE_2;
	}
	else
		Param.Strategy = NO_RESAMPLE;

	int Kp = 1;
	switch(Param.Strategy)
	{
	case DECIMATE_1:
		Param.NumPoints = ceil(Mode);
		Filter.initialize(Kp, 60, outSampleRate/2 - 1000, outSampleRate/2 - 100, inSampleRate);
		break;
	case DECIMATE_2:
		Filter.initialize(Kp, 60, outSampleRate/2 - 1000, outSampleRate/2 - 100, inSampleRate);
		Param.Stap = Mode;
		break;
	case INTERPOLATE_1:
		Param.NumPoints = ceil(1.0f/Mode) - 1;
		Kp = Param.NumPoints + 1;
		Filter.initialize(Kp, 60, inSampleRate/2 - 1000, inSampleRate/2 - 100, outSampleRate);
		break;
	case INTERPOLATE_2:
		Filter.initialize(Kp, 60, inSampleRate/2 - 1000, inSampleRate/2 - 100, outSampleRate);
		Param.Stap = Mode;
		break;
	default:
		break;
	}
	timeResPosition = -2.0f;
	return (true);
}

bool Resampler::process(float *pSrcI, float *pSrcQ, int size, float *pDstI, float *pDstQ, int &outSize)
{
	if((pSrcI == 0) || (pSrcQ == 0) || (pDstI == 0) || (pDstQ == 0) || (size <= 0))
	{
		qWarning() << "Resampler: process(): pSrcI=" << pSrcI << ", pSrcQ=" << pSrcQ << ", pDstI=" << pDstI << ", pDstQ=" << pDstI << ", size=" << size;
		return (false);
	}
	switch(Param.Strategy)
	{
	case DECIMATE_1:
	case DECIMATE_2:
		Filter.process(pSrcI, pSrcQ, size);
		resample(pSrcI, pSrcQ, size, pDstI, pDstQ, outSize);
		break;
	case INTERPOLATE_1:
	case INTERPOLATE_2:
		resample(pSrcI, pSrcQ, size, pDstI, pDstQ, outSize);
		Filter.process(pDstI, pDstQ, outSize);
		break;
	default:
		memcpy(pDstI, pSrcI, size*sizeof(float));
		memcpy(pDstQ, pSrcQ, size*sizeof(float));
		outSize = size;
		break;
	}
	return (true);
}

bool Resampler::resample(float *pSrcI, float *pSrcQ, int size, float *pDstI, float *pDstQ, int &outSize)
{
	int i = 0, j = 0, k = 0;
	float a0I, a1I, a2I, a3I;
	float a0Q, a1Q, a2Q, a3Q;
	long double tmp = timeResPosition;

	switch(Param.Strategy)
	{
	case DECIMATE_1:
		for(i = 0, j = 0; i < size; i += Param.NumPoints, j++)
		{
			pDstI[j] = pSrcI[i];
			pDstQ[j] = pSrcQ[i];
		}
		outSize = j;
		break;
	case DECIMATE_2:
		j = 0;
		tmp = timeResPosition;

		if(isStart)
			i = 2;
		else
			i = 3;

		for(; i < size;)
		{
			if(i == 2)
			{
				a3I = (pSrcI[i] - DataI)/6.0f + 0.5f*(pSrcI[i-2] - pSrcI[i-1]);
				a1I = 0.5*(pSrcI[i] - pSrcI[i-2]) - a3I;
				a2I = pSrcI[i] - pSrcI[i-1] - a1I - a3I;
				a0I = pSrcI[i-1];

				a3Q = (pSrcQ[i] - DataQ)/6.0f + 0.5f*(pSrcQ[i-2] - pSrcQ[i-1]);
				a1Q = 0.5*(pSrcQ[i] - pSrcQ[i-2]) - a3Q;
				a2Q = pSrcQ[i] - pSrcQ[i-1] - a1Q - a3Q;
				a0Q = pSrcQ[i-1];
			}
			else
			{
				a3I = (pSrcI[i] - pSrcI[i-3])/6.0f + 0.5f*(pSrcI[i-2] - pSrcI[i-1]);
				a1I = 0.5*(pSrcI[i] - pSrcI[i-2]) - a3I;
				a2I = pSrcI[i] - pSrcI[i-1] - a1I - a3I;
				a0I = pSrcI[i-1];

				a3Q = (pSrcQ[i] - pSrcQ[i-3])/6.0f + 0.5f*(pSrcQ[i-2] - pSrcQ[i-1]);
				a1Q = 0.5*(pSrcQ[i] - pSrcQ[i-2]) - a3Q;
				a2Q = pSrcQ[i] - pSrcQ[i-1] - a1Q - a3Q;
				a0Q = pSrcQ[i-1];
			}
			for(;tmp <= 1.0; tmp += Param.Stap, j++)
			{
				pDstI[j] = tmp*(tmp*(tmp*a3I + a2I) + a1I) + a0I;
				pDstQ[j] = tmp*(tmp*(tmp*a3Q + a2Q) + a1Q) + a0Q;
			}
			if(i < (size - 3))
			{
				i += 3;
				tmp -= 3.0;
			}
			else if(i < (size-2))
			{
				i += 2;
				tmp -= 2.0;
			}
			else if(i < (size-1))
			{
				i++;
				tmp -= 1.0;
			}
			else
				break;
		}
		outSize = j;
		timeResPosition = (1.0 - (j/Param.Stap - ((int)(j/Param.Stap))))*Param.Stap - 2.0;
		isStart = true;
		DataI = pSrcI[size-1];
		DataQ = pSrcQ[size-1];
		break;
	case INTERPOLATE_1:
		for(i = 0, j = 0; i < size; i ++)
		{
			pDstI[j] = pSrcI[i];
			pDstQ[j] = pSrcQ[i];
			j++;
			for(k = 0; k < Param.NumPoints; k++)
			{
				pDstI[j] = 0.0f;
				pDstQ[j] = 0.0f;
				j++;
			}
		}
		outSize = j;
		break;
	case INTERPOLATE_2:
		j = 0;
		tmp = timeResPosition;
		if(isStart)
			i = 2;
		else
			i = 3;

		for(; i < size;)
		{
			if(i == 2)
			{
				a3I = (pSrcI[i] - DataI)/6.0f + 0.5f*(pSrcI[i-2] - pSrcI[i-1]);
				a1I = 0.5*(pSrcI[i] - pSrcI[i-2]) - a3I;
				a2I = pSrcI[i] - pSrcI[i-1] - a1I - a3I;
				a0I = pSrcI[i-1];

				a3Q = (pSrcQ[i] - DataQ)/6.0f + 0.5f*(pSrcQ[i-2] - pSrcQ[i-1]);
				a1Q = 0.5*(pSrcQ[i] - pSrcQ[i-2]) - a3Q;
				a2Q = pSrcQ[i] - pSrcQ[i-1] - a1Q - a3Q;
				a0Q = pSrcQ[i-1];
			}
			else
			{
				a3I = (pSrcI[i] - pSrcI[i-3])/6.0f + 0.5f*(pSrcI[i-2] - pSrcI[i-1]);
				a1I = 0.5*(pSrcI[i] - pSrcI[i-2]) - a3I;
				a2I = pSrcI[i] - pSrcI[i-1] - a1I - a3I;
				a0I = pSrcI[i-1];

				a3Q = (pSrcQ[i] - pSrcQ[i-3])/6.0f + 0.5f*(pSrcQ[i-2] - pSrcQ[i-1]);
				a1Q = 0.5*(pSrcQ[i] - pSrcQ[i-2]) - a3Q;
				a2Q = pSrcQ[i] - pSrcQ[i-1] - a1Q - a3Q;
				a0Q = pSrcQ[i-1];
			}
			for(;tmp <= 1.0; tmp += Param.Stap, j++)
			{
				pDstI[j] = tmp*(tmp*(tmp*a3I + a2I) + a1I) + a0I;
				pDstQ[j] = tmp*(tmp*(tmp*a3Q + a2Q) + a1Q) + a0Q;
			}
			if(i < (size - 3))
			{
				i += 3;
				tmp -= 3.0;
			}
			else if(i < (size-2))
			{
				i += 2;
				tmp -= 2.0;
			}
			else if(i < (size-1))
			{
				i++;
				tmp -= 1.0;
			}
			else
				break;
		}
		outSize = j;
		timeResPosition = (1.0 - (j/Param.Stap - ((int)(j/Param.Stap))))*Param.Stap - 2.0;
		isStart = true;
		DataI = pSrcI[size-1];
		DataQ = pSrcQ[size-1];
		break;
	}
	return (true);
}


FirLP::FirLP()
{
	isOk = false;
	Taps = 3;
	State = 0;
}

bool FirLP::initialize(float Scale, float Astop, float Fpass, float Fstop, float samplerate)
{
	if(samplerate < 1000)
		return (false);

	if((Fstop > samplerate/2) || (Fpass > samplerate/2) || (Fstop < Fpass))
		return (false);

	//
	int n;
	float Beta;
	mutex.lock();
	SampleRate = samplerate;
	float normFpass = Fpass/SampleRate;
	float normFstop = Fstop/SampleRate;
	float normFcut = (normFstop + normFpass);

	if(Astop < 20.96)
		Beta = 0;
	else if(Astop >= 50.0)
		Beta = 0.1102 * (Astop - 8.71);
	else
		Beta = 0.5842*powf((Astop-20.96), 0.4) + 0.07886*(Astop - 20.96);

	Taps = (Astop - 8.0) / (2.285*PI*(normFstop - normFpass)) + 1;

	if(Taps > FIR_MAX_TAPS)
		Taps = FIR_MAX_TAPS;
	if(Taps < 3)
		Taps = 3;

	float fCenter = 0.5*(float)(Taps-1);
	float izb = Izero(Beta);
	float c, x;
	for(n = 0; n < Taps; n++)
	{
		x = (float)n - fCenter;
		if((float)n == fCenter)
			c = 2.0*normFcut;
		else
			c = (float)sinf(PI*x*normFcut)/(PI*x);

		x = ((float)n - ((float)Taps-1.0)/2.0 ) / (((float)Taps-1.0)/2.0);
		Coef[n] = Scale*c*Izero(Beta*sqrtf(1 - (x*x))) / izb;
	}

	for (n = 0; n < Taps; n++)
		Coef[n+Taps] = Coef[n];

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    std::memcpy(ICoef, Coef, sizeof(float)*Taps*2);
    std::memcpy(QCoef, Coef, sizeof(float)*Taps*2);
    std::memset(rZBuf, 0, sizeof(float)*Taps);
    std::memset(cZBuf, 0, sizeof(COMPLEX)*Taps);
#else
    qMemCopy(ICoef, Coef, sizeof(float)*Taps*2);
    qMemCopy(QCoef, Coef, sizeof(float)*Taps*2);
    qMemSet(rZBuf, 0, sizeof(float)*Taps);
    qMemSet(cZBuf, 0, sizeof(COMPLEX)*Taps);
#endif

    isOk = true;
	mutex.unlock();
	return (true);
}

void FirLP::process(float *pSrcDstL, float *pSrcDstR, int size)
{
	if(!isOk)
		return;

	COMPLEX acc;
	COMPLEX* Zptr;
	float* HIptr;
	float* HQptr;

	mutex.lock();
	for(int i = 0; i < size; i++)
	{
		cZBuf[State].re = pSrcDstL[i];
		cZBuf[State].im = pSrcDstR[i];

		HIptr = ICoef + Taps - State;
		HQptr = QCoef + Taps - State;
		Zptr = cZBuf;
		acc.re = (*HIptr++ * (*Zptr).re);
		acc.im = (*HQptr++ * (*Zptr++).im);
		for(int j=1; j < Taps; j++)
		{
			acc.re += (*HIptr++ * (*Zptr).re);
			acc.im += (*HQptr++ * (*Zptr++).im);
		}
		if(--State < 0)
			State += Taps;

		pSrcDstL[i] = acc.re;
		pSrcDstR[i] = acc.im;
	}
	mutex.unlock();
}

float FirLP::Izero(float x)
{
	float x2 = x/2.0;
	float sum = 1.0;
	float ds = 1.0;
	float di = 1.0;
	float errorlimit = 1e-9;
	float tmp;
	do
	{
		tmp = x2/di;
		tmp *= tmp;
		ds *= tmp;
		sum += ds;
		di += 1.0;
	}while(ds >= errorlimit*sum);
	return(sum);
}
