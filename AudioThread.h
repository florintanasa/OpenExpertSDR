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
#ifndef AUDIOTHREAD_H_
#define AUDIOTHREAD_H_

#include <search.h>
#include <string.h>
#include <QtWidgets>
#include "ringBuf.h"
#include "expertsdr_va2_1.h"

class ExpertSDR_vA2_1;

class AudioThread : public QThread
{
	public:
		AudioThread(void *UsrData = 0, QObject *parent = 0);
		~AudioThread();

		void setFrameSize(int size);
		void process(void *pIn, void *pOut, int size);

	private:
		ringBuf	*pInR;
		ringBuf	*pInL;
		ringBuf	*pOutR;
		ringBuf	*pOutL;
		void *userData;
		QSemaphore sem;
		DttSP *pDsp;
		ExpertSDR_vA2_1 *pExpSdr;
		int bufSize;
		void run();
};

#endif /* AUDIOTHREAD_H_ */
