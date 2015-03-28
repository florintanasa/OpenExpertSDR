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

#include "RingBuffer.h"
#include <cstring>

RingBuffer::RingBuffer(int size)
{
	pData = 0;
	setPtr = 1;
	getPtr = 0;
	Mask = 0;
	if(!NUMBER_IS_2_POW_K(size))
		Size = 0;
	else
	{
		Size = size;
		Mask = Size-1;
		pData = new float[Size];
	}
	Clear();
}

RingBuffer::~RingBuffer()
{
	if(pData != 0)
		delete[] pData;
}

bool RingBuffer::Resize(int size)
{
	if(!NUMBER_IS_2_POW_K(size))
		return (false);

	//mutex.lock();
	//QMutexLocker locker(&mutex);
	if(pData != 0)
		delete[] pData;

	setPtr = 1;
	getPtr = 0;
	Size = size;
	Mask = Size-1;
	pData = new float[Size];
	//mutex.unlock();
	return (true);
}

void RingBuffer::Clear()
{
	//QMutexLocker locker(&mutex);
	if(Size > 0)
	{
		//mutex.lock();

        //qMemSet(pData, 0, Size*sizeof(float));
        std::memset(pData, 0, Size*sizeof(float));
		//mutex.unlock();
	}
	setPtr = 1;
	getPtr = 0;
}

bool RingBuffer::setData(float *pBuf, int size)
{
	// добавляем данные
	if(Size < 0)
		return (false);
	// проверка на переполнение кольцевого буффера
	//if(!availableWriteData(size))
		//return (false);

	// запись данных
	//mutex.lock();
	//QMutexLocker locker(&mutex);
	for(int i = 0; i < size; i++)
	{
		pData[i] = pBuf[setPtr & Mask];
		setPtr++;
	}
	setPtr &= Mask;
	//mutex.unlock();
	return (true);
}

bool RingBuffer::getData(float *pBuf, int size)
{
	// забираем данные
	if(Size < 0)
		return (false);
	// проверка количества доступных данных
	//if(!availableReadData(size))
		//return (false);

	// чтение данных
	//mutex.lock();
	//QMutexLocker locker(&mutex);
	for(int i = 0; i < size; i++)
	{
		pBuf[i] = pData[getPtr & Mask];
		getPtr++;
	}
	getPtr &= Mask;
	//mutex.unlock();
	return (true);
}

bool RingBuffer::availableWriteData(int size)
{
	//QMutexLocker locker(&mutex);
	// проверка на переполнение кольцевого буффера
	if((setPtr < getPtr) && ((getPtr - setPtr -1) < size))	// если указатель записи до записи был меньше а после стал больше
	{
		qDebug() << "RingBuffer::availableWriteData: available size = " << (getPtr - setPtr -1) << "  need size = " << size;
		return (false);
	}
	else if((setPtr > getPtr) && ((Size + getPtr - setPtr -1) < size))	// если указатель записи до и после записи данных больше чем указатель чтения
	{
		qDebug() << "RingBuffer::availableWriteData: available size = " << (Size + getPtr - setPtr -1) << "  need size = " << size;
		return (false);
	}
	return (true);
}

bool RingBuffer::availableReadData(int size)
{
	//QMutexLocker locker(&mutex);
	if((setPtr > getPtr) && ((setPtr - getPtr) < size))
	{
		qDebug() << "RingBuffer::availableReadData: available size = " << (setPtr - getPtr) << "  need size = " << size;
		return (false);
	}
	else if((setPtr < getPtr) && ((Size - getPtr + setPtr) < size))
	{
		qDebug() << "RingBuffer::availableReadData: available size = " << (Size - getPtr + setPtr) << "  need size = " << size;
		return (false);
	}
	return (true);
}

























