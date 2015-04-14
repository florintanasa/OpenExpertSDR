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

#ifndef VAC_H_
#define VAC_H_

#include <QtGlobal>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "DttSP.h"
#include "ringBuf.h"
#include "Resampler.h"

#define	VAC_BUFFER_SIZE		4096*48

class Vac
{

	float	*pResBufInL;
	float	*pResBufInR;
	float	*pResBufOutL;
	float	*pResBufOutR;

	float   *pBufInL;
	float   *pBufInR;

	ringBuf	*pRbInL;
	ringBuf	*pRbInR;
	ringBuf	*pRbOutL;
	ringBuf	*pRbOutR;

	float SampleRatePa;
	float SampleRateVac;

	bool  isOpen;
	bool  isStereo;

	float inputGain;
	float outputGain;

	uint Size;

	QMutex updMut;

	Resampler resPaToVac;
	Resampler resVacToPa;

public:
	Vac();
	~Vac();

	friend void writeVacBuf(float *pOutL, float *pOutR, int size, Vac *p);

	void open(float inputSampleRate, float outputSampleRate);
	void open(float inputSampleRate, int inBuffSize, float outputSampleRate, int outBuffSize);
	void close();
	bool writeBuf(float *pOutL, float *pOutR, int size);
	bool readBuf(float *pInL, float *pInR, int size);
	void setInGain(float dB);
	void setOutGain(float dB);
	float inGain();
	float outGain();

	void reset();
	void resReset();

	void vacCallBack(const void *In, void *Out, unsigned long FrameCount, void *UserData);
};




#endif /* VAC_H_ */

