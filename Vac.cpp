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
#include "Vac.h"
#include <math.h>


Vac::Vac()
{
	pRbInL  = 0;
	pRbInR  = 0;
	pRbOutL = 0;
	pRbOutR = 0;
	pResBufInL  = 0;
	pResBufInR  = 0;
	pResBufOutL = 0;
	pResBufOutR = 0;
	isStereo = true;
	pBufInL = new float[4096];
	pBufInR = new float[4096];
	inputGain = 1.0;
	outputGain = 1.0;
	isOpen = false;
	Size = 0;
}

Vac::~Vac()
{
	if(isOpen)
		isOpen = false;

	if(pRbInL != 0)
		delete pRbInL;
	if(pRbInR != 0)
		delete pRbInR;
	if(pRbOutL != 0)
		delete pRbOutL;
	if(pRbOutR != 0)
		delete pRbOutR;

	if(pResBufInL != 0)
		delete[] pResBufInL;
	if(pResBufInR != 0)
		delete[] pResBufInR;
	if(pResBufOutL != 0)
		delete[] pResBufOutL;
	if(pResBufOutR != 0)
		delete[] pResBufOutR;

	delete[] pBufInL;
	delete[] pBufInR;
}

void Vac::open(float inputSampleRate, float outputSampleRate)
{
	SampleRatePa  = outputSampleRate;
	SampleRateVac = inputSampleRate;

	if(SampleRateVac < SampleRatePa)
		Size = qPow(2, 12 + ((int)(qLn(SampleRatePa/SampleRateVac)/qLn(2.0f)) + 2));
	else if(SampleRateVac == SampleRatePa)
		Size = 4096*2;
	else
		Size = qPow(2, 12 + ((int)(qLn(SampleRateVac/SampleRatePa)/qLn(2.0f)) + 2));

	if(pRbInL == 0)
		pRbInL  = new ringBuf(Size);
	else
	{
		delete pRbInL;
		pRbInL  = new ringBuf(Size);
	}
	if(pRbInR == 0)
		pRbInR  = new ringBuf(Size);
	else
	{
		delete pRbInR;
		pRbInR  = new ringBuf(Size);
	}
	if(pRbOutL == 0)
		pRbOutL  = new ringBuf(Size);
	else
	{
		delete pRbOutL;
		pRbOutL  = new ringBuf(Size);
	}
	if(pRbOutR == 0)
		pRbOutR  = new ringBuf(Size);
	else
	{
		delete pRbOutR;
		pRbOutR  = new ringBuf(Size);
	}

	if(pResBufInL == 0)
		pResBufInL  = new float[Size];
	else
	{
		delete[] pResBufInL;
		pResBufInL  = new float[Size];
	}
	if(pResBufInR == 0)
		pResBufInR  = new float[Size];
	else
	{
		delete[] pResBufInR;
		pResBufInR  = new float[Size];
	}
	if(pResBufOutL == 0)
		pResBufOutL  = new float[Size];
	else
	{
		delete[] pResBufOutL;
		pResBufOutL  = new float[Size];
	}
	if(pResBufOutR == 0)
		pResBufOutR  = new float[Size];
	else
	{
		delete[] pResBufOutR;
		pResBufOutR  = new float[Size];
	}

	reset();
	resReset();
	resPaToVac.initialize(SampleRatePa, SampleRateVac);
	resVacToPa.initialize(SampleRateVac, SampleRatePa);
	isOpen = true;
}

void Vac::open(float inputSampleRate, int inBuffSize, float outputSampleRate, int outBuffSize)
{
	SampleRatePa  = outputSampleRate;
	SampleRateVac = inputSampleRate;
	int x1 = inBuffSize/inputSampleRate;
	int x2 = outBuffSize/outputSampleRate;
	if(x1 > x2)
		Size = 6*inBuffSize;
	else
		Size = 6*outBuffSize;

	if(pRbInL == 0)
		pRbInL  = new ringBuf(Size);
	else
	{
		delete pRbInL;
		pRbInL  = new ringBuf(Size);
	}
	if(pRbInR == 0)
		pRbInR  = new ringBuf(Size);
	else
	{
		delete pRbInR;
		pRbInR  = new ringBuf(Size);
	}
	if(pRbOutL == 0)
		pRbOutL  = new ringBuf(Size);
	else
	{
		delete pRbOutL;
		pRbOutL  = new ringBuf(Size);
	}
	if(pRbOutR == 0)
		pRbOutR  = new ringBuf(Size);
	else
	{
		delete pRbOutR;
		pRbOutR  = new ringBuf(Size);
	}

	if(pResBufInL == 0)
		pResBufInL  = new float[Size];
	else
	{
		delete[] pResBufInL;
		pResBufInL  = new float[Size];
	}
	if(pResBufInR == 0)
		pResBufInR  = new float[Size];
	else
	{
		delete[] pResBufInR;
		pResBufInR  = new float[Size];
	}
	if(pResBufOutL == 0)
		pResBufOutL  = new float[Size];
	else
	{
		delete[] pResBufOutL;
		pResBufOutL  = new float[Size];
	}
	if(pResBufOutR == 0)
		pResBufOutR  = new float[Size];
	else
	{
		delete[] pResBufOutR;
		pResBufOutR  = new float[Size];
	}

	reset();
	resReset();

	resPaToVac.initialize(SampleRatePa, SampleRateVac);
	resVacToPa.initialize(SampleRateVac, SampleRatePa);
	isOpen = true;
}

void Vac::close()
{
	isOpen = false;
}

bool Vac::writeBuf(float *pOutL, float *pOutR, int size)
{
    if(!isOpen)
        return (false);

    int outSize = 0;
    resPaToVac.process(pOutL, pOutR, size, pResBufOutL, pResBufOutR, outSize);
	if(!updMut.tryLock(200))
	{
		qWarning() << "Vac: writeBuf(): Can't lock mutex in 200 ms!";
		return false;
	}

	if(!isOpen)
	{
		updMut.unlock();
		qWarning() << "Vac: writeBuf(): isOpen = " << isOpen;
		return false;
	}

	if((pRbOutL->writeCount() >= outSize) && (pRbOutR->writeCount() >= outSize))
	{
		for(int i = 0; i < size; i++)
		{
			pResBufOutL[i] *= outputGain;
			pResBufOutR[i] *= outputGain;
		}
		pRbOutL->write(pResBufOutL, outSize);
		pRbOutR->write(pResBufOutR, outSize);
	}
	updMut.unlock();
    return (true);
}

bool Vac::readBuf(float *pInL, float *pInR, int size)
{
    if(!isOpen)
        return (false);

	if(!updMut.tryLock(200))
	{
		qWarning() << "Vac: readBuf(): Can't lock mutex in 200 ms!";
		return (false);
	}

	if((pRbInL->readCount() >= size) && (pRbInR->readCount() >= size))
	{
        pRbInL->read(pInL, size);
        pRbInR->read(pInR, size);
        for(int i = 0; i < size; i++)
        {
        	pInL[i] *= inputGain;
        	pInR[i] *= inputGain;
        }
        updMut.unlock();
		return (true);
	}
	else
	{
        updMut.unlock();
		return (false);
	}
}

void Vac::vacCallBack(const void *In, void *Out, unsigned long FrameCount, void *UserData)
{
	int outSamps;
    int tmp1, tmp2;
	int* ArrayPtr = (int *)Out;
	float* pOutR = (float *)ArrayPtr[0];
	float* pOutL = (float *)ArrayPtr[1];
	ArrayPtr = (int *)In;
	float* pInL = (float *)ArrayPtr[0];
	float* pInR = (float *)ArrayPtr[1];
	resVacToPa.process(pInL, pInR, FrameCount, pResBufInL, pResBufInR, outSamps);
	if(!updMut.tryLock(400))
	{
		qWarning() << "Vac: vacCallBack(): Can't lock mutex in 200 ms!";
		return;
	}
	tmp1 = pRbInL->writeCount();
	tmp2 = pRbInR->writeCount();
	if((pRbInL->writeCount() >= outSamps) && (pRbInR->writeCount() >= outSamps))
	{
		pRbInL->write(pResBufInL, outSamps);
		pRbInR->write(pResBufInR, outSamps);
		tmp1 = pRbInL->writeCount();
		tmp2 = pRbInR->writeCount();
	}
	if((pRbOutL->readCount() >= (int)FrameCount) && (pRbOutR->readCount() >= (int)FrameCount))
	{
		pRbOutL->read(pOutL, FrameCount);
		pRbOutR->read(pOutR, FrameCount);
	}
	else
	{
		memset(pOutL, 0, FrameCount * sizeof(float));
		memset(pOutR, 0, FrameCount * sizeof(float));
		qWarning() << "Vac out overflow!";
	}
	updMut.unlock();
}

void Vac::setInGain(float dB)
{
	inputGain = pow(10.0,0.05*(dB));
}

float Vac::inGain()
{
	float val = inputGain;
	if(val <= 0) val = 0.000000001;
		return (20*log10(val));
}

void Vac::setOutGain(float dB)
{
	outputGain = pow(10.0,0.05*(dB));
	qDebug() << "Set Vac out gain: " << outputGain;
}

float Vac::outGain()
{
	float val = outputGain;
	if(val <= 0) val = 0.000000001;
		return (20*log10(val));
}

void Vac::reset()
{
	pRbInL->clear();
	pRbInR->clear();
	pRbOutL->clear();
	pRbOutR->clear();
}

void Vac::resReset()
{
	resPaToVac.reset();
	resVacToPa.reset();
}
