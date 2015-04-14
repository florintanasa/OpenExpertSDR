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

#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <QtGlobal>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include <QMutex>

#include "../Defines.h"

class RingBuffer
{
	int Size;
	float *pData;

	int Mask;
	int setPtr;
	int getPtr;

	QMutex mutex;

public:
	RingBuffer(int size);
	~RingBuffer();

	bool Resize(int size);
	void Clear();

	bool setData(float *pBuf, int size);
	bool getData(float *pBuf, int size);

	bool availableWriteData(int size);
	bool availableReadData(int size);

};

#endif // RINGBUFFER_H
