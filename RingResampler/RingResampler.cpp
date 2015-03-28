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

#include "RingResampler.h"
#include <cstring>

RingResampler::RingResampler()
{
	isOpened = false;
	buffSize = 0;

	inGainValue  = 1;
	outGainValue = 1;

	pDataInputL  = 0;
	pDataInputR  = 0;
	pDataOutputL = 0;
	pDataOutputR = 0;

	resPaToVac  = 0;
	resVacToPa  = 0;

	pInDataL  = 0;
	pInDataR  = 0;
	pOutDataL = 0;
	pOutDataR = 0;

	pDataInputL  = new RingBuffer(0);
	pDataInputR  = new RingBuffer(0);
	pDataOutputL = new RingBuffer(0);
	pDataOutputR = new RingBuffer(0);

	resPaToVac  = new Resampler;
	resVacToPa  = new Resampler;

	pInDataL  = new float[MAX_DSP_BUFFER_SIZE];
	pInDataR  = new float[MAX_DSP_BUFFER_SIZE];
	pOutDataL = new float[MAX_DSP_BUFFER_SIZE];
	pOutDataR = new float[MAX_DSP_BUFFER_SIZE];
}

RingResampler::~RingResampler()
{
	if(pDataInputL != 0)
		delete pDataInputL;
	if(pDataInputR != 0)
		delete pDataInputR;
	if(pDataOutputL != 0)
		delete pDataOutputL;
	if(pDataOutputR != 0)
		delete pDataOutputR;

	if(resPaToVac != 0)
		delete resPaToVac;
	if(resVacToPa != 0)
		delete resVacToPa;

	if(pInDataL != 0)
		delete[] pInDataL;
	if(pInDataR != 0)
		delete[] pInDataR;
	if(pOutDataL != 0)
		delete[] pOutDataL;
	if(pOutDataR != 0)
		delete[] pOutDataR;
}

bool RingResampler::open(int inSampleRate, int outSampleRate)
{
	qDebug() << "RingResampler: open( " << inSampleRate << ", " << outSampleRate << " )";
	//
	paSampleRate  = outSampleRate;
	vacSampleRate = inSampleRate;
	if(vacSampleRate < paSampleRate)
		buffSize = qPow(2, 12 + ((int)(qLn(paSampleRate/vacSampleRate)/qLn(2.0f)) + 3));
	else if(vacSampleRate == paSampleRate)
		buffSize = MAX_DSP_BUFFER_SIZE*4;
	else
		buffSize = qPow(2, 12 + ((int)(qLn(vacSampleRate/paSampleRate)/qLn(2.0f)) + 3));

	if(!NUMBER_IS_2_POW_K(buffSize))
	{
		qWarning() << "RingResampler: open: incorrect buffer size = " << buffSize << " !";
		return (false);
	}
	resPaToVac->initialize(paSampleRate, vacSampleRate);
	resVacToPa->initialize(vacSampleRate, paSampleRate);
	pDataInputL->Resize(buffSize);
	pDataInputR->Resize(buffSize);
	pDataOutputL->Resize(buffSize);
	pDataOutputR->Resize(buffSize);
	pDataInputL->Clear();
	pDataInputR->Clear();
	pDataOutputL->Clear();
	pDataOutputR->Clear();
//	qMemSet(pInDataL,  0, MAX_DSP_BUFFER_SIZE*sizeof(float));
//	qMemSet(pInDataR,  0, MAX_DSP_BUFFER_SIZE*sizeof(float));
//	qMemSet(pOutDataL, 0, MAX_DSP_BUFFER_SIZE*sizeof(float));
//	qMemSet(pOutDataR, 0, MAX_DSP_BUFFER_SIZE*sizeof(float));
    std::memset(pInDataL,  0, MAX_DSP_BUFFER_SIZE*sizeof(float));
    std::memset(pInDataR,  0, MAX_DSP_BUFFER_SIZE*sizeof(float));
    std::memset(pOutDataL, 0, MAX_DSP_BUFFER_SIZE*sizeof(float));
    std::memset(pOutDataR, 0, MAX_DSP_BUFFER_SIZE*sizeof(float));
	isOpened = true;
	return (true);
}

void RingResampler::close()
{
	isOpened = false;
}

bool RingResampler::setData(float *pBufL, float *pBufR, int size)
{
	if(!isOpened)
		return (false);

	qDebug() << "RingResampler: setData()";
	int outSize = 0;
	if(!resVacToPa->process(pBufL, pBufR, size, pOutDataL, pOutDataR, outSize))
	{
		qWarning() << "RingResampler: setData: Can't resampled input data!";
		return (false);
	}
	for(int i = 0; i < size; i++)
	{
		pOutDataL[i] *= outGainValue;
		pOutDataR[i] *= outGainValue;
	}
	mutexVacToPa.lock();
	if(!pDataOutputL->setData(pOutDataL, outSize) || !pDataOutputR->setData(pOutDataR, outSize))
	{
		qWarning() << "RingResampler: setData: overflow data!";
		return (false);
	}
	mutexVacToPa.unlock();
	return (true);
}

bool RingResampler::getData(float *pBufL, float *pBufR, int size)
{
	if(!isOpened)
		return (false);

	mutexPaToVac.lock();
	if(!pDataInputL->getData(pBufL, size) || !pDataInputR->getData(pBufR, size))
	{
		qWarning() << "RingResampler: getData: deficit data!";
		return (false);
	}
	mutexPaToVac.unlock();
	for(int i = 0; i < size; i++)
	{
		pBufL[i] *= inGainValue;
		pBufR[i] *= inGainValue;
	}
	return (true);
}

void RingResampler::setInGain(float dB)
{

}

void RingResampler::setOutGain(float dB)
{

}

float RingResampler::inGain()
{

}

float RingResampler::outGain()
{

}

void RingResampler::paProcess(const void *In, void *Out, unsigned long FrameCount, void *UserData)
{
	if(!isOpened)
		return;

	int* ArrayPtr = (int *)In;
	float* pInL = (float *)ArrayPtr[0];
	float* pInR = (float *)ArrayPtr[1];
	ArrayPtr = (int *)Out;
	float* pOutR = (float *)ArrayPtr[0];
	float* pOutL = (float *)ArrayPtr[1];
	int outSize = 0;
	resPaToVac->process(pInL, pInR, FrameCount, pInDataL, pInDataR, outSize);
	for(int i = 0; i < outSize; i++)
	{
		pInDataL[i] *= inGainValue;
		pInDataR[i] *= inGainValue;
	}

	mutexPaToVac.lock();
	if(!pDataInputL->setData(pInDataL, outSize) || !pDataInputR->setData(pInDataL, outSize))
		qWarning() << "RingResampler: paProcess: overflow data!";

	mutexPaToVac.unlock();
	mutexVacToPa.lock();

	if(!pDataInputL->getData(pOutL, FrameCount) || !pDataInputR->getData(pOutR, FrameCount))
		qWarning() << "RingResampler: paProcess: deficit data!";

	mutexVacToPa.unlock();
}
