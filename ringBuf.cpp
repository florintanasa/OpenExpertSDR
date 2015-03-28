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

#include "ringBuf.h"

ringBuf::ringBuf(int size)
{
	if(size < 0)
		size  = 0;
	Size = size;
	Mask = size - 1;
	pBuffer = new float[size];
	wPtr = 0;
	rPtr = 0;
}

ringBuf::~ringBuf()
{
	delete[] pBuffer;
}

void ringBuf::setSize(int size)
{
	updMut.lock();
	delete[] pBuffer;
	pBuffer = new float[size];
	Size = size;
	Mask = size - 1;
	rPtr  = 0;
	wPtr = 0;
	updMut.unlock();
}

void ringBuf::write(float *pBuf, int size)
{
	if(size > writeCount())
	{
		qDebug() << "RingBuffer write operation is not avaliable!";
		return;
	}
	updMut.lock();
	for(int i = 0; i < size; i++)
	{
		pBuffer[wPtr & Mask] = pBuf[i];
		wPtr++;
	}
    wPtr &= Mask;
    updMut.unlock();
}

void ringBuf::read(float *pBuf, int size)
{
	if(size > readCount())
	{
		qDebug() << "RingBuffer read operation is not avaliable!";
		return;
	}
	updMut.lock();
	for(int i = 0; i < size; i++)
	{
		pBuf[i] = pBuffer[rPtr & Mask];
		rPtr++;
	}
    rPtr &= Mask;
    updMut.unlock();
}

void ringBuf::clear()
{
	updMut.lock();
	wPtr = 0;
	rPtr = 0;
	memset(pBuffer, 0.0f, Size*sizeof(float));
	updMut.unlock();
}

int ringBuf::writeCount()
{
	if(wPtr > rPtr)
		return ((Size - wPtr + rPtr) & Mask) - 1;
	else if(wPtr < rPtr)
		return (rPtr - wPtr - 1);
	else
		return (Size - 1);
}

int ringBuf::readCount()
{
	if(wPtr > rPtr)
		return wPtr - rPtr;
	else
		return (Size - rPtr + wPtr) & Mask;

}
